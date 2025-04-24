#include "Server.h"

namespace tweet
{
    grpc::Status Server::Tweet(
        grpc::ServerContext* context,
        const proto::TweetIn* request,
        proto::TweetOut* response)
    {
        bool value = storage_->Tweet(
            request->token(),
            request->content());
        response->set_error(value);
        return grpc::Status::OK;
    }

    grpc::Status Server::Follow(
        grpc::ServerContext* context,
        const proto::FollowIn* request,
        proto::FollowOut* response)
    {
        bool value = storage_->Follow(
            request->token(),
            request->name());
        response->set_error(value);
        return grpc::Status::OK;
    }

    grpc::Status Server::Show(
        grpc::ServerContext* context,
        const proto::ShowIn* request,
        proto::ShowOut* response)
    {
        const auto tweets = storage_->Show(
            request->token(),
            request->user());
        for (const auto& tweet : tweets)
        {
            proto::TweetIn tweet_in;
            tweet_in.set_user(tweet.name);
            tweet_in.set_content(tweet.text);
            tweet_in.set_time(tweet.time);
            *response->add_tweets() = tweet_in;
        }
        response->set_error(true);
        return grpc::Status::OK;
    }

    grpc::Status Server::Login(
        grpc::ServerContext* context,
        const proto::LoginIn* request,
        proto::LoginOut* response)
    {
        auto maybe_token = storage_->Login(
            request->user(),
            request->pass());
        response->set_error(maybe_token.has_value());
        response->set_token(maybe_token.value());
        return grpc::Status::OK;
    }

    grpc::Status Server::Logout(
        grpc::ServerContext* context,
        const proto::LogoutIn* request,
        proto::LogoutOut* response)
    {
        bool value = storage_->Logout(
            request->token());
        response->set_error(value);
        return grpc::Status::OK;
    }

    grpc::Status Server::Register(
        grpc::ServerContext* context,
        const proto::RegisterIn* request,
        proto::RegisterOut* response)
    {
        auto maybe_token =
            storage_->Register(
                request->name(),
                request->pass());
        response->set_error(maybe_token.has_value());
        response->set_token(maybe_token.value());
        return grpc::Status::OK;
    }

    grpc::Status Server::Update(
        grpc::ServerContext* context,
        const proto::UpdateIn* update_in,
        grpc::ServerWriter<proto::UpdateOut>* update_out_writer)
    {
        {
            std::lock_guard<std::mutex> lock(writers_mutex_);
            writers_.insert({update_in->token(), update_out_writer});
        }
        while (!context->IsCancelled())
        {
            Sleep(100);
        }
        std::lock_guard<std::mutex> lock(writers_mutex_);
        writers_.erase(update_in->token());
        return grpc::Status::OK;
    }

    void Server::ProceedAsync()
    {
        bool proceed = true;
        std::map<int64_t, std::int64_t> user_start_time;
        while (proceed)
        {
            Sleep(10);
            std::scoped_lock lock(writers_mutex_);
            for (const auto& [key, value] : writers_)
            {
                if (!user_start_time.contains(key))
                {
                    user_start_time.insert({key, 0});
                }
                proto::UpdateOut update_out;
                std::multimap<std::int64_t, proto::TweetIn> tweets_map;
                std::string name = storage_->GetNameFromToken(key);
                auto follows = storage_->GetFollowerList(name);
                for (const auto& follow : follows)
                {
                    auto tweets = storage_->GetTweetsFromNameTime(follow, user_start_time.at(key));
                    for (const auto& tweet : tweets)
                    {
                        proto::TweetIn tweet_in;
                        tweet_in.set_user(tweet.name);
                        tweet_in.set_content(tweet.text);
                        tweet_in.set_time(tweet.time);
                        tweets_map.insert({tweet.time, tweet_in});
                    }
                }
                auto time = user_start_time.at(key);
                for (const auto& [time ,value] : tweets_map)
                {
                    if (value.time() >= time)
                    {
                        *update_out.add_tweets() = value;
                        user_start_time.at(key) = std::max(user_start_time.at(key), value.time());
                    }
                }
                if (update_out.tweets_size())
                {
                    for (auto& writer : writers_)
                    {
                        writer.second->Write(update_out);
                    }
                }
            }
        }
    }
} // End namespace tweet.

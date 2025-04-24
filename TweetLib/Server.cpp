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
} // End namespace tweet.

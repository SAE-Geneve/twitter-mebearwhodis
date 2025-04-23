#include "Server.h"

namespace tweet
{
    grpc::Status Server::Tweet(
        grpc::ServerContext* context,
        const proto::TweetIn* request,
        proto::TweetOut* response)
    {
        
        std::string client = context->peer().substr(0, 
            context->peer().find_last_of(':'));
        
        bool value = storage_->Tweet(
            client,
            request->content());
        response->set_error(value);
        return grpc::Status::OK;
    }

    grpc::Status Server::Follow(
        grpc::ServerContext* context,
        const proto::FollowIn* request,
        proto::FollowOut* response)
    {
        
        std::string client = context->peer().substr(0, 
            context->peer().find_last_of(':'));
        
        bool value = storage_->Follow(
            client,
            request->name());
        response->set_error(value);
        return grpc::Status::OK;
    }

    grpc::Status Server::Show(
        grpc::ServerContext* context,
        const proto::ShowIn* request,
        proto::ShowOut* response)
    {
        std::string client = context->peer().substr(0, 
            context->peer().find_last_of(':'));
        
        const auto tweets = storage_->Show(
            client,
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
        std::string client = context->peer().substr(0, 
            context->peer().find_last_of(':'));
        
        bool value = storage_->Login(
            client,
            request->user(),
            request->pass());
        response->set_error(value);
        return grpc::Status::OK;
    }

    grpc::Status Server::Logout(
        grpc::ServerContext* context,
        const proto::LogoutIn* request,
        proto::LogoutOut* response)
    {
        std::string client = context->peer().substr(0, 
            context->peer().find_last_of(':'));
        
        bool value = storage_->Logout(
            client);
        response->set_error(value);
        return grpc::Status::OK;
    }

    grpc::Status Server::Register(
        grpc::ServerContext* context,
        const proto::RegisterIn* request,
        proto::RegisterOut* response)
    {
        std::string client = context->peer().substr(0, 
            context->peer().find_last_of(':'));
        
        bool value = storage_->Register(
            client,
            request->name(),
            request->pass());
        response->set_error(value);
        return grpc::Status::OK;
    }
} // End namespace tweet.

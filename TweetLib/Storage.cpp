#include "Storage.h"
#include <algorithm>
#include <iterator>
#include <chrono>

namespace tweet {

	Storage::Storage() : engine_(std::random_device{}()), dist_(std::numeric_limits<std::int64_t>::min(), std::numeric_limits<std::int64_t>::max())
	{

	}

	bool Storage::Tweet(std::int64_t token, const std::string& text)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = token_names_.find(token);
		if (it == token_names_.end())
		{
			return false;
		}
		// Create a tweet with user name current.
		TweetValue tv{};
		tv.name = it->second;
		auto now = std::chrono::system_clock::now();
		auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
		auto value = now_s.time_since_epoch();
		tv.time = value.count();
		tv.text = text;
		name_tweets_.insert({ it->second, tv });
		return true;
	}

	bool Storage::Follow(std::int64_t token, const std::string& name)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = token_names_.find(token);
		if (it == token_names_.end())
		{
			return false;
		}
		// Check if user name is not current one.
		if (name == it->second)
		{
			return false;
		}
		// Check if user name is existing.
		auto it_user = name_passes_.find(name);
		if (it_user == name_passes_.end())
		{
			return false;
		}
		// Check if this user is already registered in the DB.
		auto range = followers_.equal_range(it->second);
		if (std::find_if(range.first, range.second, [name](const auto& value)
		{
			return value.second == name;
		}) == range.second)
		{
			followers_.insert({ it->second, name });
			return true;
		}
		return false;
	}

	const std::vector<TweetValue> Storage::Show(
		std::int64_t token,
		const std::string& name)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.

		if (!token_names_.contains(token))
		{
			return {};
		}
		bool found = false;
		auto it = token_names_.find(token);
		// Check if current user.
		if (it->second == name)
		{
			found = true;
		}
		else
		{
			// Check if the follower is in.
			auto range = followers_.equal_range(it->second);
			found = std::find_if(
				range.first,
				range.second,
				[name](const auto& value)
			{
				return value.second == name;
			}) != range.second;
		}
		// If the follower is in or this is current user.
		if (found)
		{
			// Transform the current range into a vector.
			std::vector<TweetValue> ret{};
			auto tweet_range = name_tweets_.equal_range(name);
			std::transform(
				tweet_range.first,
				tweet_range.second,
				std::back_inserter(ret),
				[](const auto& value)
			{
				return value.second;
			});
			return ret;
		}
		// Not found so return empty vector.
		return {};
	}

	std::optional<std::int64_t> Storage::Login(
		const std::string& name, 
		const std::string& pass)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = name_passes_.find(name);
		if (it == name_passes_.end())
		{
			return std::nullopt;
		}
		// Check the password.
		if (pass == it->second)
		{
			auto token = GenerateToken();
			token_names_.insert({ token, name });
			return token;
		}
		return std::nullopt;
	}

	bool Storage::Logout(std::int64_t token)
	{
		std::scoped_lock l(mutex_);
		auto it = token_names_.find(token);
		if (it == token_names_.end())
		{
			return false;
		}
		token_names_.erase(it);
		return true;
	}

	std::optional<std::int64_t> Storage::Register(
		const std::string& name, 
		const std::string& pass)
	{
		std::scoped_lock l(mutex_);
		// Check if you are already login.
		auto it = name_passes_.find(name);
		if (it != name_passes_.end())
		{
			return std::nullopt;
		}
		name_passes_.insert({ name, pass });
		auto token = GenerateToken();
		token_names_.insert({ token, name });
		return true;
	}

	std::int64_t Storage::GenerateToken()
	{
		return dist_(engine_);
	}
} // End namespace tweet.

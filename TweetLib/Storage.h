#pragma once

#include <string>
#include <mutex>
#include <random>
#include <unordered_map>

namespace tweet {

	struct TweetValue 
	{
		std::string name;
		std::string text;
		int64_t time;
	};

	class Storage 
	{
	public:
		Storage();
		bool Tweet(
			std::int64_t token,
			const std::string& text);
		bool Follow(
			std::int64_t token,
			const std::string& name);
		const std::vector<TweetValue> Show(
			std::int64_t token,
			const std::string& name);
		std::optional<std::int64_t> Login(
			const std::string& name,
			const std::string& pass);
		bool Logout(std::int64_t token);
		std::optional<std::int64_t> Register(
			const std::string& name,
			const std::string& pass);
		std::int64_t GenerateToken();
		std::string GetNameFromToken(std::int64_t token) const;
		std::vector<std::string> GetFollowerList(const std::string& name) const;
		std::vector<TweetValue> GetTweetsFromNameTime(const std::string& name, std::int64_t time_s) const;

	protected:
		std::mutex mutex_ = {};
		std::mt19937_64 engine_ = {};
		std::uniform_int_distribution<std::int64_t> dist_;
		std::unordered_multimap<std::string, TweetValue> name_tweets_ = {};
		std::unordered_multimap<std::string, std::string> followers_ = {};
		std::unordered_map<std::int64_t, std::string> token_names_ = {};
		std::unordered_map<std::string, std::string> name_passes_ = {};
	};

} // End namespace tweet.
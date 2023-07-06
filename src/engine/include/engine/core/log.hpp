// log.h
#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <signal.h>

namespace Log
{
	void init();
	std::shared_ptr<spdlog::logger>& get_logger();
}

#define LOG_ERROR(...) ::Log::get_logger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::Log::get_logger()->warn(__VA_ARGS__)
#define LOG_INFO(...) ::Log::get_logger()->info(__VA_ARGS__)
#define LOG_TRACE(...) ::Log::get_logger()->trace(__VA_ARGS__)

#define DEBUG_BREAK() raise(SIGTRAP)

#define ENABLE_ASSERTIONS

#ifdef ENABLE_ASSERTIONS

#define ASSERT(check, message) \
	if (!(check)) \
	{ \
		LOG_ERROR("Assertion Failed: {0} (file '{1}', line {2})\nMessage: {3}", \
		#check, __FILE__, __LINE__, message); \
		DEBUG_BREAK(); \
	}

#else

#define ASSERT(...)

#endif

// log.h
#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Log
{
	void init();
	std::shared_ptr<spdlog::logger>& get_logger();
}

#define LOG_ERROR(...) ::Log::get_logger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::Log::get_logger()->warn(__VA_ARGS__)
#define LOG_INFO(...) ::Log::get_logger()->info(__VA_ARGS__)
#define LOG_TRACE(...) ::Log::get_logger()->trace(__VA_ARGS__)

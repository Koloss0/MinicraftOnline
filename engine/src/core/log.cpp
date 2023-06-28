#include <engine/core/log.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Log
{
	std::shared_ptr<spdlog::logger> logger;

	void init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		
		logger = spdlog::stdout_color_mt("MCO");
		logger->set_level(spdlog::level::trace);	
	}

	std::shared_ptr<spdlog::logger>& get_logger()
	{
		return logger;
	}
}

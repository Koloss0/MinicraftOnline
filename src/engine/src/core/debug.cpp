#include <engine/core/debug.hpp>
#include <engine/core/log.hpp>

#include <unordered_map>

namespace Debug
{
	static std::unordered_map<std::string, unsigned long> counters{};

	void reset_counter(const std::string& name)
	{
		counters[name] = 0;
	}

	void increment_counter(const std::string& name)
	{
		if (counters.find(name) == counters.end())
		{
			counters[name] = 0;
		}

		counters[name]++;
	}

	void log_counter(const std::string& name)
	{
		if (counters.find(name) == counters.end())
		{
			LOG_TRACE("DEBUG: Counter '{0}' does not have a count", name);
		}
		else
		{
			LOG_TRACE("DEBUG: [Counter: {0}]: {1}", name, counters[name]);
		}
	}
}

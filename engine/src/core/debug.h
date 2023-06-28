#pragma once

#include <string>

namespace Debug
{
	void reset_counter(const std::string& name);

	void increment_counter(const std::string& name);

	void log_counter(const std::string& name);
}

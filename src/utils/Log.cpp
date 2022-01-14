#include <chrono>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>

#include <phase2/utils/Log.hpp>

namespace phase2
{

	std::mutex log_lock;
	Log<LogLevel::LOG_ERROR> log_error;
	Log<LogLevel::LOG_WARNING> log_warning;
	Log<LogLevel::LOG_INFO> log_info;
	Log<LogLevel::LOG_DEBUG> log_debug;

	std::string to_string(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::LOG_ERROR:
			return "[" TTY_RED "ERROR" TTY_RESET "]  ";
		case LogLevel::LOG_WARNING:
			return "[" TTY_ORANGE "WARNING" TTY_RESET "]";
		case LogLevel::LOG_INFO:
			return "[" TTY_BLUE "INFO" TTY_RESET "]   ";
		case LogLevel::LOG_DEBUG:
			return "[" TTY_GRAY "DEBUG" TTY_RESET "]  ";
		}

		log_error << "unknown log type";
		std::terminate();
	}

};

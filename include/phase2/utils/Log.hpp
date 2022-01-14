#pragma once

#include <cstdint>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#define TTY_RED "\x1B[38;5;204m"
#define TTY_ORANGE "\x1B[38;5;214m"
#define TTY_GREEN "\x1B[38;5;37m"
#define TTY_BLUE "\x1B[38;5;75m"
#define TTY_GRAY "\x1B[38;5;243m"
#define TTY_RESET "\x1B[0m"

namespace phase2
{

	enum class LogLevel
	{
		LOG_ERROR,
		LOG_WARNING,
		LOG_INFO,
		LOG_DEBUG
	};

	/**
	 * @brief Convert log level to string.
	 *
	 * @param level log level.
	 * @return The string.
	 */
	std::string to_string(LogLevel level);

#ifdef NDEBUG
	constexpr LogLevel _global_level = LogLevel::LOG_INFO;
#else
	constexpr LogLevel _global_level = LogLevel::LOG_DEBUG;
#endif
	extern std::mutex log_lock;

	template <LogLevel level>
	class Log
	{
	public:
		template <typename T>
		void log(const T &data) const
		{
			if constexpr (_global_level < level)
				return;

			auto &&now   = std::chrono::system_clock::now();
			auto &&now_c = std::chrono::system_clock::to_time_t(now);
			char time_buf[128];
			strftime(time_buf, sizeof(time_buf),
					 "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
			{
				std::lock_guard<std::mutex> guard{log_lock};
				std::cerr << to_string(level) << ' ' << time_buf << " | " << data << '\n';
			}
		}
	};

	extern Log<LogLevel::LOG_ERROR> log_error;
	extern Log<LogLevel::LOG_WARNING> log_warning;
	extern Log<LogLevel::LOG_INFO> log_info;
	extern Log<LogLevel::LOG_DEBUG> log_debug;

	template <LogLevel level>
	class LogBuffer
	{
	public:
		LogBuffer() = default;
		~LogBuffer()
		{
			if constexpr (level == LogLevel::LOG_DEBUG)
				log_debug.log(ss.str());
			else if constexpr (level == LogLevel::LOG_WARNING)
				log_warning.log(ss.str());
			else if constexpr (level == LogLevel::LOG_INFO)
				log_info.log(ss.str());
			else
				log_debug.log(ss.str());
		}

		LogBuffer(const LogBuffer &) = delete;
		LogBuffer &operator=(const LogBuffer &) = delete;

		LogBuffer(LogBuffer &&buf) : ss(std::move(buf.ss)) {}
		LogBuffer &operator=(LogBuffer &&) = delete;

		template <typename T>
		LogBuffer &operator<<(T &&data)
		{
			ss << std::forward<T>(data);
			return *this;
		}

	protected:
		std::stringstream ss;
	};

	template <LogLevel level, typename T>
	LogBuffer<level> operator<<(Log<level> log, T &&data)
	{
		LogBuffer<level> buf;
		buf << std::forward<T>(data);
		return buf;
	}

	/**
	 * @brief Print a buffer.
	 *
	 * @param os the output stream to print to.
	 * @param buffer the buffer to print.
	 * @return The reference of os.
	 */
	std::ostream &operator<<(std::ostream &os, const std::vector<std::uint8_t> buffer);

};

//
// Created by liu_zongchang on 2025/1/9 2:52.
// Email 1439797751@qq.com
// 
//

#ifndef MICROSCOPE_UTILS_LOG_H
#define MICROSCOPE_UTILS_LOG_H

#pragma once
#include <mutex>
#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

#define logApp Microscope_Utils_Log::getInstance()
#define LOG_DEFAULT_LEVEL Microscope_Utils_Log::Level::Debug
#define LOG_DEFAULT_NAME "Microscope_Utils_Log"
#define LOG_TRACE(...) SPDLOG_LOGGER_CALL(Microscope_Utils_Log::getInstance()->getLogger().get(), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_CALL(Microscope_Utils_Log::getInstance()->getLogger().get(), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_CALL(Microscope_Utils_Log::getInstance()->getLogger().get(), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_CALL(Microscope_Utils_Log::getInstance()->getLogger().get(), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_CALL(Microscope_Utils_Log::getInstance()->getLogger().get(), spdlog::level::err, __VA_ARGS__)

class Microscope_Utils_Log {
public:
    enum Level {
		Trace,
		Debug,
		Info,
		Warn,
		Error,
	};

	enum Mode
	{
		Sync,
		Async,
	};

	enum SinkType
	{
		Console = 0x1,
		File = 0x2,
	};

    static Microscope_Utils_Log *getInstance();

	std::shared_ptr<spdlog::logger> getLogger();
	void init(const std::string &prefix = LOG_DEFAULT_NAME, Level level = LOG_DEFAULT_LEVEL, Mode mode = Sync, int sinkType = Console | File, const std::string &logPath = "./log", int maxFileSize = 1024 * 1024 * 10, int maxFileNum = 10);
	void reset();
private:
	static Microscope_Utils_Log *m_instance;
	static std::mutex m_mutex;
	Microscope_Utils_Log();
	~Microscope_Utils_Log();
	Microscope_Utils_Log(const Microscope_Utils_Log &) = delete;
	Microscope_Utils_Log &operator=(const Microscope_Utils_Log &) = delete;
	bool m_inited = false;

	std::shared_ptr<spdlog::logger> m_logger;
};



#endif //MICROSCOPE_UTILS_LOG_H

//
// Created by liu_zongchang on 2025/1/9 2:52.
// Email 1439797751@qq.com
// 
//

#include "Microscope_Utils_Log.h"

#include <filesystem>
#include <iostream>

Microscope_Utils_Log *Microscope_Utils_Log::m_instance = nullptr;
std::mutex Microscope_Utils_Log::m_mutex;

Microscope_Utils_Log* Microscope_Utils_Log::getInstance()
{
    if (m_instance == nullptr) {
        m_mutex.lock();
        if (m_instance == nullptr) {
            m_instance = new Microscope_Utils_Log();
        }
        m_mutex.unlock();
    }
    return m_instance;
}


Microscope_Utils_Log::Microscope_Utils_Log()
{
    m_logger = spdlog::stdout_color_mt("console");
    m_logger->set_level(spdlog::level::debug);
    m_inited = false;
}

Microscope_Utils_Log::~Microscope_Utils_Log()
{
    spdlog::drop_all();
    spdlog::shutdown();
}


std::shared_ptr<spdlog::logger> Microscope_Utils_Log::getLogger()
{
    return m_logger;
}

void Microscope_Utils_Log::init(const std::string& prefix, Level level, const Mode mode, const int sinkType,
    const std::string& logPath, const int maxFileSize, const int maxFileNum)
{
    if (m_inited) {
        return;
    }

    m_inited = true;
    try
    {
        std::vector<spdlog::sink_ptr> vecSink;

        if (sinkType & Console) {
            const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] <%t> [%!] [%l]: %^%v%$");
            vecSink.push_back(console_sink);
        }

        if (sinkType & File) {
            if (const std::filesystem::path log_dir(logPath); !exists(log_dir)) {
                create_directories(log_dir);
            }

            const auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath + "/" + prefix + ".log", maxFileSize, maxFileNum);
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] <%t> [%@,%!] [%l]: %^%v%$");
            file_sink->set_level(static_cast<spdlog::level::level_enum>(Trace));
            vecSink.push_back(file_sink);
        }

        if (mode == Async)
        {
            spdlog::init_thread_pool(102400, 1);
            auto tp = spdlog::thread_pool();
            m_logger = std::make_shared<spdlog::async_logger>(LOG_DEFAULT_NAME, begin(vecSink), end(vecSink), tp, spdlog::async_overflow_policy::block);
        }
        else
        {
            m_logger = std::make_shared<spdlog::logger>(LOG_DEFAULT_NAME, begin(vecSink), end(vecSink));
        }
        m_logger->set_level(static_cast<spdlog::level::level_enum>(level));
        m_logger->flush_on(spdlog::level::warn);
        spdlog::flush_every(std::chrono::seconds(5));
        register_logger(m_logger);

    } catch (const spdlog::spdlog_ex& ex) {
        m_inited = false;
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

void Microscope_Utils_Log::reset()
{
    m_inited = false;
    init();
}
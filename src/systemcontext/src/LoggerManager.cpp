#include "LoggerManager.h"
#include <iostream>
#include <cstring> // 用于 strrchr
#include <filesystem>

// 添加 Windows 版本定义
#if defined(_WIN32) || defined(_WIN64)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601 // Windows 7
#endif
#endif

LoggerManager &LoggerManager::getInstance()
{
    static LoggerManager instance;
    return instance;
}

bool LoggerManager::initialize(const std::string &logDir,
                               spdlog::level::level_enum level,
                               bool enableConsole)
{
    std::lock_guard<std::mutex> lock(init_mutex_);

    if (initialized_)
    {
        LOG_WARN("Logger system already initialized");
        return true;
    }

    try
    {
        // 确保日志目录存在
        std::filesystem::create_directories(logDir);

        std::vector<spdlog::sink_ptr> sinks;

        // 创建文件sink
        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            logDir + "/app.log", 23, 59); // 每天23:59轮换

        // 设置文件日志格式
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [thread %t] [%l] [%n] %v");

        sinks.push_back(file_sink);

        // 创建控制台sink（可选）
        if (enableConsole)
        {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");
            sinks.push_back(console_sink);
        }

        // 创建组合logger
        logger_ = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());

        // 设置日志级别
        logger_->set_level(level);

        // 错误级别以上立即刷新
        logger_->flush_on(spdlog::level::info);

        // 注册logger
        spdlog::register_logger(logger_);

        // 设置全局异常处理器
        spdlog::set_error_handler([](const std::string &msg)
                                  { std::cerr << "Logger error: " << msg << std::endl; });

        initialized_ = true;

        LOG_INFO("Logger system initialized successfully");
        LOG_INFO("Log level: {}, Log directory: {}",
                 spdlog::level::to_string_view(level), logDir);

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
        // 创建回退logger
        try
        {
            logger_ = spdlog::stdout_color_mt("fallback");
            logger_->set_level(level);
            initialized_ = true;
            LOG_ERROR("Using fallback console logger due to initialization failure: {}", e.what());
            return false;
        }
        catch (...)
        {
            std::cerr << "Critical: Failed to create fallback logger" << std::endl;
            return false;
        }
    }
}

void LoggerManager::shutdown()
{
    std::lock_guard<std::mutex> lock(init_mutex_);

    if (initialized_ && logger_)
    {
        LOG_INFO("Shutting down logger system");
        flush();
        spdlog::drop_all();
        logger_.reset();
        initialized_ = false;
    }
}

void LoggerManager::setLevel(spdlog::level::level_enum level)
{
    if (logger_)
    {
        logger_->set_level(level);
        LOG_INFO("Log level changed to: {}", spdlog::level::to_string_view(level));
    }
}

void LoggerManager::flush()
{
    if (logger_)
    {
        logger_->flush();
    }
}

LoggerManager::~LoggerManager()
{
    shutdown();
}
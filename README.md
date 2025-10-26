# Orange
Qt项目

## 1. 前期配置

### 1.1 配置vscode+Qt+cmake环境

[(90 封私信 / 80 条消息) 在VSCode中优雅使用Qt6.7.3 - 知乎](https://zhuanlan.zhihu.com/p/1939453153671098791)

### 1.2 配置github和git

[详细介绍：Github如何上传项目(超详细小白教程) - wzzkaifa - 博客园](https://www.cnblogs.com/wzzkaifa/p/19002011)

### 1.3 通过.gitignore忽略文件或文件夹

[一分钟学会gitignore（附配置规则）.gitignore 是一个文本文件，用于告诉 Git 哪些文件或目录应该被忽 - 掘金](https://juejin.cn/post/7449325810689458188)

## 2. 安装三方库

### 2.1 spdlog 日志库

[spdlog库笔记 ：编译、安装_spdlog 编译-CSDN博客](https://blog.csdn.net/qq_28368377/article/details/130872394)

### 2.2 

## 3. 编写系统上下文模块

### 3.1 日志管理类（LoggerManager）

#### LoggerManager.h

```cpp
#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <memory>
#include <filesystem>
#include <mutex>
#include <string>

// 类名获取宏
#ifdef _MSC_VER
    #define __CLASS_NAME__ (typeid(*this).name() + 6)  // MSVC 去掉 "class " 前缀
#else
    #define __CLASS_NAME__ typeid(*this).name()
#endif

// 提取纯函数名的辅助宏（去掉类名部分）
#ifdef __GNUC__
    #define __FUNCTION_SIMPLE__ \
        ([]() -> const char* { \
            static const char* func = __PRETTY_FUNCTION__; \
            const char* colon = strrchr(func, ':'); \
            return colon ? colon + 1 : func; \
        }())
#else
    #define __FUNCTION_SIMPLE__ __FUNCTION__
#endif

// 基础日志宏
#define LOG_TRACE(...)    LoggerManager::getInstance().trace(__VA_ARGS__)
#define LOG_DEBUG(...)    LoggerManager::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...)     LoggerManager::getInstance().info(__VA_ARGS__)
#define LOG_WARN(...)     LoggerManager::getInstance().warn(__VA_ARGS__)
#define LOG_ERROR(...)    LoggerManager::getInstance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) LoggerManager::getInstance().critical(__VA_ARGS__)

// 带类名和函数名的日志宏（优化格式，避免重复）
#define LOG_CLASS_TRACE(...)    \
    LoggerManager::getInstance().log(spdlog::level::trace, __FUNCTION_SIMPLE__, __VA_ARGS__)
#define LOG_CLASS_DEBUG(...)    \
    LoggerManager::getInstance().log(spdlog::level::debug, __FUNCTION_SIMPLE__, __VA_ARGS__)
#define LOG_CLASS_INFO(...)     \
    LoggerManager::getInstance().log(spdlog::level::info,  __FUNCTION_SIMPLE__, __VA_ARGS__)
#define LOG_CLASS_WARN(...)     \
    LoggerManager::getInstance().log(spdlog::level::warn, __FUNCTION_SIMPLE__, __VA_ARGS__)
#define LOG_CLASS_ERROR(...)    \
    LoggerManager::getInstance().log(spdlog::level::err, __FUNCTION_SIMPLE__, __VA_ARGS__)
#define LOG_CLASS_CRITICAL(...) \
    LoggerManager::getInstance().log(spdlog::level::critical, __FUNCTION_SIMPLE__, __VA_ARGS__)

// 异常日志宏
#define LOG_EXCEPTION(e, ...) \
    LoggerManager::getInstance().logException(e, __CLASS_NAME__, __FUNCTION_SIMPLE__, __VA_ARGS__)

class LoggerManager {
public:
    // 删除拷贝构造函数和赋值运算符
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;
    
    // 获取单例实例
    static LoggerManager& getInstance();
    
    // 初始化日志系统
    bool initialize(const std::string& logDir = "logs", 
                   spdlog::level::level_enum level = spdlog::level::info,
                   bool enableConsole = true);
    
    // 关闭日志系统
    void shutdown();
    
    // 基础日志方法
    template<typename... Args>
    void trace(const char* fmt, Args&&... args) {
        if (logger_) logger_->trace(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void debug(const char* fmt, Args&&... args) {
        if (logger_) logger_->debug(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void info(const char* fmt, Args&&... args) {
        if (logger_) logger_->info(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warn(const char* fmt, Args&&... args) {
        if (logger_) logger_->warn(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void error(const char* fmt, Args&&... args) {
        if (logger_) logger_->error(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void critical(const char* fmt, Args&&... args) {
        if (logger_) logger_->critical(fmt, std::forward<Args>(args)...);
    }
    
    // 带类名和函数名的日志方法（优化格式）
    template<typename... Args>
    void log(spdlog::level::level_enum level, 
             const std::string& funcName, const char* fmt, Args&&... args) {
        if (logger_ && logger_->should_log(level)) {
            // 优化格式：使用简洁的 [ClassName::funcName] 格式
            std::string formatted = fmt::format("[{}] {}",  funcName, fmt);
            logger_->log(level, formatted, std::forward<Args>(args)...);
        }
    }
    
    // 异常日志方法
    template<typename... Args>
    void logException(const std::exception& e, const std::string& className,
                     const std::string& funcName, const char* fmt, Args&&... args) {
        if (logger_) {
            std::string message = fmt::format("[{}::{}] {} - Exception: {}", 
                                             className, funcName, fmt, e.what());
            logger_->error(message, std::forward<Args>(args)...);
        }
    }
    
    // 获取底层logger
    std::shared_ptr<spdlog::logger> getLogger() const { return logger_; }
    
    // 设置日志级别
    void setLevel(spdlog::level::level_enum level);
    
    // 立即刷新日志
    void flush();
    
    // 检查是否已初始化
    bool isInitialized() const { return initialized_; }
    
private:
    LoggerManager() = default;
    ~LoggerManager();
    
    std::shared_ptr<spdlog::logger> logger_;
    std::mutex init_mutex_;
    bool initialized_ = false;
};
```

#### LoggerManager.cpp

```cpp
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
```

#### use example

```cpp
// 初始化日志系统
  if (!LoggerManager::getInstance().initialize("logs", spdlog::level::info,
                                               true)) {
    // 如果日志系统初始化失败，使用默认输出
    Log::message("警告：日志系统初始化失败，使用默认日志输出\n");
  }
```

#### 解析





## 错误/难点 整理

### 1. 








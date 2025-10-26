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
#pragma once


#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
// https://github.com/gabime/spdlog/issues/282  Why not use stream syntax
// TODO : 需要异步 ？  
// todo : 比如client不需要在屏幕里显示信息，怎么去除sink?
// todo 系统需要为不同任务设置log?
// TODO : 设计下如何使用
// todo: pattern + coroutine 
// https://spdlog.docsforge.com/v1.x/3.custom-formatting/#extending-spdlog-with-your-own-flags
#include <spdlog/spdlog.h>
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h" //support for async logging.
// #include "base/noncopyable.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <sstream>
#include <atomic> 
#include <stdlib.h>




class Logger {
public:
    Logger( const Logger& ) = delete;
    Logger& operator= ( const Logger& ) = delete;
    static Logger& Instance();
    bool init(const std::string& filePath = "./logs/log", 
            const std::string& loggerName = "Logger",
            spdlog::level::level_enum level = spdlog::level::info);

    void setLevel(spdlog::level::level_enum level = spdlog::level::info);
    static void onlyToConsole() { writeToFile_ = false; }
    static void onlyToFile() { writeToConsole_ = false; }
    static void shutdown() { spdlog::shutdown(); };

protected:
    Logger() = default;
    ~Logger() = default;

private:
    std::atomic<bool> isInited_{false};
    static std::atomic<bool> writeToConsole_;
    static std::atomic<bool> writeToFile_;
};



// spd 带行号的打印，同时输出console和文件
#define TRACE(...) SPDLOG_LOGGER_TRACE(spdlog::default_logger_raw(), __VA_ARGS__);
#define DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__);
#define INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__);
#define WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__);
#define ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__);
#define CRITICAL(...) SPDLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__);abort();

#define LOG_LEVEL_INFO spdlog::set_level(spdlog::level::info);
#define LOG_LEVEL_DEBUG spdlog::set_level(spdlog::level::debug);
#define LOG_LEVEL_TRACE spdlog::set_level(spdlog::level::trace);
#define LOG_LEVEL_WARN spdlog::set_level(spdlog::level::warn);
#define LOG_LEVEL_ERROR spdlog::set_level(spdlog::level::err);
#define LOG_LEVEL_CRITICAL spdlog::set_level(spdlog::level::critical);

// todo need to improve
// #define LOGGER Logger::Instance().init("Logger", "logs/log.txt", spdlog::level::trace);

#define LOGINIT(...) Logger::Instance().init(__VA_ARGS__);
#define LOGINIT_WITH_NAME(name) Logger::Instance().init("../logs/log", name);
#define ONLY_TO_CONSOLE Logger::Instance().onlyToConsole();
#define ONLY_TO_FILE Logger::Instance().onlyToFile();


std::atomic<bool> Logger::writeToConsole_{true};
std::atomic<bool> Logger::writeToFile_{true};



Logger& Logger::Instance() {
    static Logger log;
    return log;
}

bool Logger::init(const std::string& filePath, 
                const std::string& loggerName, 
                spdlog::level::level_enum level) {
    if(isInited_) return true;
    if(!writeToFile_ && !writeToConsole_) {
        std::cout << "Initialized AN EMPTY Logger!" << std::endl;
        return true;
    }
    try {
        spdlog::init_thread_pool(8192, 1); 
        // spdlog::flush_every(std::chrono::seconds(3));  // todo 这里能影响道logger吗
        
        std::vector<spdlog::sink_ptr> sinks;
        if(writeToConsole_) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks.push_back(std::move(console_sink));
        }

        if(writeToFile_) {
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt> (filePath, 1024*1024*5, 5, false);
            sinks.push_back(std::move(file_sink));
        }

        

        // std::vector<spdlog::sink_ptr> sinks{file_sink};   // 暂时先不要输出到显示屏
        auto logger = std::make_shared<spdlog::async_logger>(loggerName, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        logger->set_level(level);    // 需要单独设置logger的level      
        
        // 日志格式
        logger->set_pattern("%Y-%m-%d %H:%M:%S [%l] [tid : %t] [%s : %# <%!>] %v");
        

        // logger->flush_on(spdlog::level::warn);
        spdlog::set_default_logger(logger);
    } catch(const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
    isInited_ = true;
    return true;
}

void Logger::setLevel(spdlog::level::level_enum level) {
    spdlog::set_level(level);  // 后面setlevel就可以影响logger了
}

enum PutType {
    Console = 0,
    File,
    ConsoleFile
};

enum LogType {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

void set_log( PutType put_type, LogType log_type ) {

    switch ( put_type )
    {
    case Console:
        ONLY_TO_CONSOLE;
        break;
    case File:
        ONLY_TO_FILE;
    default:
        break;
    }

    LOGINIT(); 
    switch (log_type) {
        case LogType::Trace:
             LOG_LEVEL_TRACE;
             break;
        case LogType::Debug:
             LOG_LEVEL_DEBUG;
             break;
        case LogType::Info:
             LOG_LEVEL_INFO;
             break;
        case LogType::Warn:
             LOG_LEVEL_WARN;
             break;
        case LogType::Error:
             LOG_LEVEL_ERROR;
             break;
        case LogType::Critical:
             LOG_LEVEL_CRITICAL;
             break;
        default:
            exit( 1 );      
    }
}

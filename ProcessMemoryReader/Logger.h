#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <memory>

/**
 * @brief Comprehensive logging system for bot activities
 */
class Logger {
public:
    enum class LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };

private:
    std::string m_logDirectory;
    std::string m_currentLogFile;
    std::ofstream m_logStream;
    
    LogLevel m_minLogLevel = LogLevel::INFO;
    bool m_consoleOutput = true;
    bool m_fileOutput = true;
    
    // Async logging
    std::queue<std::string> m_logQueue;
    std::mutex m_queueMutex;
    std::thread m_logThread;
    std::atomic<bool> m_running{false};
    
    // Statistics
    uint64_t m_totalLogs = 0;
    std::unordered_map<LogLevel, uint64_t> m_logCounts;

public:
    Logger(const std::string& logDirectory = "logs");
    ~Logger();
    
    // Core logging methods
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    
    // Formatted logging
    template<typename... Args>
    void debug(const std::string& format, Args... args) {
        log(LogLevel::DEBUG, formatString(format, args...));
    }
    
    template<typename... Args>
    void info(const std::string& format, Args... args) {
        log(LogLevel::INFO, formatString(format, args...));
    }
    
    template<typename... Args>
    void warning(const std::string& format, Args... args) {
        log(LogLevel::WARNING, formatString(format, args...));
    }
    
    template<typename... Args>
    void error(const std::string& format, Args... args) {
        log(LogLevel::ERROR, formatString(format, args...));
    }
    
    template<typename... Args>
    void critical(const std::string& format, Args... args) {
        log(LogLevel::CRITICAL, formatString(format, args...));
    }
    
    // Specialized logging methods
    void logBotAction(const std::string& action, const std::string& details = "");
    void logCombat(const std::string& target, const std::string& result);
    void logLoot(const std::string& itemName, const std::string& rarity);
    void logNavigation(float x, float y, const std::string& action);
    void logError(const std::string& system, const std::string& error);
    void logPerformance(const std::string& metric, double value);
    
    // Configuration
    void setMinLogLevel(LogLevel level) { m_minLogLevel = level; }
    void setConsoleOutput(bool enabled) { m_consoleOutput = enabled; }
    void setFileOutput(bool enabled) { m_fileOutput = enabled; }
    
    // File management
    void rotateLogs();
    void clearOldLogs(int daysToKeep = 7);
    std::string getCurrentLogFile() const { return m_currentLogFile; }
    
    // Statistics
    uint64_t getTotalLogs() const { return m_totalLogs; }
    std::unordered_map<LogLevel, uint64_t> getLogCounts() const { return m_logCounts; }

private:
    void log(LogLevel level, const std::string& message);
    void processLogQueue();
    void writeToFile(const std::string& formattedMessage);
    void writeToConsole(const std::string& formattedMessage, LogLevel level);
    
    std::string formatMessage(LogLevel level, const std::string& message);
    std::string getCurrentTimestamp();
    std::string logLevelToString(LogLevel level);
    std::string generateLogFileName();
    
    template<typename... Args>
    std::string formatString(const std::string& format, Args... args) {
        size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1);
    }
};

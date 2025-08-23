#include "Logger.h"
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>

Logger::Logger(const std::string& logDirectory) : m_logDirectory(logDirectory) {
    // Create logs directory if it doesn't exist
    std::filesystem::create_directories(m_logDirectory);
    
    // Generate log file name
    m_currentLogFile = generateLogFileName();
    
    // Open log file
    m_logStream.open(m_currentLogFile, std::ios::app);
    
    // Start async logging thread
    m_running = true;
    m_logThread = std::thread(&Logger::processLogQueue, this);
    
    info("Logger initialized");
}

Logger::~Logger() {
    if (m_running) {
        m_running = false;
        if (m_logThread.joinable()) {
            m_logThread.join();
        }
    }
    
    if (m_logStream.is_open()) {
        m_logStream.close();
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::logBotAction(const std::string& action, const std::string& details) {
    std::string message = "BOT_ACTION: " + action;
    if (!details.empty()) {
        message += " - " + details;
    }
    info(message);
}

void Logger::logCombat(const std::string& target, const std::string& result) {
    info("COMBAT: Target=" + target + ", Result=" + result);
}

void Logger::logLoot(const std::string& itemName, const std::string& rarity) {
    info("LOOT: Item=" + itemName + ", Rarity=" + rarity);
}

void Logger::logNavigation(float x, float y, const std::string& action) {
    info("NAVIGATION: Action=" + action + ", Position=(" + 
         std::to_string(x) + "," + std::to_string(y) + ")");
}

void Logger::logError(const std::string& system, const std::string& error) {
    this->error("SYSTEM_ERROR: " + system + " - " + error);
}

void Logger::logPerformance(const std::string& metric, double value) {
    info("PERFORMANCE: " + metric + "=" + std::to_string(value));
}

void Logger::rotateLogs() {
    if (m_logStream.is_open()) {
        m_logStream.close();
    }
    
    m_currentLogFile = generateLogFileName();
    m_logStream.open(m_currentLogFile, std::ios::app);
}

void Logger::clearOldLogs(int daysToKeep) {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24 * daysToKeep);
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_logDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".log") {
                auto fileTime = std::filesystem::last_write_time(entry);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
                
                if (sctp < cutoff) {
                    std::filesystem::remove(entry);
                }
            }
        }
    }
    catch (const std::exception& e) {
        error("Failed to clear old logs: " + std::string(e.what()));
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < m_minLogLevel) {
        return;
    }
    
    std::string formattedMessage = formatMessage(level, message);
    
    // Add to queue for async processing
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_logQueue.push(formattedMessage);
    }
    
    // Update statistics
    m_totalLogs++;
    m_logCounts[level]++;
    
    // Also output to console if enabled
    if (m_consoleOutput) {
        writeToConsole(formattedMessage, level);
    }
}

void Logger::processLogQueue() {
    while (m_running) {
        std::string message;
        
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            if (!m_logQueue.empty()) {
                message = m_logQueue.front();
                m_logQueue.pop();
            }
        }
        
        if (!message.empty() && m_fileOutput) {
            writeToFile(message);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Logger::writeToFile(const std::string& formattedMessage) {
    if (m_logStream.is_open()) {
        m_logStream << formattedMessage << std::endl;
        m_logStream.flush();
    }
}

void Logger::writeToConsole(const std::string& formattedMessage, LogLevel level) {
    // Use different colors for different log levels
    switch (level) {
        case LogLevel::DEBUG:
            std::cout << "\033[37m" << formattedMessage << "\033[0m" << std::endl; // White
            break;
        case LogLevel::INFO:
            std::cout << "\033[32m" << formattedMessage << "\033[0m" << std::endl; // Green
            break;
        case LogLevel::WARNING:
            std::cout << "\033[33m" << formattedMessage << "\033[0m" << std::endl; // Yellow
            break;
        case LogLevel::ERROR:
        case LogLevel::CRITICAL:
            std::cout << "\033[31m" << formattedMessage << "\033[0m" << std::endl; // Red
            break;
    }
}

std::string Logger::formatMessage(LogLevel level, const std::string& message) {
    std::ostringstream oss;
    oss << "[" << getCurrentTimestamp() << "] "
        << "[" << logLevelToString(level) << "] "
        << message;
    return oss.str();
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string Logger::generateLogFileName() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << m_logDirectory << "/bot_" 
        << std::put_time(std::localtime(&time_t), "%Y-%m-%d") 
        << ".log";
    return oss.str();
}

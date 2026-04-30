#pragma once

/**
 * Simple debug logger class and macros for simple usage
 *
 * This is a simple macro which implements a debug logger with or without time
 * stamp.
 * The logging can be used like with standard C++ streams, using the normal
 * '<<' streamoperator
 *
 * Usage with the time stamp:
 *      LOG_INFO() << "You failed " << 20 << " Times!"
 *      LOG_WARN() << "You failed " << 20 << " Times!"
 *      LOG_ERROR() << "You failed " << 20 << " Times!"
 *
 * Usage without the time stamp:
 *      LOG_INFO_NT() << "You failed " << 20 << " Times!"
 *      LOG_WARN_NT() << "You failed " << 20 << " Times!"
 *      LOG_ERROR_NT() << "You failed " << 20 << " Times!"
 */

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace thengine {

/**
 * This mutex is to make logging thread safe.
 * Mutex is locked in DebugLogger constructor and released in
 * the destructor. 
 */
inline std::mutex g_debuglogger_mutex;

/**
 * Defines the supported warning levels for the macro.
 * The level defines a warning level text displayed by the macro
 */
enum class DebugLogLevel : int { EINFO = 0, EWARN = 1, EERROR = 2 };

/**
 * DebugLogger class definition
 * Class has stream output operation with operator <<
 * Constructor creates the initial output with (or without) a time stamp
 * and the desired warning level (severity).
 *
 * Default warning level is ERROR
 */
class DebugLogger {
private: // constants
    static constexpr const char* KTextLevelInfo = "INFO";
    static constexpr const char* KTextLevelWarn = "WARN";
    static constexpr const char* KTextLevelError = "ERROR";

public:
    DebugLogger(DebugLogLevel severity = DebugLogLevel::EERROR, bool showtime = true)
        : m_lock(g_debuglogger_mutex), m_buffer(), m_stm{} {

        if (showtime) {
            make_time();
            m_buffer << "[" << std::put_time(&m_stm, "%F %T") << "] ";
        }

        switch (severity) {
        case DebugLogLevel::EWARN:
            m_buffer << KTextLevelWarn;
            break;
        case DebugLogLevel::EERROR:
            m_buffer << KTextLevelError;
            break;
        default:
            m_buffer << KTextLevelInfo;
            break;
        }
        m_buffer << ": ";
    }

    // Destructor, causes the debug info to be outputted with new line
    virtual ~DebugLogger() {
        std::cerr << m_buffer.str() << std::endl;
        // lock is released automatically
    }

    /**
     * Stream output operator, appends a value to the debug output stream
     */
    template <typename T> 
    DebugLogger& operator<<(const T& value) {
        m_buffer << value;
        return *this;
    }

private:
    void make_time() {
        std::time_t t_now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#if defined(_WIN32)
        localtime_s(&m_stm, &t_now);
#else
        localtime_r(&t_now, &m_stm);
#endif
    }

private:
    std::unique_lock<std::mutex> m_lock;
    std::ostringstream m_buffer;
    struct std::tm m_stm;
};

} // namespace thengine

/**
 * Helper macros/defines for using the DebugLogger
 */

// Default logger, severity level is ERROR, with a time stamp
#define LOG() thengine::DebugLogger()

// Default logger, severity level is ERROR, without a time stamp
#define LOG_NT() thengine::DebugLogger(thengine::DebugLogLevel::EERROR, false)

// These variants show system time
#define LOG_INFO() thengine::DebugLogger(thengine::DebugLogLevel::EINFO)
#define LOG_WARN() thengine::DebugLogger(thengine::DebugLogLevel::EWARN)
#define LOG_ERROR() thengine::DebugLogger(thengine::DebugLogLevel::EERROR)

// These variants omit the system time and only show the warning level
#define LOG_INFO_NT() thengine::DebugLogger(thengine::DebugLogLevel::EINFO, false)
#define LOG_WARN_NT() thengine::DebugLogger(thengine::DebugLogLevel::EWARN, false)
#define LOG_ERROR_NT() thengine::DebugLogger(thengine::DebugLogLevel::EERROR, false)

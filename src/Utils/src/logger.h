#ifndef LOGGER_H
#define LOGGER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <concepts>
#include <cstddef>

// Log levels
#define LOG_LEVEL_ERROR   0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_INFO    2
#define LOG_LEVEL_DEBUG   3

#ifndef GLOBAL_LOG_LEVEL
    #define GLOBAL_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

namespace Utils
{
    inline const char* getTimestamp() 
    {
        static char buffer[20];
        std::time_t now = std::time(nullptr);
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return buffer;
    }

    inline void logMessage(const char* logLevel, const std::string& message) 
    {
        // std::cout << "[" << getTimestamp() << "] ";
        std::cout << "[" << logLevel << "] " << message << std::endl;
    }

    inline void logMessage(const char* logLevel, const std::string& message) 
    {
        // std::cout << "[" << getTimestamp() << "] ";
        std::cout << "[" << logLevel << "] " << message << std::endl;
    }
}

#define LOG_AT_LEVEL(level, msg) \
    { \
        std::ostringstream oss; \
        oss << msg; \
        Utils::logMessage(level, oss.str()); \
    }

#if GLOBAL_LOG_LEVEL >= LOG_LEVEL_ERROR
    #define LOG_ERROR(msg) \
        LOG_AT_LEVEL("ERROR", msg)
#else
    #define LOG_ERROR(msg)
#endif

#if GLOBAL_LOG_LEVEL >= LOG_LEVEL_WARNING
    #define LOG_WARNING(msg) \
        LOG_AT_LEVEL("WARNING", msg)
#else
    #define LOG_WARNING(msg)
#endif

#if GLOBAL_LOG_LEVEL >= LOG_LEVEL_INFO
    #define LOG_INFO(msg) \
        LOG_AT_LEVEL("INFO", msg)
#else
    #define LOG_INFO(msg)
#endif

#if GLOBAL_LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define LOG_DEBUG(msg) \
        LOG_AT_LEVEL("DEBUG", msg)
#else
    #define LOG_DEBUG(msg)
#endif

template <typename T>
concept Vector = requires(T obj, std::size_t i) {
    { obj.length() } -> std::convertible_to<std::size_t>;
    { obj[i] };
};

template <typename T>
concept Matrix = requires(T obj, std::size_t i) {
    { obj.length() } -> std::convertible_to<std::size_t>;
    { obj[i] };
    { obj[0].length() } -> std::convertible_to<std::size_t>;
};

template <Vector T>
inline std::string printVector(const T& vector)
{
    std::ostringstream oss;
    oss << "( ";
    for ( int i = 0; i < vector.length(); ++i )
        oss << vector[i] << " ";
    oss << ")";
    return oss.str();
}

template <Matrix T>
inline std::string printMatrix(const T& matrix)
{
    std::ostringstream oss;
    for ( int i = 0; i < matrix.length(); ++i )
    {
        oss << "\n[ ";
        for ( int j = 0; j < matrix[0].length(); ++j )
            oss << matrix[i][j] << ' ';
        oss << "]";
    }

    return oss.str();
}

#endif
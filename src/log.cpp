#include "../include/log.h"

#include <cstdarg>
#include <iostream>

namespace logger {

void Info(const char *fmt, ...) {
    std::cout << "[INFO] ";
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    std::cout << std::endl;
}

void Warn(const char *fmt, ...) {
    std::cout << "[WARN] ";
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    std::cout << std::endl;
}

void Error(const char *fmt, ...) {
    std::cerr << "[ERROR] ";
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    std::cerr << std::endl;
}
}  // namespace logger

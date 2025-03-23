#pragma once

namespace logger {
enum Level { INFO, WARN, ERROR };

void Info(const char *fmt, ...);
void Warn(const char *fmt, ...);
void Error(const char *fmt, ...);
}  // namespace logger

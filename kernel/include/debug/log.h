#ifndef _LOG_H_
#define _LOG_H_

#include "config.h"
#include "printf.h"
#include "console.h"

#define LEVEL_TRACE 0
#define LEVEL_DEBUG 1
#define LEVEL_INFO  2
#define LEVEL_WARN  3
#define LEVEL_ERROR 4

#if defined(CONFIG_LOGGING_LEVEL_TRACE)
	#define LEVEL LEVEL_TRACE
#elif defined(CONFIG_LOGGING_LEVEL_DEBUG)
	#define LEVEL LEVEL_DEBUG
#elif defined(CONFIG_LOGGING_LEVEL_INFO)
	#define LEVEL LEVEL_INFO
#elif defined(CONFIG_LOGGING_LEVEL_WARN)
	#define LEVEL LEVEL_WARN
#elif defined(CONFIG_LOGGING_LEVEL_ERROR)
	#define LEVEL LEVEL_ERROR
#endif

#define LOG(level_str, color, fmt, ...)                                \
	do {                                                           \
		console_set_background((color));                       \
		console_set_foreground(CONSOLE_COLOR_BLACK);           \
		printf("[ " #level_str " ]");                          \
		console_set_foreground((color));                       \
		console_set_background(CONSOLE_COLOR_BLACK);           \
		printf(" %s:%s:%d: " fmt "\n", __FILE__, __FUNCTION__, \
		       __LINE__, ##__VA_ARGS__);                       \
		console_set_background(CONSOLE_COLOR_BLACK);           \
		console_set_foreground(CONSOLE_COLOR_WHITE);           \
	} while (0)

#define USER_LOG_OK(fmt, ...)                                       \
	do {                                                        \
		console_set_background(CONSOLE_COLOR_GREEN_BRIGHT); \
		console_set_foreground(CONSOLE_COLOR_BLACK);        \
		printf("[ OK ]");                                   \
		console_set_foreground(CONSOLE_COLOR_GREEN_BRIGHT); \
		console_set_background(CONSOLE_COLOR_BLACK);        \
		printf(" " fmt "\n", ##__VA_ARGS__);                \
		console_set_background(CONSOLE_COLOR_BLACK);        \
		console_set_foreground(CONSOLE_COLOR_WHITE);        \
	} while (0)

#define USER_LOG_WARN(fmt, ...)                                      \
	do {                                                         \
		console_set_background(CONSOLE_COLOR_YELLOW_BRIGHT); \
		console_set_foreground(CONSOLE_COLOR_BLACK);         \
		printf("[ WARN ]");                                  \
		console_set_foreground(CONSOLE_COLOR_YELLOW_BRIGHT); \
		console_set_background(CONSOLE_COLOR_BLACK);         \
		printf(" " fmt "\n", ##__VA_ARGS__);                 \
		console_set_background(CONSOLE_COLOR_BLACK);         \
		console_set_foreground(CONSOLE_COLOR_WHITE);         \
	} while (0)

#if LEVEL > LEVEL_TRACE
	#define TRACE(fmt, ...)
#else
	#define TRACE(fmt, ...) \
		LOG(TRACE, CONSOLE_COLOR_WHITE, fmt, ##__VA_ARGS__)
#endif

#if LEVEL > LEVEL_DEBUG
	#define DEBUG(fmt, ...)
#else
	#define DEBUG(fmt, ...) \
		LOG(DEBUG, CONSOLE_COLOR_WHITE_BRIGHT, fmt, ##__VA_ARGS__)
#endif

#if LEVEL > LEVEL_INFO
	#define INFO(fmt, ...) USER_LOG_OK(fmt, ##__VA_ARGS__)
#else
	#define INFO(fmt, ...) \
		LOG(INFO, CONSOLE_COLOR_GREEN_BRIGHT, fmt, ##__VA_ARGS__)
#endif

#if LEVEL > LEVEL_WARN
	#define WARN(fmt, ...) USER_LOG_WARN(fmt, ##__VA_ARGS__)
#else
	#define WARN(fmt, ...) \
		LOG(WARN, CONSOLE_COLOR_YELLOW_BRIGHT, fmt, ##__VA_ARGS__)
#endif

/* we always print error and fatal messages */
#define ERROR(fmt, ...) LOG(ERROR, CONSOLE_COLOR_RED_BRIGHT, fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) \
	LOG(FATAL, CONSOLE_COLOR_MAGENTA_BRIGHT, fmt, ##__VA_ARGS__)

#endif // _LOG_H_

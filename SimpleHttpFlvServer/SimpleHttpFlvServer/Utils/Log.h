#pragma once
#include <stdio.h>

#ifdef ENABLE_MINILOGGER
#include "minilogger/log.h"
#else
#define minilog_warn(...)
#define minilog_error(...)
#define minilog_info(...)
#define minilog_debug(...)
#define __minilog_log_init(...)
#endif

#define LOG_LEVEL_ENV "SIMPLE_HLS_SERVER_APP_LOG_LEVEL"

extern int enable_minilog;
extern int log_level;

extern void setLogLevel(int level);
extern int getLogLevel();

#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARN 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

#ifndef LOG_TAG
#define LOG_TAG "cascade_app"
#endif // LOG_TAG

//#define ENABLE_BUFFER_TIME_DEBUG

#define LOG_INFO(format, ...)                                                  \
  do {                                                                         \
    if (getLogLevel() < LOG_LEVEL_INFO)                                \
      break;                                                                   \
    if (enable_minilog)                                                        \
      minilog_info("[%s][%s]:" format, LOG_TAG, __FUNCTION__, ##__VA_ARGS__);  \
    else                                                                       \
      fprintf(stderr, "[%s][%s]:" format, LOG_TAG, __FUNCTION__,               \
              ##__VA_ARGS__);                                                  \
  } while (0)

#define LOG_WARN(format, ...)                                                  \
  do {                                                                         \
    if (getLogLevel() < LOG_LEVEL_WARN)                                \
      break;                                                                   \
    if (enable_minilog)                                                        \
      minilog_warn("[%s][%s]:" format, LOG_TAG, __FUNCTION__, ##__VA_ARGS__);  \
    else                                                                       \
      fprintf(stderr, "[%s][%s]:" format, LOG_TAG, __FUNCTION__,               \
              ##__VA_ARGS__);                                                  \
  } while (0)

#define LOG_ERROR(format, ...)                                                 \
  do {                                                                         \
    if (getLogLevel() < LOG_LEVEL_ERROR)                               \
      break;                                                                   \
    if (enable_minilog)                                                        \
      minilog_error("[%s][%s]:" format, LOG_TAG, __FUNCTION__, ##__VA_ARGS__); \
    else                                                                       \
      fprintf(stderr, "[%s][%s]:" format, LOG_TAG, __FUNCTION__,               \
              ##__VA_ARGS__);                                                  \
  } while (0)

#define LOG_DEBUG(format, ...)                                                 \
  do {                                                                         \
    if (getLogLevel() < LOG_LEVEL_DEBUG)                               \
      break;                                                                   \
    if (enable_minilog)                                                        \
      minilog_debug("[%s][%s]:" format, LOG_TAG, __FUNCTION__, ##__VA_ARGS__); \
    else                                                                       \
      fprintf(stderr, "[%s][%s]:" format, LOG_TAG, __FUNCTION__,               \
              ##__VA_ARGS__);                                                  \
  } while (0)

#define LOG_D(format, ...) LOG_DEBUG(format, ##__VA_ARGS__)
#define LOG_I(format, ...) LOG_INFO(format, ##__VA_ARGS__)
#define LOG_W(format, ...) LOG_WARN(format, ##__VA_ARGS__)
#define LOG_E(format, ...) LOG_ERROR(format, ##__VA_ARGS__)

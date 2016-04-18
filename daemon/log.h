#ifndef _TE_DAEMON_LOG_H
#define _TE_DAEMON_LOG_H
#include <android/log.h>

#define LOG_TAG "cmccd"

#define LOGD(...) {                                                     \
    if (ADB_LOG)                                                        \
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__);   \
}

#define LOGI(...) {                                                     \
    if (ADB_LOG)                                                        \
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);    \
}

#define LOGW(...) {                                                     \
    if (ADB_LOG)                                                        \
        __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__);    \
}

#define LOGE(...) {                                                     \
    if (ADB_LOG)                                                        \
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__);   \
}

#define LOGF(...) {                                                     \
    if (ADB_LOG)                                                        \
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)    \
}

#endif

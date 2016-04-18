#ifndef _TE_LOG_H
#define _TE_LOG_H

#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_TAG
#define LOG_TAG "CMCC_Transparent_Encryption"
#else
#undef  LOG_TAG
#define LOG_TAG "CMCC_Transparent_Encryption"
#endif

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

#ifdef __cplusplus
}
#endif
#endif

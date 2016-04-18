#ifndef _TE_USER_H
#define _TE_USER_H

#include <private/android_filesystem_config.h>
#include <sys/system_properties.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

uid_t getpuid(void);
uid_t getpaid(void);
uid_t getpdid(void);
char* getpname(void);

#ifdef __cplusplus
}
#endif
#endif

#ifndef _TE_CONSTANTS_H
#define _TE_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024
#else
#undef  PATH_MAX
#define PATH_MAX 1024
#endif

#ifndef PACKAGE_NAME_MAX
#define PACKAGE_NAME_MAX 256
#else
#undef  PACKAGE_NAME_MAX
#define PACKAGE_NAME_MAX 256
#endif

#define THREADS_COUNT       4
#define THREADS_STACK_SIZE  0xA000

#ifdef __cplusplus
}
#endif
#endif

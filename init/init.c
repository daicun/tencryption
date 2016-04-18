// #include <stdio.h>
// #include <dlfcn.h>
// #include <stdlib.h>
//
// #include "log.h"
// #include "util.h"
// #include "linker.h"
// #include "constants.h"
//
// int linker_running_state = 0;
//
// struct linker {
//     char *name;		        /* the name */
//     void (**syscall)(void); /* the underlying syscall */
// };
//
// static struct linker linker_list[] = {
//     { "dup",        (void (**)(void)) &linker_dup,      },
//     { "stat",       (void (**)(void)) &linker_stat,     },
//     { "lstat",      (void (**)(void)) &linker_lstat,    },
//     { "fstat",      (void (**)(void)) &linker_fstat,    },
//     { "open",       (void (**)(void)) &linker_open,     },
//     { "read",       (void (**)(void)) &linker_read,     },
//     { "write",      (void (**)(void)) &linker_write,    },
//     { "close",      (void (**)(void)) &linker_close,    },
//     { "lseek",      (void (**)(void)) &linker_lseek,    },
//     { "lseek64",    (void (**)(void)) &linker_lseek64,  },
//     { "pread",      (void (**)(void)) &linker_pread,    },
//     { "pwrite",     (void (**)(void)) &linker_pwrite,   },
//     { "mmap",       (void (**)(void)) &linker_mmap,     },
//     { "fread",      (void (**)(void)) &linker_fread,    },
//     { NULL,         NULL,                               }
// };
//
// #define LIBC_PATH "libc.so"
// static void linker_init(struct linker *ln)
// {
//     void *handler = NULL;
//
//     if (*ln->syscall != NULL)
//     {
//         LOGD("%s\t the %s linker already exsts", __FUNCTION__, ln->name);
//         return;
//     }
//
//     handler = dlopen(LIBC_PATH, RTLD_NOW | RTLD_GLOBAL);
//     if (handler != NULL)
//     {
//         *ln->syscall = dlsym(handler, ln->name);
//         if (*ln->syscall == NULL)
//         {
//             LOGE("%s\t error messages: %s", __FUNCTION__, dlerror());
//         }
//         dlclose(handler);
//     }
// }
//
// static void linker_list_init(void)
// {
//     static int state = 0;
//
//     if (state == 0)
//     {
//         state = 1;
//         for (int i = 0; linker_list[i].name != NULL; ++i)
//         {
//             linker_init(&linker_list[i]);
//         }
//     }
// }
//
// static void linker_list_load(void)
// {
//     if (linker_running_state == 0)
//     {
//         linker_list_init();
//         file_info_list_init();
//         linker_running_state = 1;
//     }
// }
//
// static void init(void) __attribute__ ((constructor));
// void init(void)
// {
//     linker_list_load();
// }
//
// void linker_reload(void)
// {
//     linker_list_load();
// }


#include <stdlib.h>

#include <log.h>

static void init_lib(void) __attribute__ ((constructor));
static void init_lib(void)
{

}

static void fini_lib(void) __attribute__ ((destructor));
static void fini_lib(void)
{

}

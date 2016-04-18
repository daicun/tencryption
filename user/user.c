#include "user.h"
// get current user id
// uid_t userid = getuid() / AID_USER;
// get current app id
// uid_t appid  = getuid() % AID_USER;
#ifndef PACKAGE_NAME_MAX
#define PACKAGE_NAME_MAX 256
#else
#undef  PACKAGE_NAME_MAX
#define PACKAGE_NAME_MAX 256
#endif

uid_t getpuid(void)
{
    return getuid() / AID_USER;
}

uid_t getpaid(void)
{
    return getuid() % AID_USER;
}

uid_t getpdid(void)
{
    const char *domain = "persist.sys.current_user_id";

    char value[PROP_VALUE_MAX] = { 0 };

    memset(value, 0, PROP_VALUE_MAX);
    if (__system_property_get(domain, value) == 0)
        return 0;

    return atoi(value);
}

char* getpname(void)
{
    char path[PATH_MAX] = { 0 };

    // get cmdline path
    sprintf(path, "/proc/%d/cmdline", getpid());

    int fd = open(path, O_RDONLY);
    if (fd > 0)
    {
        char *pname = (char *) calloc(1, PACKAGE_NAME_MAX);
        if (pname != NULL)
        {
            if (read(fd, pname, PACKAGE_NAME_MAX) > 0)
            {
                close(fd);
                return pname;
            }
        }
        close(fd);
    }

    return NULL;
}

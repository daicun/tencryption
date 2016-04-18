#include <ftw.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>

#include <cutils/sockets.h>

#include <private/android_filesystem_config.h>

#include "log.h"

#define CMCCD_NAME      "cmcc:daemon"
#define SECURITY_PATH	"/data/enterprise"

static int restorecon(const char *file)
{
	chmod(file, 0775);
	chown(file, AID_CONTAINER, AID_CONTAINER);
	return 0;
}

static int nftw_restorecon(const char *file, const struct stat *sb, int flags, struct FTW *s)
{
    restorecon(file);
    return 0;
}

static int restorecon_recursive(const char *dir)
{
    int fd_limit = 20;
    int flags = FTW_DEPTH | FTW_MOUNT | FTW_PHYS;
    return nftw(dir, nftw_restorecon, fd_limit, flags);
}

static void restorecon_handler(int signum)
{
    LOGD("%s\t signal number: %d", __FUNCTION__, signum);
    switch (signum) {
        case SIGALRM:
            restorecon_recursive(SECURITY_PATH);
        break;
    }
}

/*
static void set_timer(int seconds, int microsecond)
{
    struct itimerval timer;

    signal(SIGALRM, restorecon_handler);

    timer.it_interval.tv_sec    = seconds;
    timer.it_interval.tv_usec   = microsecond;
    timer.it_value.tv_sec       = seconds;
    timer.it_value.tv_usec      = microsecond;
    setitimer(ITIMER_REAL, &timer, NULL);
}
*/

static char *convert_file_descriptor_to_file_path(int pid, int fd)
{
    char path[PATH_MAX] = { 0 };

    sprintf(path, "/proc/%d/fd/%d", pid, fd);

    // readlink(path, pathname, n);

    return NULL;
}

enum REQUEST_COMMAND {
    CONVERT_FILE_DESCRIPTOR_TO_FILE_PATH = 0x81,
};

/*
enum RESPONSE_COMMAND {
    GET_FILE_PATH = 0x01,
};
*/

struct command_package {
    char command;
    char package[256];
};

#define command_package_size (sizeof(struct command_package))

static void handle_request(int fd)
{
    int count;

    char buffer[command_package_size];

    count = read(fd, buffer, command_package_size);
    if (count > 0) {
        struct command_package *ptr;

        const char * delim = ",";

        ptr = (struct command_package *) buffer;
        switch(ptr->command) {
            case CONVERT_FILE_DESCRIPTOR_TO_FILE_PATH:
            {
                unsigned int s_fd, s_pid;

                if (ptr->package[0] == '\0') {
                    LOGE("not data");
                    break;
                }

                s_fd  = atoi(strtok(ptr->package, delim));
                s_pid = atoi(strtok(NULL, delim));
                convert_file_descriptor_to_file_path(s_pid, s_fd);
            }
            break;


            default:
                LOGE("bad command: %x", ptr->command);
            break;
        }
    }

    close(fd);
}

int main(int argc, char **argv)
{
    pid_t pid;

    if ((pid = fork()) == 0) {
        for (;;) {
            // LOGD("children");
            sleep(2);
            restorecon_recursive(SECURITY_PATH);
        }
    } else {
        for (;;) {
            // LOGD("parent");
            int s;

            s = socket_local_server(CMCCD_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);

            if (s < 0)
                continue;

            fcntl(s, F_SETFD, FD_CLOEXEC);

            for (;;) {
                struct sockaddr addr;

                socklen_t alen = sizeof(addr);

                LOGD("waiting for connection\n");
                int fd = accept(s, &addr, &alen);
                if (fd < 0) {
                    LOGE("accept failed: %s\n", strerror(errno));
                    continue;
                }
                fcntl(fd, F_SETFD, FD_CLOEXEC);

                handle_request(fd);
            }
            close(s);
        }
    }
    return 0;
}

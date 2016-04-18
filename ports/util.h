#ifndef _TE_UTIL_H
#define _TE_UTIL_H
#include <cutils/list.h>

#include "log.h"
#include "crypto.h"
#include "constants.h"

int update_linker_running_state(void);
int update_usb_connector_state(void);

char *get_process_name(void);
int get_process_id(void);
int get_process_uid(void);
int get_process_gid(void);
int get_process_user(void);
int get_process_domain(void);

int is_trust_process(void);

int get_domain_state(void);

struct file_info {
    struct listnode node;       // child nodes
    int file_descriptor;        // file descriptor
    char file_path[PATH_MAX];   // file path
    struct file_header header;  // file header
};

void file_info_list_init(void);
void file_info_list_deinit(void);
struct file_info *file_info_list_add(const int fd, const char *path);
struct file_info *file_info_list_query_by_fd(const int fd);
void file_info_list_remove_by_fd(const int fd);

#endif

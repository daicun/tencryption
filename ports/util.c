#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/system_properties.h>

#include <private/android_filesystem_config.h>

#include "util.h"
#include "linker.h"

extern int linker_running_state;

extern int linker_reload(void);

void trim(char *str)
{
    size_t j = 0;
        
    for (size_t i = 0; i < strlen(str); ++i)
    {
        if (isspace(str[i]) != 0)
		{
			continue;
		}
        
        str[j] = str[i];
        j++;
    }
    str[j] = '\0';
}

int update_linker_running_state(void)
{
    if (linker_running_state == 0)
        linker_reload();
    
    return linker_running_state;
}

#define USB_STATE_PATH "/sys/class/android_usb/android0/state"
int update_usb_connector_state(void)
{
	int result = 0;
		
    int fd = linker_open(USB_STATE_PATH, O_RDONLY);
    if(fd > 0)
    {
        char buffer[32] = {0};
        
        if (linker_read(fd, buffer, sizeof(buffer)) > 0)
        {
            if (memcmp(buffer, USB_STATE_CONNECTED,  strlen(buffer)) == 0)
            {
                result = 1;
            }
            else if (memcmp(buffer, USB_STATE_CONFIGURED, strlen(buffer)) == 0)
            {
                result = 1;
            }
            else
            {
                result = 0;
            }
        }
        linker_close(fd);
    }
    
    return result;
}

char *get_process_name(void)
{
    char path[PATH_MAX] = { 0 };       

    char *name = (char *) calloc(1, PROCESS_NAME_MAX);
    if (name == NULL)
    {
        return NULL;
    }
    else
    {
        sprintf(path, "/proc/%d/cmdline", getpid());

        int fd = linker_open(path, O_RDONLY);

        if (fd > 0)
        {
            if (linker_read(fd, name, PROCESS_NAME_MAX) > 0)
            {
                
            }
            linker_close(fd);
        }
    }
    
    return name;
}

int get_process_id(void)
{
    return getpid();
}

int get_process_uid(void)
{
    return (getuid() % AID_USER);
}

int get_process_gid(void)
{
    return (getgid() % AID_USER);
}

int get_process_user(void)
{
    return (getuid() / AID_USER);
}

#define PROP_CURRENT_DOMAIN "persist.sys.current_user_id"
int get_process_domain(void)
{
    char value[PROP_VALUE_MAX] = {0};

    if (__system_property_get(PROP_CURRENT_DOMAIN, value) == 0)
        return 0;

    return atoi(value);
}

int is_trust_process(void)
{
    int trust = 0;
    
    int domain = get_process_domain();
    if (domain == 0)
    {
        return trust;
    }

    int user = get_process_user();
    if (user == 0)
    {
        int uid = get_process_uid();
        if (uid == AID_MEDIA)
        {
            trust = 1;
        }
    }
    else
    {
        if (domain == user)
        {
            trust = 1;
        }
    }

    return trust;
}

#define SECURITY_DIRECTORY_LINK "/enterprise"
#define SECURITY_DIRECTORY_PATH "/data/enterprise"
int is_security_directory(const char *path)
{
	char cwd[PATH_MAX] = {0};

	if (path == NULL)
    {
        return 0;
    }

	if (strstr(path, SECURITY_DIRECTORY_LINK) != NULL)
    {
        return 1;
    }   
		

	if (strstr(path, SECURITY_DIRECTORY_PATH) != NULL)
    {
        return 1;
    }

	if (strstr(getcwd(cwd, PATH_MAX), SECURITY_DIRECTORY_LINK) != NULL)
    {
        return 1;
    }

	if (strstr(getcwd(cwd, PATH_MAX), SECURITY_DIRECTORY_PATH) != NULL)
    {
        return 1;
    }
    
	return 0;
}

#define PROP_PERSONAL_DOMAIN_STATE  "persist.encryption.enable"
static int get_personal_state(int domain)
{
    int user, state = 0;
    
    char needle[16] = {0};
    char value[PROP_VALUE_MAX] = {0};

    if (__system_property_get(PROP_PERSONAL_DOMAIN_STATE, value) == 0)
    {
        return state;
    }
    
    sprintf(needle, "%d", domain); // format string
    
    if (strstr(value, needle) != NULL)
    {
        sscanf(strstr(value, needle), "%d:%d", &user, &state);
    }

    return state;
}

#define PROP_SECURITY_DOMAIN    "persist.mdm.domain_id"
#define CONFIG_PATH             "/system/etc/security/config"
static int get_security_state(int domain)
{
    char value[PROP_VALUE_MAX] = {0};
    
    if (__system_property_get(PROP_SECURITY_DOMAIN, value) == 0)
    {
        return 0;
    }   

    if (atoi(value) != domain)
    {
        return 0;
    }

    if (access(CONFIG_PATH, F_OK) != 0)
    {
        return 0;
    }

    int fd = linker_open(CONFIG_PATH, O_RDONLY);
    if (fd > 0)
    {
		linker_read(fd, value, sizeof(value));
		linker_close(fd);
	}

    if (value[0] == '\0')
    {
        return 0;
    }

    trim(value);

    if (strncmp(strtok(value, ":"), "encrypt", 7) != 0)
    {
        return 0;
    }
	
	return atoi(strtok(NULL, ":"));
}

int get_domain_state(void)
{
    int domain = get_process_domain();
    if (domain == 0)
    {
        return 0;
    }

    int personal = get_personal_state(domain);
    if (personal != 0)
    {
        return personal;
    }

    int security = get_security_state(domain);
    if (security != 0)
    {
        return security;
    }

    return 0;
}

static struct listnode file_info_list;

void file_info_list_init(void)
{
    list_init(&file_info_list);
}

void file_info_list_deinit(void)
{
    struct listnode *item;

    while (! list_empty(&file_info_list))
    {
        item = list_head(&file_info_list);
        list_remove(item);
        free(node_to_item(item, struct file_info, node));
    }
}

struct file_info *file_info_list_add(const int fd, const char *path)
{
    struct file_info *info = (struct file_info *)calloc(1, sizeof(struct file_info));
    if (info == NULL)
    {
        return NULL;
    }

    info->file_descriptor= fd;
    memcpy(info->file_path, path, strlen(path) + 1);

    list_add_tail(&file_info_list, &info->node);
    return info;
}

struct file_info *file_info_list_query_by_fd(const int fd)
{
    struct listnode *item;

    list_for_each(item, &file_info_list)
    {
        struct file_info *info = node_to_item(item, struct file_info, node);
        if (info->file_descriptor == fd)
        {
            return info;
        }
    }

    return NULL;
}

void file_info_list_remove_by_fd(const int fd)
{
    struct listnode *item;

    list_for_each(item, &file_info_list)
    {
        struct file_info *info = node_to_item(item, struct file_info, node);
        if (info->file_descriptor == fd)
        {
            list_remove(item);
            free(info);
            break;
        }
    }
}

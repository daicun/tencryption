#include <errno.h>
#include <stdarg.h>
#include <pthread.h>

#include "util.h"
#include "ftype.h"
#include "linker.h"

static off_t get_file_size_by_path(const char *path)
{
    struct stat state;

    memset(&state, 0, sizeof(state));
	
    if (linker_stat(path, &state) == 0)
    {
		return state.st_size;
	}

    return 0;
}

static char *get_security_path_by_path(const char *path)
{
    char *npath = (char *) calloc(1, PATH_MAX);
    if (npath != NULL)
    {
        sprintf(npath, "%s.s", path);
    }
    
    return npath;
}

typedef struct pthread_rfs_arg_t
{
    pthread_t       id;				// thread id
    unsigned int    number;			// thread number
    unsigned int    postion;    	// thread postion
    unsigned int    bsize;			// block size
    unsigned int    tsize;			// total size
    unsigned char   path[PATH_MAX];	// path
} pthread_rfs_arg_t;

static void *pthread_rfs_run(void *arg)
{
    pthread_rfs_arg_t *ptr = (pthread_rfs_arg_t *) arg;
    pthread_t id  = ptr->id;
    int number    = ptr->number;
    int offset    = ptr->postion;
    int bsize     = ptr->bsize;
    int tsize     = ptr->tsize;

    char *path = get_security_path_by_path((char *) ptr->path);
    if (path == NULL)
    {
        return NULL;
    }
    
    LOGD("id: %d, number: %d, path: %s offset: %d, bsize: %d, tsize: %d,", (int)id, number, path, offset, bsize, tsize);
    
    if (number == 0 && offset == 0)
    {
        int fd = -1;

        unsigned char buff[4096] = {0};
        
        if (access(path, F_OK) == 0)
        {
            remove(path);
        }
        
        fd = linker_open(path, O_CREAT | O_TRUNC | O_WRONLY, 0700);
        if (fd > 0)
        {
            if (ftruncate(fd, tsize + FILE_HEADER_POLL_SIZE) < 0)
            {
                LOGD("error messages: %s", strerror(errno));
            }
            
            if (file_header_init((char *) buff) == FILE_TYPE_RIGHTS)
            {
                linker_pwrite(fd, buff, sizeof(buff), offset);
            }
            
            linker_close(fd);
        }
    }

    while (1)
    {
        if (access(path, F_OK) == 0 && is_encrypted_by_path(path) == FILE_TYPE_RIGHTS)
        {
            break;
        }
    }

    if (access(path, F_OK) == 0)
    {
        unsigned char buff[4096] = {0};
        
        LOGD("number: %d, old file path: %s", number, (char *) ptr->path);
        int ofd = linker_open((char *) ptr->path, O_RDONLY);
        if (ofd < 0)
        {
            return NULL;
        }
        LOGD("number: %d, new file path: %s", number, path);
        int nfd = linker_open(path, O_WRONLY);
        if (nfd < 0)
        {
            return NULL;
        }

        while ((bsize - 4096) >= 0)
        {
            int count = 0;
            
            memset(buff, 0, sizeof(buff));

            count = linker_pread(ofd, buff, sizeof(buff), offset);
            if (count == sizeof(buff))
            {
                offset = offset + count;
            }

            for (int i = 0; i < count/16; i++)
            {
                aes_encrypt_128bit(buff + i*16, buff + i*16);
            }

            linker_pwrite(nfd, buff, count, offset);

            bsize = bsize - 4096;
            //LOGD("number: %d, bsize: %d", number, bsize);
        }

        LOGD("number + 1: %d", number + 1);
        if ((number + 1) == THREADS_COUNT)
        {
            int count = 0;
            
            count = linker_pread(ofd, buff, sizeof(buff), offset);
            if (count > 0)
            {   
                offset = offset + 4096;
                
                for (int i = 0; i < count/16; i++)
                {
                    aes_encrypt_128bit(buff + i*16, buff + i*16);
                }
                linker_pwrite(nfd, buff, count, offset);
            }
        }
        else
        {
            free(path);
            path = NULL;
        }
        
        linker_close(ofd);
        linker_close(nfd);
    }
    
    return path;
}

int rebuild_file_struct(const char *path, off_t size)
{
    int fd, count, result = 0;

    unsigned char *buffer = NULL;

    if (path == NULL || path[0] == '\0')
    {
        return result;
    }

    if (size > 0)
    {
        if (size <= (4096 * THREADS_COUNT))
        {
            fd = linker_open(path, O_RDWR);
            if (fd > 0)
            {
                buffer = (unsigned char *) calloc(1, size + 4096);
                if (buffer != NULL)
                {
                    file_header_init((char *)buffer);
                    count = linker_pread(fd, buffer + 4096, size, 0);
                    if (count == size)
                    {
                        for (off_t i = 0; i < size/16; i++)
                        {
                            aes_encrypt_128bit(buffer + 4096 + i*16, buffer + 4096 + i*16);
                        }
                        
                        count = linker_pwrite(fd, buffer, size + 4096, 0);
                        if (count != (size + 4096))
                        {
                            LOGE("old file size: %d, new file size: %d, not match", (int)size, (int)count);
                            result = 0;
                        }
                        else
                        {
                            LOGD("old file size: %d, new file size: %d, complete match", (int)size, (int)count);
                            result = 1;
                        }
                    }
                    free(buffer);
                }
                linker_close(fd);
            }
        }
        else
        {
            off_t average;

            pthread_attr_t attr;
            pthread_rfs_arg_t args[THREADS_COUNT];

            if (pthread_attr_init(&attr) != 0 || pthread_attr_setstacksize(&attr, THREADS_STACK_SIZE) != 0)
            {
                return result;
            }

            average = size / THREADS_COUNT;
            while(average % 4096)
            {
                average++;
            }

            for (int i = 0; i < THREADS_COUNT; ++i)
            {
                memset(&args[i], 0, sizeof(pthread_rfs_arg_t));

                args[i].number = i;
                args[i].postion = (average*i);
                if (i < (THREADS_COUNT - 1))
                {
                    args[i].bsize = average;
                }
                else
                {
                    args[i].bsize = size - (average*i);
                }
                args[i].tsize = size;
                memcpy(args[i].path, path, PATH_MAX);

                if (pthread_create(&args[i].id, &attr, &pthread_rfs_run, &args[i]) != 0)
                {
                    LOGE("i: %d create, failed", i);
                }
            }
            
            if (pthread_attr_destroy(&attr) != 0)
            {
                LOGE("destroy: attr, failed");
            }

            buffer = (unsigned char *) calloc(1, 4096);
            if (buffer == NULL)
            {
                LOGD("buffer == NULL");
            }
            
            for (int i = 0; i < THREADS_COUNT; ++i)
            {
                void *np;
                
                if (pthread_join(args[i].id, &np) != 0)
                {
                    LOGE("i: %d join, failed", i);
                }
                else
                {
                    if (np != NULL)
                    {
                        LOGD("i: %d path: %s", i, (char *)np);
                        memcpy(buffer, np, strlen(np));
                        free(np);
                    }
                    else
                    {
                        LOGD("error i: %d path: %s", i, (char *)np);
                    }
                }
            }

            if (buffer != NULL && buffer[0] != '\0')
            {
                LOGD("n path: %s, n size: %d", (char *)buffer, (int)get_file_size_by_path((char *)buffer));
                rename((char *)buffer, path);
                free(buffer);
                result = 1;
            }
        }
    }
    return result;
}

int (*linker_dup)(int oldfd) = NULL;
int dup(int oldfd)
{
    int fd = -1;
    
    if (update_linker_running_state() == 0 || linker_dup == NULL)
        return fd;

    if (is_trust_process() == 0 || get_domain_state() == 0)
    {
        return linker_dup(oldfd);
    }

    if (file_info_list_query_by_fd(oldfd) == NULL && is_encrypted_by_fd(oldfd) == FILE_TYPE_NORMAL)
    {
        return linker_dup(oldfd);
    }

    fd = linker_dup(oldfd);
    if (fd > 0)
    {
        struct file_info *info = file_info_list_query_by_fd(oldfd);
        if (info != NULL)
        {
            file_info_list_add(fd, info->file_path);
        }
    }
    
    return fd;
}

int (*linker_stat)(const char *path, struct stat *buf) = NULL;
int stat(const char *path, struct stat *buf)
{
    int state = -1;
    
    if (update_linker_running_state() == 0 || linker_stat == NULL)
        return state;

    if (is_trust_process() == 0 || get_domain_state() == 0)
    {
        return linker_stat(path, buf);
    }

    state = linker_stat(path, buf);
    if (state == 0 && is_encrypted_by_path(path) == FILE_TYPE_RIGHTS)
    {
        buf->st_size = buf->st_size - FILE_HEADER_POLL_SIZE;
    }

    // LOGD("%s\t path: %s size: %d", __FUNCTION__, path, (int)buf->st_size);

    return state;
}

int (*linker_lstat)(const char *path, struct stat *buf) = NULL;
int lstat(const char *path, struct stat *buf)
{
    int state = -1;
    
    if (update_linker_running_state() == 0 || linker_lstat == NULL)
    {
        return state;
    }

    if (is_trust_process() == 0 || get_domain_state() == 0)
    {
        return linker_lstat(path, buf);
    }
    
    state = linker_lstat(path, buf);
    if (state == 0 && is_encrypted_by_path(path) == FILE_TYPE_RIGHTS)
    {
        buf->st_size = buf->st_size - FILE_HEADER_POLL_SIZE;
    }

    // LOGD("%s\t path: %s size: %d", __FUNCTION__, path, (int)buf->st_size);

    return state;
}

int (*linker_fstat)(int fd, struct stat *buf) = NULL;
int fstat(int fd, struct stat *buf)
{
    int state = -1;
    
    if (update_linker_running_state() == 0 || linker_fstat == NULL)
        return state;

    if (is_trust_process() == 0 || get_domain_state() == 0)
    {
        return linker_fstat(fd, buf);
    }

    state = linker_fstat(fd, buf);
    if (state == 0 && is_encrypted_by_fd(fd) == FILE_TYPE_RIGHTS)
    {
        buf->st_size = buf->st_size - FILE_HEADER_POLL_SIZE;
    }

    // LOGD("%s\t fd: %d size: %d", __FUNCTION__, fd, (int)buf->st_size);
    
    return state;
}

int (*linker_open)(const char *path, int flags, ...) = NULL;
int open(const char *path, int flags, ...)
{
    int fd = -1, fs = 0;
    
    mode_t mode = 0;

    char *header = NULL;

    if (update_linker_running_state() == 0 || linker_open == NULL)
        return fd;
    
	if (flags & O_CREAT)
    {
	    va_list  args;

	    va_start(args, flags);
	    mode = (mode_t) va_arg(args, int);
	    va_end(args);
	}

    if (is_trust_process() == 0 || get_domain_state() == 0)
    {
        return linker_open(path, flags, mode);
    }
        

    if (is_support_file_type_by_path(path) == 0)
    {
        return linker_open(path, flags, mode);
    }
    
    LOGD("%s\t flags: %o, mode: %o", __FUNCTION__, flags, mode);
    
    fs = access(path, F_OK);
    if (fs == 0)
    {
        if ((fd = linker_open(path, O_RDONLY)) > 0)
        {
            header = (char *) calloc(1, FILE_HEADER_POLL_SIZE);
            if (header != NULL)
            {
                if (linker_read(fd, header, FILE_HEADER_POLL_SIZE) != FILE_HEADER_POLL_SIZE)
                {
                    free(header);
                    header = NULL;
                }
                else
                {
                    if (is_encrypted_by_file_header(header) != FILE_TYPE_RIGHTS)
                    {
                        free(header);
                        header = NULL;
                    }
                }
            }
    		linker_close(fd);
	    }
    }
    
    LOGD("%s\t header context: %s", __FUNCTION__, header);
    
    fd = linker_open(path, flags, mode);
    if (fd > 0)
    {
        if (fs != 0)
        {
            if (is_support_special_file_type_by_path(path) == 1)
            {
                LOGD("%s\t special new file", __FUNCTION__);
            }
            else
            {
                LOGD("%s\t common new file", __FUNCTION__);
                struct file_info *fi = file_info_list_add(fd, path);
                if (fi != NULL)
                {
                    file_header_init(&fi->header);
                    if (linker_write(fd, &fi->header, FILE_HEADER_POLL_SIZE) != FILE_HEADER_POLL_SIZE)
                    {
                        file_info_list_remove_by_fd(fd);
                        linker_close(fd);
                        fd = -1;
                    }
                }
            }
        }
        else
        {
            LOGD("%s\t open old file", __FUNCTION__);
            if (header == NULL)
            {
                LOGD("%s\t not encryption header", __FUNCTION__);

                char *ptr = get_security_path_by_path(path);
                if (ptr != NULL)
                {
                    if(access(ptr, F_OK) != 0)
                    {
                        linker_close(fd); // close old file
                        if (rebuild_file_struct(path, get_file_size_by_path(path)) == 1)
                        {
                            LOGD("%s\t rebuild file struct, success", __FUNCTION__);
                            fd = linker_open(path, flags, mode);
                            
                            struct file_info *fi = file_info_list_add(fd, path);
                            if (fi != NULL)
                            {
                                linker_lseek(fd, FILE_HEADER_POLL_SIZE, SEEK_SET);
                            }
                        }
                        else
                        {
                            LOGD("%s\t rebuild file struct, failed", __FUNCTION__);
                            
                            fd = linker_open(path, flags, mode);
                        }
                    }
                }
                free(ptr);
            }
            else
            {
                LOGD("%s\t found encryption header", __FUNCTION__);
                
                struct file_info *fi = file_info_list_add(fd, path);
                if (fi != NULL)
                {
                    memcpy(&fi->header, header, sizeof(fi->header));
                    if ((flags & O_APPEND) == 0)
                    {
                        linker_lseek(fd, FILE_HEADER_POLL_SIZE, SEEK_SET);
                    }
                }
            }
        }
    }
    else
    {
        LOGD("%s\t error messages: %s", __FUNCTION__, strerror(errno));
    }
    
    if (header != NULL)
    {
        free(header);
    }
    return fd;
}

ssize_t (*linker_read)(int fd, void *buf, size_t count) = NULL;
ssize_t read(int fd, void *buf, size_t count)
{
    ssize_t size = -1;
    
    if (update_linker_running_state() == 0 || linker_read == NULL)
    {
        return size;
    }

    if (is_trust_process() == 0 || get_domain_state() == 0)
    {
        return linker_read(fd, buf, count);
    }

    struct stat *state = (struct stat *) calloc(1, sizeof(struct stat));
	if (state == NULL)
    {
		return linker_read(fd, buf, count);
	}

	if (linker_fstat(fd, state) < 0 || ! S_ISREG(state->st_mode))
    {
		free(state);
		return linker_read(fd, buf, count);
	}
	free(state);

    if (file_info_list_query_by_fd(fd) == NULL && is_encrypted_by_fd(fd) == FILE_TYPE_NORMAL)
    {
        return linker_read(fd, buf, count);
    }
    
    off_t offset = linker_lseek(fd, 0, SEEK_CUR);
    if (offset < FILE_HEADER_POLL_SIZE)
    {
        offset = offset + FILE_HEADER_POLL_SIZE;
        linker_lseek(fd, FILE_HEADER_POLL_SIZE, SEEK_SET);
    }
    // LOGD("%s\t decrypt data, fd: %d count: %d offset: %d", __FUNCTION__, fd, count, (int) offset);
    
    ssize_t tmpcount = count + offset%16 + 16 - (count+offset)%16;
    unsigned char *tmpbuf = (unsigned char*) calloc(1, tmpcount);

    ssize_t size_1 = linker_pread(fd, tmpbuf, offset%16, offset-offset%16);
    ssize_t size_2 = linker_read(fd, tmpbuf + offset%16, count);
    ssize_t size_3 = linker_pread(fd, tmpbuf + offset%16 + count, 16 - (count + offset)%16, offset + count);

    if(size_3 > 0)
    {
        size = size_1 + size_2 + size_3;
    }
    else
    {
        size = size_1 + size_2;
    }

    if (size > 0)
    {
        for (int i = 0; i < size/16; i++)
        {
            aes_decrypt_128bit(tmpbuf + i*16,tmpbuf + i*16);
        }
        memcpy(buf, tmpbuf+offset%16, size_2);
    }
    
    free(tmpbuf);
    return size_2;
}

ssize_t (*linker_write)(int fd, const void *buf, size_t count) = NULL;
ssize_t write(int fd, const void *buf, size_t count)
{
    ssize_t size = -1;
    
    if (update_linker_running_state() == 0 || linker_write == NULL)
    {
        return size;
    }

    if (is_trust_process() == 0 || get_domain_state() == 0)
    {
        return linker_write(fd, buf, count);
    }

    if (count > 0)
    {
        off_t offset = linker_lseek(fd, 0, SEEK_CUR);

        unsigned char *temp = (unsigned char *) calloc(1, count + offset%16);
        if (temp != NULL)
        {
            struct file_info *fi = file_info_list_query_by_fd(fd);

            int rfd = linker_open(fi->file_path, O_RDWR);

            ssize_t rc1 = linker_pread(rfd, temp, offset%16, offset - offset%16);
            
            memcpy (temp + offset%16, buf, count);

            for (int i = 0; i < (count + offset%16)/16; i++)
			{
				aes_encrypt_128bit(temp + i*16, temp + i*16);
			}

            ssize_t rc2 = linker_pwrite(fd, temp, offset%16,offset-offset%16);
			linker_close(rfd);
				
			size = linker_write(fd, temp + offset%16, count);
			free(temp);
        }
    }
    else
    {
        size = linker_write(fd, buf, count);
    }

    return size;
}

int (*linker_close)(int fd) = NULL;
int close(int fd)
{
    if (update_linker_running_state() == 0 || linker_close == NULL)
        return -1;
    
    if (is_trust_process() == 0 || get_domain_state() == 0)
        return linker_close(fd);

    struct file_info *info = file_info_list_query_by_fd(fd);
    if (info != NULL)
    {
        linker_lseek(fd, 0, SEEK_SET);
        file_info_list_remove_by_fd(fd);
    }
    
    return linker_close(fd);
}

off_t (*linker_lseek) (int fd, off_t offset, int whence) = NULL;
off_t lseek(int fd, off_t offset, int whence)
{
    off_t position = -1;
    
    if (update_linker_running_state() == 0 || linker_lseek == NULL)
        return position;

    if (is_trust_process() == 0 || get_domain_state() == 0)
        return linker_lseek(fd, offset, whence);

    if (file_info_list_query_by_fd(fd) == NULL && is_encrypted_by_fd(fd) == FILE_TYPE_NORMAL)
        return linker_lseek(fd, offset, whence);

    // LOGD("%s\t fd: %d offset: %d whence: %d", __FUNCTION__, fd, (int)offset, whence);

    if (whence == SEEK_SET)
    {
        offset += FILE_HEADER_POLL_SIZE;
    }
    position = linker_lseek(fd, offset, whence);

    return (position - FILE_HEADER_POLL_SIZE);
}

off64_t (*linker_lseek64)(int fd, off64_t offset, int whence) = NULL;
off64_t lseek64(int fd, off64_t offset, int whence)
{
    off64_t position = -1;

    if (update_linker_running_state() == 0 || linker_lseek64 == NULL)
        return position;

    if (is_trust_process() == 0 || get_domain_state() == 0)
        return linker_lseek64(fd, offset, whence);

    if (file_info_list_query_by_fd(fd) == NULL && is_encrypted_by_fd(fd) == FILE_TYPE_NORMAL)
        return linker_lseek64(fd, offset, whence);

    // LOGD("%s\t fd: %d offset: %d whence: %d", __FUNCTION__, fd, (int)offset, whence);

    if (whence == SEEK_SET)
    {
		offset += FILE_HEADER_POLL_SIZE;
    }
    position = linker_lseek64(fd, offset, whence);

    return (position - FILE_HEADER_POLL_SIZE);
}

ssize_t (*linker_pread)(int fd, void *buf, size_t count, off_t offset) = NULL;
ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    if (update_linker_running_state() == 0 || linker_pread == NULL)
        return -1;

    if (is_trust_process() == 0 || get_domain_state() == 0)
        return linker_pread(fd, buf, count, offset);
    
    return linker_pread(fd, buf, count, offset);
}

ssize_t (*linker_pwrite)(int fd, const void *buf, size_t count, off_t offset) = NULL;
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    if (update_linker_running_state() == 0 || linker_pwrite == NULL)
        return -1;

    if (is_trust_process() == 0 || get_domain_state() == 0)
        return linker_pwrite(fd, buf, count, offset);

    return linker_pwrite(fd, buf, count, offset);
}

void *(*linker_mmap)(void *addr, size_t length, int prot, int flags, int fd, off_t offset) = NULL;
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    if (update_linker_running_state() == 0 || linker_mmap == NULL)
        return MAP_FAILED;

    if (is_trust_process() == 0 || get_domain_state() == 0)
        return linker_mmap(addr, length, prot, flags, fd, offset);

    if (file_info_list_query_by_fd(fd) == NULL && is_encrypted_by_fd(fd) == FILE_TYPE_NORMAL)
        return linker_mmap(addr, length, prot, flags, fd, offset);

    if ((prot & PROT_READ) != 0)
    {
        struct stat state;

        memset(&state, 0, sizeof(state));
        linker_fstat(fd, &state);

        int len = state.st_size - FILE_HEADER_POLL_SIZE;
        void *result = linker_mmap(addr, len, prot|PROT_EXEC|PROT_WRITE, flags, fd, offset + FILE_HEADER_POLL_SIZE);
        if (result != MAP_FAILED)
        {
            for (int i = 0; i < (len/16); ++i)
                aes_decrypt_128bit((unsigned char *)result + i*16, (unsigned char *)result + i*16);
            return result;
        }
	}
	
	return linker_mmap(addr, length, prot, flags, fd, offset);
}

size_t (*linker_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream) = NULL;
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t result = 0;
    
    if (update_linker_running_state() == 0 || linker_fread == NULL)
        return result;

    if (is_trust_process() == 0 || get_domain_state() == 0)
        return linker_fread(ptr, size, nmemb, stream);

    if (is_encrypted_by_stream(stream) == FILE_TYPE_NORMAL) 
		return linker_fread(ptr, size, nmemb, stream);

    long offset = ftell(stream);
	LOGD("%s\t offset: %d", __FUNCTION__, (int)offset);
	if (offset < ((off_t)FILE_HEADER_POLL_SIZE)) {
		offset += FILE_HEADER_POLL_SIZE;
		fseek(stream, FILE_HEADER_POLL_SIZE, SEEK_SET);
	}

    size_t count = size * nmemb;
	LOGD("%s\t count: %d", __FUNCTION__, (int)count);
	size_t temp_count = count + offset%16 + 16 - (count+offset)%16;
	LOGD("%s\t temp_count: %d", __FUNCTION__, (int)temp_count);
	unsigned char *temp = (unsigned char*) calloc(temp_count, sizeof(unsigned char));
	if (temp == NULL) 
		return result;

	fseek(stream, (offset - offset%16), SEEK_SET);
	size_t result1 = linker_fread(temp, sizeof(unsigned char), offset%16, stream);
	
	fseek(stream, offset, SEEK_SET);
	size_t result2 = linker_fread(temp + offset%16, sizeof(unsigned char), count, stream);
	
	fseek(stream, offset + count, SEEK_SET);
	size_t result3 = linker_fread(temp + offset%16 + count, sizeof(unsigned char), 16 - (count+offset)%16, stream);
	
	fseek(stream, offset + count, SEEK_SET);
	LOGD("LOLLIPOP %s \t result1: %d, result2: %d, result3: %d", __FUNCTION__, (int)result1, (int)result2, (int)result3);

	if(result3 > 0)
        result = result1 + result2 + result3;
    else
        result = result1 + result2;

	if (result > 0) {
		for (unsigned int i = 0; i < result/16; i++) {
			aes_decrypt_128bit(temp + i*16, temp + i*16);
		}
		memcpy(ptr, temp + offset%16, result2);
	}
	free(temp);
	return result2;
}

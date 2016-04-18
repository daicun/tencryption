#include <aes.h>
#include <string.h>

#include "log.h"
#include "util.h"
#include "linker.h"
#include "constants.h"

static const char messages[64] = "china mobile research institute";

static const unsigned char userKey[AES_BLOCK_SIZE] = { 
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C,
    0x0D, 0x0E, 0x0F, 0x10
};

int file_header_init(char *header)
{
    struct file_header *ptr = (struct file_header *) header;

    memcpy(ptr->common.messages, messages, sizeof(messages));
    
    memcpy(ptr->common.signature, SIGNATURE, SIGNATURE_LENGTH);

    ptr->common.major = MAJOR_VERSION;
    ptr->common.minor = MINOR_VERSION;
    
    char *process_name = get_process_name();
    if (process_name == NULL)
    {
        memcpy(ptr->common.name, "unknown", 7);
    }
    else
    {
        memcpy(ptr->common.name, process_name, strlen(process_name));
        free(process_name);
    }

    memset(ptr->reserve, '*', sizeof(ptr->reserve));

    ptr->kernel.file_type = FILE_TYPE_RIGHTS;
    
    return ptr->kernel.file_type;
}

int is_encrypted_by_file_header(char *header)
{
    struct file_header *ptr = (struct file_header *) header;
    
    if (memcmp(ptr->common.signature, SIGNATURE, SIGNATURE_LENGTH) != 0)
    {
        return FILE_TYPE_NORMAL;
    }
    
    return ptr->kernel.file_type;
}

int is_encrypted_by_fd(int fd)
{
	if ((fcntl(fd, F_GETFL) & O_WRONLY) == O_WRONLY)
    {
        return FILE_TYPE_NORMAL;
    }
    
    int nfd = linker_dup(fd);
    if (nfd > 0)
    {
        char header[FILE_HEADER_POLL_SIZE] = { 0 };

        linker_pread(nfd, header, sizeof(header), 0);

        linker_close(nfd);

        return is_encrypted_by_file_header(header);
    }
    
    return FILE_TYPE_NORMAL;
}

int is_encrypted_by_path(const char *path)
{
    int fd = linker_open(path, O_RDONLY);
	if (fd > 0)
    {
        char header[FILE_HEADER_POLL_SIZE] = { 0 };

        linker_read(fd, header, FILE_HEADER_POLL_SIZE);

        linker_close(fd);

        return is_encrypted_by_file_header(header);
	}

	return FILE_TYPE_NORMAL;
}

int is_encrypted_by_stream(FILE *stream)
{    
    char header[FILE_HEADER_POLL_SIZE] = { 0 };
    
    long offset = ftell(stream);
    if (offset < 0)
    {
        return FILE_TYPE_NORMAL;
    }

    fseek(stream, 0, SEEK_SET);
    linker_fread(header, 1, FILE_HEADER_POLL_SIZE, stream);
    fseek(stream, offset, SEEK_SET);
    
    return is_encrypted_by_file_header(header);
}

void aes_encrypt_128bit(unsigned char *in, unsigned char *out)
{
    AES_KEY key;

    if (AES_set_encrypt_key(userKey, 128, &key) < 0) {
        LOGE("%s:\t unable to set encryption key in AES", __FUNCTION__);
        return ;
    }

    unsigned char _in[AES_BLOCK_SIZE] = {0};
    memcpy(_in, in, AES_BLOCK_SIZE);

    // encrypt (iv will change)
    AES_encrypt(in, out, &key);
}

void aes_decrypt_128bit(unsigned char *in, unsigned char *out)
{
    AES_KEY key;

    if (AES_set_decrypt_key(userKey, 128, &key) < 0) {
        LOGE("%s:\t unable to set encryption key in AES", __FUNCTION__);
        return ;
    }

    unsigned char _in[AES_BLOCK_SIZE] = {0};
    memcpy(_in, in, AES_BLOCK_SIZE);

    // decrypt
    AES_decrypt(in, out, &key);
}

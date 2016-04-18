#ifndef _TE_CRYPTO_H
#define _TE_CRYPTO_H
#include <stdio.h>

#pragma pack(push, 4)
#define SIGNATURE "\x08\x88\x41\xF3\xCD\x83\xE8\xC0\x8F\x29\x03\xDC\x31\xE5\xF5\xA8"
#define SIGNATURE_LENGTH 16

#define MAJOR_VERSION 1
#define MINOR_VERSION 1

struct common_info {
    unsigned char   messages[64];   // display messages
    unsigned char   signature[16];  // signature code
    unsigned short  major;          // major version
    unsigned short  minor;          // minor version
    unsigned char   name[256];      // process name
    unsigned char   reserve[44];    // reserved space
};

#define FILE_TYPE_NORMAL 0x00a0 // normal file
#define FILE_TYPE_RIGHTS 0x00a1 // encrypted file

struct kernel_info {
    unsigned short  file_type;      // FILE_TYPE_NORMAL or FILE_TYPE_RIGHTS
    unsigned char   reserve[126];   // reserved space
};

struct file_header {
    struct common_info common;          // common info
    unsigned char reserve[28 * 128];    // reserved space
    struct kernel_info kernel;          // kernel info
};

#define FILE_HEADER_POLL_SIZE   (sizeof(struct file_header))
#pragma pack(pop)

int file_header_init(char *header);

int is_encrypted_by_file_header(char *header);
int is_encrypted_by_fd(int fd);
int is_encrypted_by_path(const char *path);
int is_encrypted_by_stream(FILE *stream);

void aes_encrypt_128bit(unsigned char *in, unsigned char *out);
void aes_decrypt_128bit(unsigned char *in, unsigned char *out);
#endif

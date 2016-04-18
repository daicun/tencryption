#ifndef _TE_FTYPE_H
#define _TE_FTYPE_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// document
#define TID_DOC         100
#define TID_DOCX        101
#define TID_DOT         102
#define TID_DOTX        103
#define TID_WPS         104
#define TID_WPT         105
#define TID_PPT         106
#define TID_PPTX        107
#define TID_POT         108
#define TID_POTX        109
#define TID_PPS         110
#define TID_DPS         111
#define TID_DPT         112
#define TID_XLS         113
#define TID_XLT         114
#define TID_XLSX        115
#define TID_XLTX        116
#define TID_ET          117
#define TID_ETT         118
#define TID_PDF         119
#define TID_TXT         120

// image
#define TID_JPG         200
#define TID_JPEG        201
#define TID_JPEG2000    202
#define TID_PNG         203
#define TID_BMP         204
#define TID_GIF         205
#define TID_TIF         206
#define TID_TIFF        207

// audio
#define TID_CD          300
#define TID_WAV         301
#define TID_MP3         302
#define TID_WMA         303
#define TID_APE         304
#define TID_FLAC        305
#define TID_MID         306

// vedio
#define TID_AVI         400
#define TID_MOV         401
#define TID_WMV         402
#define TID_3GP         403
#define TID_MP4         404
#define TID_MP4_TMP     405
#define TID_RMVB        406
#define TID_MPEG        407

// compressed
#define TID_RAR         500
#define TID_ZIP         501

int getfid(const char *filename);

#ifdef __cplusplus
}
#endif
#endif

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "constants.h"

enum {
    TID_DOC = 100,
    TID_DOCX,
    TID_DOT,
    TID_DOTX,
    TID_WPS,
    TID_WPT,
    TID_PPT,
    TID_PPTX,
    TID_POT,
    TID_POTX,
    TID_PPS,
    TID_DPS,
    TID_DPT,
    TID_XLS,
    TID_XLT,
    TID_XLSX,
    TID_XLTX,
    TID_ET,
    TID_ETT,
    TID_PDF,
    TID_TXT,

    TID_JPG = 200,
    TID_JPEG,
    TID_JPEG2000,
    TID_PNG,
    TID_BMP,
    TID_GIF,
    TID_TIF,
    TID_TIFF,

    TID_CD = 300,
    TID_WAV,
    TID_MP3,
    TID_WMA,
    TID_APE,
    TID_FLAC,
    TID_MID,

    TID_AVI= 400,
    TID_MOV,
    TID_WMV,
    TID_3GP,
    TID_MP4,
    TID_MP4_TMP,
    TID_RMVB,
    TID_MPEG,

    TID_RAR = 500,
    TID_ZIP
};

struct type {
    const char *name;
    const unsigned int id;
};

static const struct type types[] = {
    {".doc",            TID_DOC,        },
    {".docx",           TID_DOCX,       },
    {".dot",            TID_DOT,        },
    {".dotx",           TID_DOTX,       },
    {".wps",            TID_WPS,        },
    {".wpt",            TID_WPT,        },
    {".ppt",            TID_PPT,        },
    {".pptx",           TID_PPTX,       },
    {".pot",            TID_POT,        },
    {".potx",           TID_POTX,       },
    {".pps",            TID_PPS,        },
    {".dps",            TID_DPS,        },
    {".dpt",            TID_DPT,        },
    {".xls",            TID_XLS,        },
    {".xlt",            TID_XLT,        },
    {".xlsx",           TID_XLSX,       },
    {".xltx",           TID_XLTX,       },
    {".et",             TID_ET,         },
    {".ett",            TID_ETT,        },
    {".pdf",            TID_PDF,        },
    {".txt",            TID_TXT,        },

    {".jpg",            TID_JPG,        },
    {".jpeg",           TID_JPEG,       },
    {".jpeg2000",       TID_JPEG2000    },
    {".png",            TID_PNG,        },
    {".bmp",            TID_BMP,        },
    {".gif",            TID_GIF,        },
    {".tif",            TID_TIF,        },
    {".tiff",           TID_TIFF,       },

    {".cd",             TID_CD,         },
    {".wav",            TID_WAV,        },
    {".mp3",            TID_MP3,        },
    {".wma",            TID_WMA,        },
    {".ape",            TID_APE,        },
    {".flac",           TID_FLAC,       },
    {".mid",            TID_MID,        },

    {".avi",            TID_AVI,        },
    {".mov",            TID_MOV,        },
    {".wmv",            TID_WMV,        },
    {".3gp",            TID_3GP,        },
    {".mp4",            TID_MP4,        },
    {".mp4.tmp",        TID_MP4_TMP,    },
    {".rmvb",           TID_RMVB,       },
    {".mpeg",           TID_MPEG,       },

    {".rar",            TID_RAR,        },
    {".zip",            TID_ZIP,        }
};

#define count (sizeof(types) / sizeof(types[0]))

int get_tid_by_path(const char *path)
{
    if (strrchr(path, '.') != NULL)
    {
        for (size_t index = 0; index < count; index++)
        {
            char *type = strcasestr(path, types[index].name);
            if (type == NULL)
            {
                continue;
            }
            else
            {
                if (strncasecmp(type, types[index].name, strlen(type)) == 0)
                {
                    return types[index].id;
                }
            }
        }
    }

    return 0;
}

int is_special_type_by_tid(const size_t tid)
{
    switch (tid)
    {
        case TID_MP4_TMP:
        {
            return 1;
        }
        break;

        default:
        {
            LOGW("tid: [%d] is common type", tid);
        }
        break;
    }

    return 0;
}

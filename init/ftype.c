#include "ftype.h"

struct ftype
{
    const char *name;
    const unsigned int tid;
};

static const struct ftype flist[] =
{
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

#define fcount (sizeof(flist) / sizeof(flist[0]))

// int getfid(const char *filename)
// {
//     // returns a pointer to the last occurrence of the character c in the string s.
//     if (! (char *sname = strchr(filename, '.')))
//     {
//         return 0;
//     }
//
//     for (size_t index = 0; index < count; index++)
//     {
//
//     }
//
//     return 0;
// }

int getfid(const char *filename)
{
    char *sname;

    // returns a pointer to the last occurrence of the character c in the string s.
    if (! (sname = strchr(filename, '.')))
    {
        return 0;
    }

    for (size_t index = 0; index < fcount; index++)
    {
        // strcasestr(sname, types[index].name);
        if (! strcasestr(sname, flist[index].name))
        {
            continue;
        }
        else
        {
            if (! strncasecmp(sname, flist[index].name, strlen(sname)))
            {
                return flist[index].tid;
            }
        }
    }

    return 0;
}

int checkfid(const int fid)
{
    return 0;
}

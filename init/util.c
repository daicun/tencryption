#include "util.h"

void trim(char *str)
{
    size_t j = 0;

    for (size_t i = 0; i < strlen(str); ++i)
    {
        if (isspace(str[i]))
			continue;

        str[j] = str[i];
        j++;
    }

    str[j] = '\0';
}

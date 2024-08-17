#include "game.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

const char *log_level_strings[LogLevelCount] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

void log_ex(LogLevel log_level, const char *text, ...)
{
    char fmt_buf[KiloBytes(5)];
    sprintf(fmt_buf, "[%s]: %s\n", log_level_strings[log_level], text);

    va_list args;
    va_start(args, text);
    vprintf(fmt_buf, args);
    va_end(args);
    fflush(stdout);
}

u8 *read_file(const char *path, u32 *bytes_read)
{
    FILE *fptr = fopen(path, "rb");
    if (fptr == NULL) 
    {
        error("Failed to read file: %s", path);
        return NULL;
    }
    fseek(fptr, 0, SEEK_END);
    i32 len = ftell(fptr);
    u8 *buf = (u8*) malloc(len + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, len, 1, fptr);
    buf[len] = 0;
    if (bytes_read)
        *bytes_read = len;
    fclose(fptr);
    return buf;
}

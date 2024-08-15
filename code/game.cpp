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
    char fmt_buf[KiloBytes(1)];
    sprintf(fmt_buf, "[%s]: %s\n", log_level_strings[log_level], text);

    va_list args;
    va_start(args, text);
    vprintf(fmt_buf, args);
    va_end(args);
    fflush(stdout);
}

void _assert(const char *file, u32 line, const char *expression, bool value)
{
    if (!value)
    {
        error("Assert (%s:%u) failed: %s", file, line, expression);
        exit(1);
    }
}

u32 read_file(const char *path, u8 *buffer, u32 buffer_size)
{
    u32 bytes_read = 0;
    FILE *file = {};
    if (fopen_s(&file, path, "rb") == 0) 
    {
        bytes_read = fread(buffer, 1, buffer_size, file);
        fclose(file);
    } 
    else 
    {
        error("Failed to read file %s", path);
    }

    return bytes_read;
}

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

u8 *read_file(const char *path, u32 *bytes_read)
{
    *bytes_read = 0;
    FILE *file = {};
    if (fopen_s(&file, path, "rb") == 0) 
    {
        fseek(file, 0, SEEK_END);
        *bytes_read = ftell(file);
        fseek(file, 0, SEEK_SET);

        u8 *buffer = (u8 *) malloc(*bytes_read);
        fread(buffer, 1, *bytes_read, file);
        fclose(file);

        return buffer;
    } 
    else 
    {
        error("Failed to read file %s", path);
    }

    return NULL;
}

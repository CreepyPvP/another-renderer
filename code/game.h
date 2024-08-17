#pragma once

#include <stdint.h>
#include <assert.h>

#define DEBUG

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef float f32;
typedef double f64;

#define KiloBytes(Count) (1024 * Count)
#define MegaBytes(Count) (1024 * KiloByte(Count))

#define advance_pointer(pointer, size) (((u8 *) pointer) + size)

enum LogLevel
{
    Log_Debug,
    Log_Info,
    Log_Warning,
    Log_Error,
    LogLevelCount
};

#define debug(text, ...) log_ex(Log_Debug, text, __VA_ARGS__)
#define info(text, ...) log_ex(Log_Info, text, __VA_ARGS__)
#define warn(text, ...) log_ex(Log_Warning, text, __VA_ARGS__)
#define error(text, ...) log_ex(Log_Error, text, __VA_ARGS__)

void log_ex(LogLevel log_level, const char *text, ...);

u8* read_file(const char* path, u32* bytes_read);

#pragma once

#if defined(_WIN64) || defined(_WIN32)
#if defined(FLASHIMAGECHECKER_EXPORTS)
#define LIBEX_EXPORT __declspec(dllexport)
#else
#define LIBEX_EXPORT __declspec(dllimport)
#endif

#define LIB_API _cdecl
#elif defined(_LINUX64)
#if defined(FLASHIMAGECHECKER_EXPORTS)
#define LIBEX_EXPORT __attribute__((visibility("default")))
#else
#define LIBEX_EXPORT
#endif
#define LIB_API
#else
#error "win32 or win64 or linux64 platform is not specified."
#endif

#include <stdint.h>
#include "Flash.Mode.Struct.h"
#define STATUS_IMAGE_CHK_SUCCEEDED 0
#define STATUS_IMAGE_CHK_FAILED -1
#define STATUS_IMAGE_CHK_XML_NOT_FOUND -2
#define STATUS_IMAGE_CHK_CANCELLED -3

#ifdef __cplusplus
extern "C" {
#endif

LIBEX_EXPORT int LIB_API 
flashtool_images_checker_label(const char *scatter_file_path, const struct callback_transfer_t *callbacks);

LIBEX_EXPORT int LIB_API
flashtool_images_checker_verify(const char *scatter_file_path, const struct callback_transfer_t *callbacks);

#ifdef __cplusplus
}
#endif

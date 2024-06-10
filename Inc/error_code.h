#pragma once
#if defined(_WIN64) || defined (_WIN32)
#include <winerror.h>
#else
using HRESULT = int;
#endif

using ZHRESULT = HRESULT;

#define SUCCESSED(code) (((ZHRESULT)(code)) >= 0) // SUCCEEDED used by win32.
#define FAIL(code) (((ZHRESULT)(code)) < 0)

#define E_OK 0
#define E_ERROR -1
#define E_CANCEL -2
#define E_UNSUPPORTED -3

//plugin error code
#define E_NO_INTERFACE -100
#define E_NO_CLS_ID -101
#define E_FILE_NOT_FOUND -102
#define E_LOAD_REG_DLL_FAIL -103
#define E_LOAD_REG_XML_FAIL -104
#define E_NO_HMODULE_FOUND -105
#define E_NO_DLLGETCLASSOBJECT -106
#define E_NO_DLLGETCLASSTYPE -107
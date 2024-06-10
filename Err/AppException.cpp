/*
 * AppException.cpp
 *
 *  Created on: Sep 2, 2011
 *      Author: MTK81019
 */

#include "AppException.h"
#include "CFlashToolErrorCode.h"

AppException::AppException(int error_code, const QString& message,
        const char* func, const char* file, int line)
    :BaseException(func,file,line),
      err_code_(error_code),
      err_msg_(message)
{

}

AppException::~AppException() noexcept
{
}

QString AppException::err_msg() const
{
    if (err_msg_.length() <= 0) {
        return ErrorLookup::CFlashToolErrorMessage(static_cast<CFlashToolErrorCode>(err_code_));
    }
    return err_msg_;
}




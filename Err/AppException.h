/*
 * AppException.h
 *
 *  Created on: Sep 2, 2011
 *      Author: MTK81019
 */

#ifndef APPEXCEPTION_H_
#define APPEXCEPTION_H_

#include "BaseException.h"

class AppException : public BaseException
{
public:
    AppException(int error_code, const QString & message,
                 const char* func="", const char* file="", int line=0);
    virtual ~AppException() noexcept;

    virtual QString err_msg() const;
    virtual int err_code() const { return err_code_;}

private:
    QString err_msg_;
    int err_code_;
};

#endif /* APPEXCEPTION_H_ */

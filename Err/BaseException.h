/*
 * BaseException.h
 *
 *  Created on: Sep 2, 2011
 *      Author: MTK81019
 */

#ifndef BASEEXCEPTION_H_
#define BASEEXCEPTION_H_

#include <exception>
#include <QString>

class BaseException : public std::exception
{
public:
    BaseException(const char* func, const char* file, int line);
    virtual ~BaseException() noexcept;

    virtual QString err_msg() const = 0;
    virtual int err_code() const = 0;

    QString context() const;

private:
    const char* func_;
    const char* file_;
    int line_;
};

#endif /* BASEEXCEPTION_H_ */

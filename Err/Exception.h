/*
 * Exception.h
 *
 *  Created on: Sep 2, 2011
 *      Author: MTK81019
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "AppException.h"

#define THROW_APP_EXCEPTION(error_code, error_msg) \
                do{ throw AppException(error_code, error_msg,  __FUNCTION__, __FILE__, __LINE__); } while(0);


#endif /* EXCEPTION_H_ */

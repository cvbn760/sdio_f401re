//#ifndef _APP_CFG_H_
//#define _APP_CFG_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "string.h"
#include "gpio.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef char                CHAR;     /*  8-bit plain char used for character data */
typedef float               FLOAT32;  /* 32-bit floating point */
typedef double              FLOAT64;  /* 64-bit extended floating point */

typedef signed char         INT8;     /*  8-bit signed integer, used for numeric data */
typedef signed short        INT16;    /* 16-bit signed integer */
typedef signed int          INT32;    /* 32-bit signed integer */
typedef signed long long    INT64;    /* 64-bit signed integer */

typedef unsigned char       UINT8;    /*  8-bit unsigned integer, used for numeric data */
typedef unsigned short int  UINT16;   /* 16-bit unsigned integer */
typedef unsigned int        UINT32;   /* 32-bit unsigned integer */
typedef unsigned long long  UINT64;   /* 64-bit unsigned integer */
typedef UINT8               BOOLEAN;

typedef UINT32 SIZE_T;
typedef INT32  SSIZE_T;

//#endif /* _APP_CFG_H_ */

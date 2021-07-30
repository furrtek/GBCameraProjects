/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _FF_INTEGER
#define _FF_INTEGER

#include "LPC13xx.h"

#ifdef _WIN32	/* Development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */

typedef unsigned char	BOOL;

#define TRUE 1
#define FALSE 0

/* This type MUST be 8-bit */
typedef uint8_t			BYTE;

/* These types MUST be 16-bit */
typedef int16_t			SHORT;
typedef uint16_t		WORD;
typedef uint16_t		WCHAR;

/* These types MUST be 16-bit or 32-bit */
typedef int16_t			INT;
typedef uint16_t		UINT;

/* These types MUST be 32-bit */
typedef int32_t			LONG;
typedef uint32_t		DWORD;

#endif

#endif

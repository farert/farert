//
//  stdafx.h
//  CP
//
//  Created by TAKEDA, Noriyuki on 2014/05/26.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#ifndef CP_stdafx_h
#define CP_stdafx_h


#if defined _CONSOLE_TEST
#define ASSERT(cond) if (!(cond)) {  \
    char dummy; \
    fprintf(stderr, "\n\n\n!!!!!!!!!!!!!!Assertion error!!!!!!!!!!!!!!!!!!!!!!!%s:%d!!!!!!!!!!!!!!!!!! \n\n\n\nPress enter key to continue.", \
    __FILE__, __LINE__); \
    scanf("%c", &dummy); \
    fprintf(stderr, "\n\n"); }
#else
#define ASSERT(cond) if (!(cond)) { \
    fprintf(stderr, "\n\n\n!!!!!!!!!!!!!!Assertion error!!!!!!!!!!!!!!!!!!!!!!!%s:%d!!!!!!!!!!!!!!!!!! \n\n\n", \
    __FILE__, __LINE__); \
    for (;;); }
#endif

#if !defined _WINDOWS

#include <string.h>
#include <stdlib.h>

#define _ftprintf fprintf
#define _tcschr strchr
#define _tfopen fopen
#define _tcscpy strcpy
#define _tcsncmp strncmp

typedef char* LPTSTR;

#define TRACE printf
#define _T

#ifndef FALSE
#define FALSE   false
#endif
#ifndef TRUE
#define TRUE    true
#endif

typedef const char* LPCTSTR;
typedef char* LPSTR;
typedef char TCHAR;
typedef unsigned char  BYTE;

//#if !defined _DEBUG
//#define _DEBUG
//#endif

extern char* strcat_s(char* str, int len, const char* at);
extern char* strcpy_s(char* str, int len, const char* at);





#define _sntprintf_s snprintf

typedef unsigned char BYTE;

//extern void assertion_function(int cond);
//#define ASSERT  assertion_function  //NSAssert


#define _tcstok_s strtok_r

#define _tcscat_s strcat_s
#define _tcscpy_s strcpy_s
#define _tcslen   strlen
#define sprintf_s snprintf
#else	// _WINDOWS
//------------------------------------------------------------------- windows >>

#include <AtlBase.h>
#include <AtlConv.h>

//#include <tchar.h>
//#include <stdlib.h>
#include <time.h>
//#define TRACE _tprintf

extern void TRACE(const char* fmt, ...);
extern void TRACE(const wchar_t* fmt, ...);

//-------------------------------------------------------------------<< windows

#include <string>
typedef std::basic_string<TCHAR> tstring;


#if _MSC_VER <= 1600
#define u8
#endif

#endif // !_WINDOWS

#include "sqlite3.h"
#include "db.h"
#include "alpdb.h"

extern int g_tax;
#endif	// CP_stdafx_h

//
//  stdafx.h
//  CP
//
//  Created by TAKEDA, Noriyuki on 2014/05/26.
//  Copyright (c) 2014年 TAKEDA, Noriyuki. All rights reserved.
//

#ifndef CP_stdafx_h
#define CP_stdafx_h

#define _DEBUG

//#import <Foundation/Foundation.h>

#if 0
↑をいれると、@やらなんやら、Objective-Cではないためおこられる。
#endif

typedef const char* LPCTSTR;
typedef char* LPSTR;
typedef char TCHAR;
typedef unsigned char  BYTE;

#include <string.h>
#include <stdlib.h>

#if 0	//////
extern void logout(const char* fmt, ...);
#define TRACE logout
#else
#if 0
#define TRACE(c, ...) {}
#else
#define TRACE printf
#endif
#endif	/////


extern char* strcat_s(char* str, int len, const char* at);
extern char* strcpy_s(char* str, int len, const char* at);


#include <string>
typedef std::basic_string<TCHAR> tstring;



#define _sntprintf_s snprintf

#define _T
typedef unsigned char BYTE;

//extern void assertion_function(int cond);
//#define ASSERT  assertion_function  //NSAssert

#define ASSERT(cond) if (!(cond)) {  printf("\n\n\n!!!!!!!!!!!!!!Assertion error!!!!!!!!!!!!!!!!!!!!!!!%s:%d!!!!!!!!!!!!!!!!!! \n\n\n", __FILE__, __LINE__); }


#ifndef FALSE
#define FALSE   false
#endif
#ifndef TRUE
#define TRUE    true
#endif

#define _tcstok_s strtok_r

#define _tcscat_s strcat_s
#define _tcscpy_s strcpy_s
#define _tcslen   strlen
#define sprintf_s snprintf

#if _MSC_VER <= 1600
#define u8
#endif

#include "sqlite3.h"
#include "db.h"
#include "alpdb.h"

#endif

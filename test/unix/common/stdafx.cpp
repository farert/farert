#include "stdafx.h"

// ソース側で使用している TRACEマクロで、文字列に _T()をつけたりつけなか・ｽりしたので

#if defined _WINDOWS
void TRACE(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
}

void TRACE(const wchar_t* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vwprintf(fmt, ap);
}

#else

char* strcpy_s(char* dst, int32_t maxlen, const char* src)
{
    int32_t l;
    l = strlen(src);
    if (maxlen <= l) {
        l = maxlen - 1;
        strncpy(dst, src, l);
        dst[l] = (char)0;
    } else {
        strcpy(dst, src);
    }
    return dst;
}


char* strcat_s(char* dst, int32_t maxlen, const char* src)
{
    int32_t l;
    int32_t l2;
    l = strlen(src);
    l2 = strlen(dst);
    if (maxlen < l2) {
        ASSERT(FALSE);
        return dst;     /* error */
    }
    if (maxlen <= (l + l2)) {
        l = maxlen - l2 - 1;
        strncpy(dst, src, l);
        dst[l] = (char)0;
    } else {
        strcpy(dst + l2, src);
    }
    return dst;
}


//extern
void assertion_function(int32_t cond)
{
    if (!cond) {
        TRACE("\n\n\n!!!!!!!!!!!!!!Assertion error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n\n\n");
		exit(0);
        //for (;;);
    }
}

#endif	/* !_WINDOWS */

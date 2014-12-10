#include "stdafx.h"

// ソース側で使用している TRACEマクロで、文字列に _T()をつけたりつけなかったりしたので


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


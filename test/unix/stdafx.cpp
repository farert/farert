#include "stdafx.h"

// �\�[�X���Ŏg�p���Ă��� TRACE�}�N���ŁA������� _T()����������Ȃ������肵���̂�


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


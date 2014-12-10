#include "stdafx.h"

extern int test_exec(void);

int g_tax = 8;

int main(int argc, char** argv)
{
	_tsetlocale(LC_ALL, _T(""));	// tstring
	
	TRACE(_T("Hello\n"));
	TRACE(_T("В±Вс\n"));
	
	
	if (! DBS::getInstance()->open(_T("../../db/jr.db"))) {
		printf("Can't db open\n");
		return -1;
	}
	test_exec();
	return 0;
}


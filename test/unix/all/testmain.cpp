#include "stdafx.h"

extern int test_exec(void);

int g_tax = 8;

int main(int argc, char** argv)
{
#if defined _WINDOWS
	_tsetlocale(LC_ALL, _T(""));	// tstring
#endif
	if (! DBS::getInstance()->open(_T("../../db/jrdb2015.db"))) {
		printf("Can't db open\n");
		return -1;
	}
	test_exec();
	return 0;
}


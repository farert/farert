#include "stdafx.h"

extern int test_exec(void);
extern void test_route(const TCHAR *route_def[], bool round = true);
extern void test_autoroute(const TCHAR *route_def[]);

int g_tax = 8;

char tbuf[1024];
char tbuf2[64];
const TCHAR* tr[] = { tbuf,  _T("") };
const TCHAR* tr_a[] = { tbuf,  tbuf2, _T("") };
// _T("../../../db/jrdb2018.db")

int main(int argc, char** argv)
{
#if defined _WINDOWS
	_tsetlocale(LC_ALL, _T(""));	// tstring
#endif
	char* dbpath = getenv("farertDB");
	if (! DBS::getInstance()->open(dbpath)) {
		printf("Can't db open\n");
		return -1;
	}
	if (argc < 2) {
		test_exec();
	} else {
		char *t = tbuf;

		for (int i = 1; i < argc; i++) {
			if (((argc % 2) != 0) && (i == (argc - 1))) {
				strcpy(tbuf2, *++argv);
			} else {
				strcpy(t, *++argv);
				t = tbuf + strlen(tbuf);
				*t++ = _T(' ');
				*t = _T('\0');
			}
		}
		if ((argc % 2) == 0) {
			test_route(tr);
		} else {
			test_autoroute(tr);
		}
	}
	return 0;
}

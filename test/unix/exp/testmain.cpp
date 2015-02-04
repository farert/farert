#include "stdafx.h"

extern int test_exec(void);

int g_tax = 8;

TCHAR buffer[1024];
extern Route route;

static tstring cr_remove(tstring s)
{
	tstring::size_type idx = 0;
	while (tstring::npos != (idx = s.find(_T('\r'), idx))) {
		s.replace(idx, 1, _T(" "), 1, 1);
	}
	return s;
}

void setup(int argc, char**argv)
{
	int i;
	TCHAR* p = buffer;
	
	for (i = 1; i < argc; i++) {
		strcpy(p, *++argv);
		p += strlen(*argv);
		if ((i + 1) < argc) {
			*p++ = _T(' ');
			*p = _T('\0');
		}
	}
}

int main(int argc, char** argv)
{
	tstring s;
	int32_t rc;
#if defined _WINDOWS
	_tsetlocale(LC_ALL, _T(""));	// tstring
#endif
	if (! DBS::getInstance()->open(_T("../../db/jr.db"))) {
		printf("Can't db open\n");
		return -1;
	}
	setup(argc, argv);

	route.removeAll();
	rc = route.setup_route(buffer);
	ASSERT(0 <= rc);

	route.setFareOption(FAREOPT_RULE_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
	s = route.showFare();
	s = cr_remove(s);
	printf("%s\n", s.c_str());

	return 0;
}


#include "stdafx.h"

extern int test_exec(void);
extern tstring cr_remove(tstring s);

int g_tax = 8;

extern void test_route(const TCHAR *route_def[]);

const static TCHAR *test_tbl[] {
	//	_T("大阪 東海道線 草津 草津線 柘植 関西線 今宮 大阪環状線 鶴橋"),
	//	_T("大阪 東海道線 草津 草津線 柘植 関西線 新今宮 大阪環状線 鶴橋"),
		_T("大阪 東海道線 草津 草津線 柘植 関西線 天王寺 大阪環状線 鶴橋"),
		_T("弁天町 大阪環状線 京橋"),
		_T("大阪 大阪環状線 京橋"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 鶴橋"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 天満"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 大阪"),
	//ok_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 福島(環)"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 天王寺"),
	//ok	_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 西九条"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 今宮"),
		_T("今宮 大阪環状線 鶴橋"),
		_T("新今宮 大阪環状線 鶴橋"),
		_T("天王寺 大阪環状線 鶴橋"),
	_T(""),
};

TCHAR buffer[1024];
extern Route route;

static int setup(int argc, TCHAR**argv)
{
	int i;
	TCHAR* p = buffer;
	
	for (i = 1; i < argc; i++) {
		_tcscpy(p, *++argv);
		p += _tcslen(*argv);
		if ((i + 1) < argc) {
			*p++ = _T(' ');
			*p = _T('\0');
		}
	}
	return i - 1;
}


#ifdef _WINDOWS
int _tmain(int argc, TCHAR** argv)
#else
int main(int argc, char** argv)
#endif
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
	if (setup(argc, argv) < 1) {
		test_route(test_tbl);
	}

	route.removeAll();
	rc = route.setup_route(buffer);
	ASSERT(0 <= rc);

	route.setFareOption(FAREOPT_RULE_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
	s = route.showFare();
	s = cr_remove(s);
	TRACE(_T("%s\n"), s.c_str());

	return 0;
}


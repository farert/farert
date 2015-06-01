#include "stdafx.h"

extern int test_exec(void);
extern tstring cr_remove(tstring s);

int g_tax = 8;

extern void test_route(const TCHAR *route_def[]);

static const TCHAR *test_tbl[] = {
	_T("渋谷 山手線 恵比寿"),
	_T(""),
};

TCHAR buffer[1024];
extern Route route;

static int test_setup_route(Route& route, TCHAR* buffer)
{
	TCHAR* p;
	int lineId = 0;
	int stationId1 = 0;
	int stationId2 = 0;
	TCHAR* ctx = NULL;
	bool fail;
	int rc;

	for (p = _tcstok_s(buffer, _T(", \t"), &ctx); p; p = _tcstok_s(NULL, _T(", \t"), &ctx)) {
		fail = false;
		if (stationId1 == 0) {
			stationId1 = Route::GetStationId(p);
			ASSERT(0 < stationId1);
			route.add(stationId1);
		} else if (lineId == 0) {
			if ('r' == *p) {
				++p;
				route.setDetour();
			}
			lineId = Route::GetLineId(p);
			ASSERT(0 < lineId);
		} else {
			if ('x' == *p) {
				++p;
				fail = true;
			} else {
				fail = false;
			}
			stationId2 = Route::GetStationId(p);
			ASSERT(0 < stationId2);
			rc = route.add(lineId, /*stationId1,*/ stationId2);
			if (fail) {
				ASSERT(rc < 0);
				TRACE(_T("Setup route: Failure OK (%d)\n"), rc);
				return 1;
			} else {
				ASSERT(0 <= rc);
			}
			lineId = 0;
			stationId1 = stationId2;
		}
	}
	return 0;
}

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
	if (! DBS::getInstance()->open(_T("../../db/jrdb2015.db"))) {
		printf("Can't db open\n");
		return -1;
	}
	if (setup(argc, argv) < 1) {
		test_route(test_tbl);
	} else {
		route.removeAll();
		rc = test_setup_route(route, buffer);
		ASSERT(0 <= rc);
		route.setFareOption(FAREOPT_RULE_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
		s = route.showFare();
		s = cr_remove(s);
		TRACE(_T("%s\n"), s.c_str());
	}
	return 0;
}


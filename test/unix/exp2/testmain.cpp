#include "stdafx.h"

int g_tax = 8;
extern Route route;

static const char *test_tbl[] = {
	_T("東北新幹線 上野 盛岡"),
	_T("東北新幹線 上野 仙台"),
	_T("東北新幹線 上野 白石蔵王"),
	_T(""),
};


void test(void)
{
	int zline_id;
	int line_id;
	int station_id1;
	int station_id2;
	int i;
	
	for (i = 0; *test_tbl[i] != '\0'; i++) {
		char* p;
		TCHAR* ctx = NULL;

		printf("%s", test_tbl[i]);
		p = _tcstok_s((char*)test_tbl[i], _T(", \t"), &ctx);

		line_id = Route::GetLineId(p);
		p = _tcstok_s(NULL, _T(", \t"), &ctx);

		station_id1 = Route::GetStationId(p);
		p = _tcstok_s(NULL, _T(", \t"), &ctx);

		station_id2 = Route::GetStationId(p);

		zline_id = Route::GetHZLine(line_id, station_id1, station_id2);
		if ((0 < zline_id) && (zline_id < 32767)) {
			printf("OK: %s\n", Route::LineName(zline_id).c_str());
		} else {
			printf("Error\n");
		}
	}
}


#ifdef _WINDOWS
int _tmain(int argc, TCHAR** argv)
#else
int main(int argc, char** argv)
#endif
{
#if defined _WINDOWS
	_tsetlocale(LC_ALL, _T(""));	// tstring
#endif
	if (! DBS::getInstance()->open(_T("../../db/jrdb2015.db"))) {
		printf("Can't db open\n");
		return -1;
	}
	printf("----------");
	test();
	return 0;
}


#include "stdafx.h"

extern int test_exec(void);
extern tstring cr_remove(tstring s);

int g_tax = 8;

extern void test_route(const TCHAR *route_def[]);

static const TCHAR *test_tbl[] = {
		_T("本郷台 根岸線 横浜 東海道線 東神奈川 横浜線 新横浜 東海道新幹線 東京 東北線 高久"),
		_T("本郷台 根岸線 横浜 東海道線 東神奈川 横浜線 新横浜 東海道新幹線 東京 東北線 黒田原"),
		_T("大阪 東海道線 草津 草津線 柘植 関西線 今宮 大阪環状線 x鶴橋"),
		_T("大阪 東海道線 草津 草津線 柘植 関西線 新今宮 大阪環状線 x鶴橋"),
		_T("大阪 東海道線 草津 草津線 柘植 関西線 天王寺 大阪環状線 鶴橋"),
		_T("弁天町 大阪環状線 京橋"),
		_T("大阪 大阪環状線 京橋"),
		_T("今宮 大阪環状線 鶴橋"),
		_T("新今宮 大阪環状線 鶴橋"),
		_T("天王寺 大阪環状線 鶴橋"),
		_T("芦原橋 大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 野田"),
		_T("芦原橋 大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 寺田町"),
		_T("芦原橋 大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 天満"),
		_T("福島(環) 大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 x野田"),
		_T("福島(環) r大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 野田"),       // pass!!!
		_T("桃谷 大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 x寺田町"),
		_T("桃谷 r大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 寺田町"),
		_T("桃谷 大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 天王寺"),
		_T("西九条 大阪環状線 大阪"),
		_T("天王寺 r大阪環状線 大阪"),
		_T("大正 r大阪環状線 寺田町"),
		_T("芦原橋 r大阪環状線 天王寺"),
		_T("大正 大阪環状線 寺田町"),
		_T("桃谷 大阪環状線 天王寺 関西線 木津 奈良線 京都 東海道線 大阪 大阪環状線 天王寺"),
		_T("西九条 r大阪環状線 大阪"),
		_T("大阪 大阪環状線 西九条"),
		_T("天王寺 大阪環状線 大阪"),
		_T("寺田町 r大阪環状線 大正"),
		_T("寺田町 大阪環状線 大正"),
		_T("天王寺 大阪環状線 今宮 大阪環状線 x天王寺 大阪環状線 西九条"),
		_T("天王寺 大阪環状線 大阪"),
		_T("大阪 大阪環状線 西九条"),
		_T("天王寺 r大阪環状線 大阪"),
// 大阪out 京橋in
		_T("野田 大阪環状線 大阪 東海道線 尼崎 JR東西線 京橋 大阪環状線 x福島(環)"),
		_T("野田 大阪環状線 大阪 東海道線 尼崎 JR東西線 京橋 大阪環状線 野田"),
		_T("芦原橋 大阪環状線 大阪 東海道線 尼崎 JR東西線 京橋 大阪環状線 x福島(環)"),
		_T("芦原橋 r大阪環状線 大阪 東海道線 尼崎 JR東西線 京橋 大阪環状線 福島(環)"),				// pass!!
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 鶴橋"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 天満"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 大阪"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 x福島(環)"),
		_T("大正 r大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 福島(環)"),	// pass!!
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 天王寺"),
		_T("大正 r大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 天王寺"),
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 x西九条"),
		_T("大正 r大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 西九条"),   // pass!!
		_T("大正 大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 今宮"),
		_T("大正 r大阪環状線 大阪 東海道線 草津 草津線 柘植 関西線 木津 片町線 京橋 大阪環状線 今宮"),
// 大阪(かすめる） 京橋in
		_T("神戸 東海道線 京都 奈良線 木津 片町線 京橋 大阪環状線 大阪"),
		_T("神戸 東海道線 京都 奈良線 木津 片町線 京橋 大阪環状線 野田"),
		_T("神戸 東海道線 京都 奈良線 木津 片町線 京橋 大阪環状線 天王寺"),
		_T("神戸 東海道線 京都 奈良線 木津 片町線 京橋 大阪環状線 西九条"),
// 大阪out 天王寺in
// 大阪(かすめる） 天王寺in
// 京橋out 大阪in
// 京橋out 天王寺in
// 京橋in 大阪out
// 京橋in 天王寺out
// 天王寺out 京橋in
		_T("大阪 r大阪環状線 天王寺 関西線 久宝寺 おおさか東線 放出 片町線 京橋 大阪環状線 鶴橋"),
		_T("大阪 r大阪環状線 天王寺  関西線 久宝寺 おおさか東線 放出 片町線 京橋 大阪環状線 天満"),
		_T("芦原橋 大阪環状線 天王寺  関西線 久宝寺 おおさか東線 放出 片町線 京橋 大阪環状線 大正"),
// 天王寺out 大阪in
// 天王寺in 京橋out
// 天王寺in 京橋in
		_T("天王寺 大阪環状線 京橋 片町線 放出 おおさか東線 久宝寺 関西線 天王寺 大阪環状線 大阪"),
// 天王寺in 大阪out
// 天王寺かすめる 大阪in
// 天王寺かすめる 京橋in  (JR難波発)
// 天王寺かすめる 大阪out
// 天王寺かすめる 京橋out  (JR難波発)




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
	if (! DBS::getInstance()->open(_T("../../db/jr.db"))) {
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


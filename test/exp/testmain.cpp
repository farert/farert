#include "stdafx.h"

static tstring cr_remove(tstring s);


int g_tax = 8;

Route route;

void test(void)
{
	int32_t rc;
	tstring s;
	LPCTSTR rt;
	
	rt = _T("京都 東海道線 山科 湖西線 近江塩津 北陸線 富山 高山線 岐阜 東海道線 山科");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");

	rt = _T("南浦和 武蔵野線 府中本町 南武線 川崎 東海道線 山科");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");

	rt = _T("山科 東海道線 岐阜 高山線 富山 北陸線 近江塩津 湖西線 山科");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");

	rt = _T("京都 東海道線 山科 湖西線 近江塩津 北陸線 富山 高山線 岐阜 東海道線 名古屋 関西線 天王寺 大阪環状線 大阪 東海道線 京都");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");
	
	rt = _T("京都 東海道線 山科 湖西線 近江塩津 北陸線 富山 高山線 岐阜 東海道線 保土ケ谷");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");

	rt = _T("山科 湖西線 近江塩津 北陸線 富山 高山線 岐阜 東海道線 山科");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");

	rt = _T("京都 東海道線 名古屋 関西線 木津 奈良線 京都");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");

	rt = _T("京都 東海道線 名古屋 関西線 天王寺 大阪環状線 大阪 東海道線 京都");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");

	rt = _T("仙台 東北線 福島(北) 奥羽線 横手 北上線 北上 東北線 仙台");
	rc = route.setup_route(rt);
	TRACE("\n\nsetup()=%d\n", rc);
	s = route.showFare(RULE_APPLIED);
	s = cr_remove(s);
	TRACE(_T("%s\n"), rt);
	TRACE(_T("%s\n"), s.c_str());
	TRACE("----------------------------------------------------------------\n");
}

int main(int argc, char** argv)
{
#if defined _WINDOWS
	_tsetlocale(LC_ALL, _T(""));	// tstring
#endif
	if (! DBS::getInstance()->open(_T("../../db/jr.db"))) {
		TRACE("Can't db open\n");
		return -1;
	}

	test();

	return 0;
}

static tstring cr_remove(tstring s)
{
	tstring::size_type idx = 0;
	while (tstring::npos != (idx = s.find(_T('\r'), idx))) {
		s.replace(idx, 1, _T(" "), 1, 1);
	}
	return s;
}

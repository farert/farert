#include "stdafx.h"

int g_tax = 8;
extern Route route;

static const TCHAR *test_tbl[] = {
	_T("上野 東北新幹線 盛岡"),
	_T(""),
};


static tstring cr_remove(tstring s)
{
	tstring::size_type idx = 0;
	while (tstring::npos != (idx = s.find(_T('\r'), idx))) {
		s.replace(idx, 1, _T(" "), 1, 1);
	}
	return s;
}

/////////////////////////////////////////////////////////////////
void test(void)
{
	int32_t rc;
	int32_t i;
	tstring s;
	
	for (i = 0; '\0' != *test_tbl[i]; i++) {
		rc = route.setup_route(test_tbl[i]);
		if ((rc != 0) && (rc != 1)) {
			TRACE("Error!!!!(%d) test_tbl[%d](%s)\n", rc, i, test_tbl[i]);
			return;
		}
		CalcRoute croute(route);
		s = croute.showFare();
		s = cr_remove(s);
		TRACE(_T("%s\n"), s.c_str());

		const vector<RouteItem>& r = route.routeList();
		CalcRoute::ConvertShinkansen2ZairaiFor114Judge(&r);
		s = RouteUtil::Show_route(r, 0);
		s = cr_remove(s);
		TRACE(_T("%s\n-------------\n"), s.c_str());
	}
}

////////////////////////////////////////////////////////////////


#ifdef _WINDOWS
int _tmain(int argc, TCHAR** argv)
#else
int main(int argc, char** argv)
#endif
{
#if defined _WINDOWS
	_tsetlocale(LC_ALL, _T(""));	// tstring
#endif
	if (! DBS::getInstance()->open(_T("../../../db/jrdb2015.db"))) {
		printf("Can't db open\n");
		return -1;
	}
	printf("----------");
	test();
	return 0;
}


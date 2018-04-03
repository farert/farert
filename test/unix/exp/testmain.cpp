#include "stdafx.h"

extern int test_exec(void);
extern tstring cr_remove(tstring s);
extern int test_setup_route(TCHAR* buffer, Route& route);

int g_tax = 8;

extern void test_route(const TCHAR *route_def[], bool rev = true);

static const TCHAR *test_tbl[] = {
#if 0
	_T("渋谷 山手線 恵比寿"),
	_T("小倉 山陽新幹線 博多"),
	_T("博多南 博多南線 博多"),
	_T("博多 博多南線 博多南"),
	_T("恵那,中央西線,名古屋,東海道新幹線,新横浜"),
	_T(              "品川 東海道線 小田原"),	/***/
	_T(              "品川 東海道新幹線 小田原"),	/***/
	_T("東京 東海道線 品川 東海道新幹線 小田原"),	/***/
	_T("東京 東海道線 品川 東海道新幹線 名古屋"),	/***/
	_T("              品川 東海道線 名古屋"),	/***/
	_T("              品川 東海道新幹線 名古屋"),	/***/
	_T("品川 東海道新幹線 京都"),	/***/
	_T("品川 東海道線 京都"),	/***/
	_T("品川 東海道新幹線 米原"),	/***/
	_T("品川 東海道線 米原"),	/***/
	_T("品川 東海道新幹線 静岡"),	/***/
	_T("品川 東海道線 静岡"),	/***/
	_T("品川 東海道新幹線 静岡 東海道線 掛川"),	/***/
	_T("品川 東海道新幹線 名古屋 関西線 亀山"),	/***/
	_T("国府津 御殿場線 沼津"),	/***/
	_T("国府津 御殿場線 沼津 東海道線 熱海"),	/***/
	_T("国府津 御殿場線 沼津 東海道線 熱海 伊東線 伊東"),	/***/
//	_T("函館 函館線 五稜郭 江差線 木古内 海峡線 中小国 津軽線 青森"),
	_T("神田 東北線 東京 東海道新幹線 三島 東海道線 富士 身延線 甲府"),
	_T("東京 東海道線 品川 東海道新幹線 名古屋"),
	_T("東京 東海道新幹線 三島 東海道線 富士 身延線 甲府"),
#endif
	_T("品川 東海道新幹線 小田原"),
	_T("品川 東海道新幹線 名古屋"),
	_T("品川 東海道新幹線 熱海"),
	_T("品川 東海道新幹線 掛川"),
	_T("品川 東海道新幹線 米原"),
	_T("品川 東海道新幹線 京都"),
	_T("掛川 東海道新幹線 米原"),
	_T("岐阜羽島 東海道新幹線 新大阪"),
	_T("新大阪 東海道新幹線 熱海"),
	_T("新宿 山手線 品川 東海道新幹線 小田原"),
	_T("新宿 山手線 品川 東海道新幹線 名古屋"),
	_T("新宿 山手線 品川 東海道新幹線 熱海"),
	_T("新宿 山手線 品川 東海道新幹線 掛川"),
	_T("新宿 山手線 品川 東海道新幹線 米原"),
	_T("新宿 山手線 品川 東海道新幹線 京都"),
	_T("新大阪 東海道新幹線 米原 北陸線 福井"),
	_T("新大阪 東海道新幹線 米原"),
	_T("品川 東海道新幹線 小田原 東海道線 早川"),
	_T("品川 東海道新幹線 静岡 東海道線 草薙"),
	_T("品川 東海道新幹線 名古屋 関西線 四日市"),
	_T("熱海 東海道新幹線 静岡 東海道線 草薙"),
	_T("熱海 東海道新幹線 米原"),
	_T("三島 東海道新幹線 名古屋 関西線 四日市"),
	_T("国府津 御殿場線 沼津"),
	_T("国府津 東海道線 小田原"),
	_T("小田原 東海道新幹線 熱海"),
	_T("品川 東海道新幹線 名古屋"),
	_T("大高 東海道線 東神奈川"),
	_T("大高 東海道線 神戸"),
	_T("品川 東海道新幹線 名古屋 関西線 四日市 "),
	_T("桑園 函館線 岩見沢 室蘭線 長万部"),
	_T("桑園 函館線 岩見沢"),
	_T("新谷 内子線 内子"),
	_T("富士 身延線 富士宮"),
	_T("岐阜羽島 東海道新幹線 米原"),
	_T("大崎 山手線 品川 東海道新幹線 熱海"),
	_T(""),
	_T(""),
};

TCHAR buffer[1024];
Route route;


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
	setvbuf(stdout, 0, _IONBF, 0);

	if (! DBS::getInstance()->open(_T("../../../db/jrdb2017.db"))) {
		printf("Can't db open\n");
		return -1;
	}
	if (setup(argc, argv) < 1) {
		test_route(test_tbl, false);
	} else {
		route.removeAll();
		rc = test_setup_route(buffer, route);
		ASSERT(0 <= rc);
		CalcRoute croute(route);
		croute.setFareOption(FAREOPT_RULE_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
		s = croute.showFare();
		s = cr_remove(s);
		TRACE(_T("%s\n"), s.c_str());
	}
	return 0;
}

#include "stdafx.h"

#define DBPATH "../../../db/jrdb2015.db"

int g_tax = 8;
extern Route route;

static tstring cr_remove(tstring s)
{
	tstring::size_type idx = 0;
	while (tstring::npos != (idx = s.find(_T('\r'), idx))) {
		s.replace(idx, 1, _T(" "), 1, 1);
	}
	return s;
}


void test_autoroute(void)
{
	const static TCHAR *route_def[] = {
/* */     _T("品川"),				_T("大井町"),
/*  */    _T("代々木"),				_T("原宿"),
/*  */    _T("渋谷"),				_T("原宿"),
/*  */    _T("代々木"),				_T("品川"),
/*  */    _T("渋谷"),				_T("品川"),
/*  */    _T("品川"),				_T("渋谷"),
/*  */    _T("品川"),				_T("代々木"),
/*  */    _T("渋谷"),				_T("大森"),
          _T("渋谷,山手線,品川"),		_T("東京"),
/*  */    _T("渋谷 山手線 品川"),		_T("有楽町"),
/*  */    _T("渋谷 山手線 品川"),		_T("大森"),
/*  */    _T("渋谷 山手線 品川"),		_T("川崎"),
/*  */    _T("渋谷 山手線 品川"),		_T("渋谷"),			//s
/*!!*/    _T("渋谷 山手線 品川"),		_T("代々木"),		//s  
/*!!*/    _T("渋谷 山手線 品川"),		_T("新宿"),			//   
/*!!*/    _T("渋谷 山手線 品川"),		_T("高田馬場"),
          _T("代々木,山手線,品川"),	_T("東京"),
/*  */    _T("代々木 山手線 品川"),	_T("有楽町"),
/*  */    _T("代々木 山手線 品川"),	_T("大森"),
/*  */    _T("代々木 山手線 品川"),	_T("川崎"),
/*  */    _T("代々木 山手線 品川"),	_T("x渋谷"),
/*!!*/    _T("代々木 山手線 品川"),	_T("代々木"),
/*  */    _T("代々木 山手線 品川"),	_T("新宿"),
/*!!*/    _T("渋谷 山手線 恵比寿"),	_T("高田馬場"),  //恵比寿、五反田は非分岐なので削除してから検索される(仕様とした)
		  _T("渋谷 山手線 五反田"),  	_T("大崎"),
		  _T("渋谷 山手線 五反田"),   _T("品川"),
		  _T("渋谷 山手線 五反田"),   _T("恵比寿"),
		  _T("渋谷 山手線 五反田"),   	_T("原宿"),
		  _T("渋谷 山手線 五反田"),   _T("渋谷"), 
		  _T("渋谷 山手線 五反田"),   _T("代々木"),
		  _T("代々木 山手線 五反田"), _T("大崎"),
		  _T("代々木 山手線 五反田"), _T("品川"), 
		  _T("代々木 山手線 五反田"), _T("恵比寿"),
		  _T("代々木 山手線 五反田"), _T("原宿"),
		  _T("代々木 山手線 五反田"), _T("代々木"),

/*  */    _T("新芝浦"),_T("海芝浦"),
/*  */    _T("海芝浦"),_T("安善"),
/*  */    _T("海芝浦"),_T("国道"),
/*  */    _T("海芝浦"),_T("鶴見"),
/*  */    _T("海芝浦"), _T("新芝浦"),
/*  */    _T("安善"), _T("海芝浦"),
/*  */    _T("国道"), _T("海芝浦"),
/*  */    _T("鶴見"), _T("海芝浦"),
/* 01 */    _T("函館"), _T("青森"),
/* 01 */    _T("岡山"), _T("松山"),
/* 01 */    _T("広島"), _T("博多"),
/* 01 */    _T("広島"), _T("長崎"),

/* 01 */    _T("青森"), _T("函館"), 
/* 01 */    _T("松山"), _T("岡山"), 
/* 01 */    _T("博多"), _T("広島"), 
/* 01 */    _T("長崎"), _T("広島"), 
            _T("品川,東海道新幹線,名古屋,中央西線,塩尻"), _T("草薙"),
            _T("品川,東海道新幹線,名古屋"), _T("草薙"),

/* 00 */    _T("東京,東海道新幹線,静岡,東海道線,浜松"), _T("名古屋"),
/* 01 */    _T("富山"), _T("天竜川"),
/* 02 */    _T("天竜川"), _T("富山"),
/* 03 */    _T("仙台"), _T("大森"),
/* 04 */    _T("根室"), _T("鹿児島"),
/* 05 */    _T("長崎"), _T("稚内"),
/* 06 */    _T("茂市"), _T("長野"),
/* 07 */    _T("茂市"), _T("熱海"),
/* 08 */    _T("宮古"), _T("坂町"),
/* 09 */    _T("坂町"), _T("宮古"),
/* 10 */    _T("代々木"), _T("用土"),
/* 11 */    _T("二本松,東北線,福島(北),奥羽線,大曲,田沢湖線,盛岡,東北線,小牛田"), _T("x宮古"),
/* 12 */    _T("二本松,東北線,福島(北),奥羽線,大曲,田沢湖線,盛岡,東北線,小牛田"), _T("坂町"),
/* 13 */    _T("二本松,東北線,福島(北),奥羽線,横手,北上線,北上,東北線,小牛田"), _T("坂町"),
/* 14 */    _T("東京,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線"), _T("x御殿場"),
/* 15 */    _T("東京,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線"), _T("x函南"),
/* 16 */    _T("小田原,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線"), _T("x早川"),
/* 17 */    _T("東京,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線"), _T("x草薙"),
/* 18 */    _T("東京,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線"), _T("x焼津"),
/* 19 */    _T("東京,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線,富士"), _T("x御殿場"),
/* 20 */    _T("東京,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線,富士"), _T("x函南"),
/* 21 */    _T("小田原,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線,富士"), _T("x早川"),
/* 22 */    _T("東京,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線,富士"), _T("x草薙"),
/* 23 */    _T("東京,東海道線,金山(中),中央西線,塩尻,中央東線,甲府,身延線,富士"), _T("x焼津"),
/* 24 */    _T("田端,山手線"), _T("品川"),
/* 25 */    _T("田端"),		_T("品川"),
/* 26 */    _T("鶴橋"),		_T("弁天町"),
/* 27 */    _T("天王寺"),	_T("新今宮"),
/* 28 */    _T("新今宮"),	_T("天王寺"),
/* 29 */    _T("綾瀬"),		_T("東川口"),
/* 30 */    _T("大崎"),		_T("田端"),
/* 31 */    _T("寒河江"),	_T("赤湯"),
/* 33 */    _T("北新地"),	_T("大阪"),
/* 34 */    _T("小海"),		_T("上田"),
/* 35 */    _T("柘植"),		_T("京橋"),
/* 36 */    _T("大阪"),		_T("鶴橋"),
/* 37 */    _T("大阪"),		_T("大正"),
/* 38 */    _T("大阪"),		_T("桜島"),
			_T("逗子 横須賀線 大船"), _T("磯子"),
			_T("逗子 横須賀線 大船"), _T("横浜"),
			_T("逗子 横須賀線 大船"), _T("茅ケ崎"),
			_T("逗子 横須賀線 大船"), _T("藤沢"),
			_T("横浜 根岸線 大船"), _T("衣笠"),
			_T("保土ケ谷 東海道線 大船"), _T("衣笠"),
//			_T(""), _T(""),
//			_T(""), _T(""),

/* 39 */    _T(""),
    };
	TCHAR buffer[1024];
	int i;
	int rc;
	for (i = 0; _T('\0') != *route_def[i]; i += 2) {
		bool b_fail;
		LPCTSTR p;
		
		TRACE(_T("!===<%02d>: auto route ==================\n\n"), i / 2);
		TRACE("test_exec(auto route): %d*************************************************\n", i / 2);
		route.removeAll();
		strcpy(buffer, route_def[i]);
		rc = route.setup_route(buffer);
		ASSERT(0 <= rc);

		b_fail = (route_def[i + 1][0] == 'x');
		p = route_def[i + 1];
		if (b_fail) {
			++p;
		}
		route.setEndStationId(Route::GetStationId(p));
		TRACE(_T("* pre route >>>>>>>\n  {%s -> %s}\n"), route_def[i], p);

		TRACE(_T("* auto route(新幹線未使用) >>>>>>>\n"));
		if (route.changeNeerest(false) < 0) {
printf("! ! ! changeNeerest E r r o r ! ! !\n");
			TRACE(_T("Can't route.%s\n"), b_fail ? _T("(OK)") : _T("(NG)"));
			ASSERT(b_fail == true);
		} else {
printf("o o o changeNeerest S u c c e s s   o o o\n");
			ASSERT(b_fail == false);
			route.setFareOption(FAREOPT_RULE_NO_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
			tstring s = route.showFare();
			s = cr_remove(s);
			TRACE(_T("///非適用\n%s\n"), s.c_str());
#if 1
			route.setFareOption(FAREOPT_RULE_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
			s = route.showFare();
			s = cr_remove(s);
			TRACE(_T("///適用\n%s\n"), s.c_str());
#endif
		}
#if 1
		route.removeAll();
		strcpy(buffer, route_def[i]);
		rc = route.setup_route(buffer);
		ASSERT(0 <= rc);
		route.setEndStationId(Route::GetStationId(p));
		TRACE(_T("* auto route(新幹線使用) >>>>>>>\n"));
		if (route.changeNeerest(true) < 0) {
			TRACE(_T("Can't route.%s\n"), b_fail ? _T("(OK)") : _T("(NG)"));
			ASSERT(b_fail == true);
		} else {
			ASSERT(b_fail == false);
			route.setFareOption(FAREOPT_RULE_NO_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
			tstring s = route.showFare();
			s = cr_remove(s);
			TRACE(_T("///非適用\n%s\n"), s.c_str());

			route.setFareOption(FAREOPT_RULE_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
			s = route.showFare();
			s = cr_remove(s);
			TRACE(_T("///適用\n%s\n"), s.c_str());
		}
#endif
	}

}



#if 0
a 渋谷 山手線 品川 蒲田
a 渋谷 山手線 品川 東北線 川崎 蒲田
b 渋谷 蒲田
#endif
int main(int argc, char** argv)
{
	int i;
	int s = 0;
	int e = 0;
	int l = 0;
	int rc;
	int bullet = 0;
	
	if (! DBS::getInstance()->open(DBPATH)) {
		printf("Can't db open\n");
		return -1;
	}
	
	if (2 <= argc) {
		for (i = 1; i < argc; i++) {
			++argv;
			if ((i % 2) != 0) {
				s = Route::GetStationId(*argv);
			} else {
				if ('-' == **argv) {
					e = Route::GetStationId(*++argv);
					bullet = 0;
					break;	//>>>>>>>>>>>>>>>>>>>>>>
				} else if ('+' == **argv) {
					e = Route::GetStationId(*++argv);
					bullet = 1;
					break;	//>>>>>>>>>>>>>>>>>>>>>>
				} else {
					l = Route::GetLineId(*argv);
				}
			}
			if (i == 1) { // first
			    if (s <= 0) {
					fprintf(stderr, "parameter error: %d\n", s);
					return -1;
			    }
				rc = route.add(s);
			} else if ((i % 2) != 0) {
			    if ((l <= 0) || (s <= 0)) {
					fprintf(stderr, "parameter error: %d-%d\n", l, s);
					return -1;
			    }
				rc = route.add(l, s);
			} else {
				rc = 1;
			}
			if (rc <= 0) {
				fprintf(stderr, "error token %d: %d\n", i, rc);
				return -1;
			}
		}
		if (e <= 0) {
			fprintf(stderr, "argument error: 'non '-' parameter'\n");
			return -1;
		}
		route.setEndStationId(e);
		rc = route.changeNeerest(bullet);
		if (rc < 0) {
			fprintf(stderr, "**auto route Error. %d\n", rc);
			return -1;
		} else {
			fprintf(stderr, "##################\n");
		}
		tstring s = route.showFare();
		printf("%s\n", s.c_str());
	} else {
		test_autoroute();
	}
	return 0;
}


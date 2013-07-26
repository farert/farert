
#include "stdafx.h"


#if defined _DEBUG

FILE *os;
Route route;


/////////////////////////////////////////////////////////////////////////////////////
static void setup_route(TCHAR* route_str)
{
	TCHAR* p;
	int lineId = 0;
	int stationId1 = 0;
	int stationId2 = 0;
	TCHAR* ctx = NULL;

	for (p = _tcstok_s(route_str, _T(", "), &ctx); p; p = _tcstok_s(NULL, _T(", "), &ctx)) {
		if (stationId1 == 0) {
			route.startStationId = stationId1 = Route::GetStationId(p);
			ASSERT(0 < stationId1);
		} else if (lineId == 0) {
			lineId = Route::GetLineId(p);
			ASSERT(0 < lineId);
		} else {
			stationId2 = Route::GetStationId(p);
			ASSERT(stationId2);
			VERIFY(0 <= route.add(lineId, stationId1, stationId2));
			lineId = 0;
			stationId1 = stationId2;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
static void show_route()
{
	CString lineName;
	int lineId;
	CString stationName;
	int stationId;

	stationName = Route::StationName(route.startStationId);
	_ftprintf(os, _T("start:  %s\n"), stationName);

	vector<RouteItem>::const_iterator pos = route.routeList().cbegin();

	ASSERT((pos->lineId == 0) && (pos->stationId == route.startStationId));
	pos++;
	for (; pos != route.routeList().cend() ; pos++) {

		stationId = pos->stationId;
		if (stationId != route.endStationId) {
			stationName = Route::StationName(stationId);
		}
		lineId = pos->lineId;
		lineName = route.LineName(lineId);

		_ftprintf(os, _T("%s, %s\n"), lineName, stationName);
	}
}

/////////////////////////////////////////////////////////////////////////////////////

static void test_a69(void)
{
	TCHAR *route_def[] = {
	 _T("‰¡•l,“ŒŠC“¹ü,“Œ‹ž,‹ž—tü,‘h‰ä,ŠO–[ü,‘å–Ô,“Œ‹àü,¬“Œ,‘•ü,²‘q,¬“cü,‰ä‘·Žq,í”Öü,ŠâÀ"),
	 _T("‰¡•l,“ŒŠC“¹ü,“Œ‹ž,‹ž—tü,‘h‰ä"),
	 _T("‰¡•l,“ŒŠC“¹ü,“Œ‹ž,‹ž—tü,‘h‰ä,ŠO–[ü, ç—t, ¬“cü, ¬“c"),
	 _T("‰¡•l,“ŒŠC“¹ü,“Œ‹ž,‹ž—tü,‘h‰ä,ŠO–[ü, ç—t, ‘•ü, ‘D‹´"),
	 _T("‘åã,“ŒŠC“¹ü,–¼ŒÃ‰®"),
	 _T("”ŸŠÙ, ”ŸŠÙü, ‘åÀ, ”ŸŠÙü(»Œ´ü), X, ”ŸŠÙü, ˆ®ì, •x—Ç–ìü, •x—Ç–ì"),
	 _T("–Ô‘–, Î–kü, Vˆ®ì, @’Jü, ˆ®ì, ”ŸŠÙü, X, ”ŸŠÙü(»Œ´ü), ‘åÀ, ”ŸŠÙü, ŒÜ—ÅŠs"),
	 _T("‰F“s‹{,“Œ–kü,‘å‹{,“Œ–kü(é‹žü),Ô‰H,“Œ–kü(”ö‹vŒo—R),“ú•é—¢,í”Öü,…ŒË,…ŒSü,Š}ŠÔ"),
	 _T("‰F“s‹{,“Œ–kü,‘å‹{,“Œ–kü(é‹žü),Ô‰H,“Œ–kü(”ö‹vŒo—R),“ú•é—¢,“Œ–kü,“Œ‹ž,‹ž—tü,‘h‰ä,ŠO–[ü,–ÎŒ´"),
	 _T("‰F“s‹{,“Œ–kü,‘å‹{,“Œ–kü(é‹žü),•‘ ‰Y˜a,•‘ –ìü,V¼ŒË,í”Öü,‘o—t"),
	 _T("‰F“s‹{,“Œ–kü,‘å‹{,“Œ–kü(é‹žü),•‘ ‰Y˜a,•‘ –ìü,•{’†–{’¬,“ì•ü,ìè"),
	 _T("‰F“s‹{,“Œ–kü,‘å‹{,“Œ–kü(é‹žü),Ô‰H,“Œ–kü,“ì‰Y˜a,•‘ –ìü,•‘ ‰Y˜a"),
	 _T("ã–ì,“Œ–kü,Ô‰H,“Œ–kü(é‹žü),‘å‹{,ì‰zü,‚—íì,”ª‚ü,”q“‡"),
	 _T("ã–ì,“Œ–kü,Ô‰H,“Œ–kü(é‹žü),‘å‹{,“Œ–kü,–kã,–kãü,‚Ù‚Á‚Æ‚ä‚¾"),
	 _T("“Œ‹ž,“ŒŠC“¹ü,•iì,“ŒŠC“¹ü(¼‘åˆäŒo—R),’ßŒ©,“ŒŠC“¹ü,‰¡•l"),
	 _T("“Œ‹ž,‹ž—tü,‘h‰ä,ŠO–[ü,Ÿ‰Y"),
	 _T("‘åã,“ŒŠC“¹ü,•ÄŒ´,–k—¤ü,•Ÿˆä"),
	 _T("”ö“¹,ŽR—zü,ŽOŒ´,Œàü,ŠC“cŽs,ŽR—zü,L“‡"),
	 _T("L“‡,ŽR—zü,VŽRŒû"),
	 _T("_ŒË,ŽR—zü,ŽOŒ´,Œàü,ŠC“cŽs,ŽR—zü,‰ºŠÖ"),
	 _T("ŽOŒ´,Œàü,ŠC“cŽs,ŽR—zü,‰ºŠÖ"),
	 _T("ŽOŒ´,Œàü,ŠC“cŽs"),
	 _T("_ŒË,ŽR—zü,ŽOŒ´,Œàü,ŠC“cŽs"),
	 _T("‰ºŠÖ,ŽR—zü,_ŒË"),
	 _T("‹ùƒ–•l,ŽR—zü,_ŒË"),
	 _T("‰ºŠÖ,ŽR—zü,‹ùƒ–•l"),
	 _T("‹ùƒ–•l,ŽR—zü,Šâ‘"),
	 _T("‰ºŠÖ,ŽR—zü,Šâ‘"),
	 _T("_ŒË,ŽR—zü,‰ºŠÖ"),
	 _T("‘h‰ä,‹ž—tü,“Œ‹ž"),
	 _T("Ÿ‰Y,ŠO–[ü,‘h‰ä,‹ž—tü,“Œ‹ž"),
	 _T("“Œ‹ž,‹ž—tü,‘h‰ä,ŠO–[ü,Ÿ‰Y"),
	 _T("“Œ‹ž,‹ž—tü,‘h‰ä,“à–[ü,‘³ƒ–‰Y"),
	 _T("“Œ‹ž,‹ž—tü,‘h‰ä"),
	 _T("“Œ‹ž,‘•ü,ç—t, ŠO–[ü,Ÿ‰Y"),
	 _T("Ÿ‰Y,ŠO–[ü,ç—t, ‘•ü,“Œ‹ž"),
	 _T("‹ÑŽ…’¬,‘•ü(‹ÑŽ…’¬-Œä’ƒƒm…),Œä’ƒƒm…,’†‰›“Œü, “Œ‹ž, ‹ž—tü, ç—t, ŠO–[ü,Ÿ‰Y"),
	 _T("—¼‘,‘•ü(‹ÑŽ…’¬-Œä’ƒƒm…),Œä’ƒƒm…,’†‰›“Œü, “Œ‹ž, ‹ž—tü, ç—t, ŠO–[ü,Ÿ‰Y"),
	 _T("ìè,“ŒŠC“¹ü,“Œ‹ž,‹ž—tü,‘h‰ä"),
	 _T("‘åã,“ŒŠC“¹ü,•ÄŒ´,–k—¤ü,’¼]’Ã"),
	 _T("ŽR‰È,“ŒŠC“¹ü,•ÄŒ´,–k—¤ü,’¼]’Ã"),
	 _T("ŽR‰È,“ŒŠC“¹ü,•ÄŒ´,–k—¤ü,‹ß]‰–’Ã"),		// NG
	 _T("‘åã,“ŒŠC“¹ü,•ÄŒ´,–k—¤ü,‹ß]‰–’Ã"),
	 _T("‘åã,“ŒŠC“¹ü,–¼ŒÃ‰®"),
	 _T("–¼ŒÃ‰®,“ŒŠC“¹ü,‘åã"),
	 _T("–¼ŒÃ‰®,“ŒŠC“¹ü,•ÄŒ´,–k—¤ü,•Ÿˆä"),
	 _T("–¼ŒÃ‰®,“ŒŠC“¹ü,•ÄŒ´"),
	 _T("“Ö‰ê,–k—¤ü,•ÄŒ´,“ŒŠC“¹ü,–¼ŒÃ‰®"),
	 _T("’¼]’Ã,–k—¤ü,•ÄŒ´,“ŒŠC“¹ü,‘åã"),
	 _T("’¼]’Ã,–k—¤ü,‹ß]‰–’Ã,ŒÎ¼ü, ŽR‰È, “ŒŠC“¹ü,‘åã"),
	 _T("‘åã,“ŒŠC“¹ü, ŽR‰È, ŒÎ¼ü, ‹ß]‰–’Ã, –k—¤ü, ’¼]’Ã"),
	 _T("‹ß]‰–’Ã,ŒÎ¼ü, ŽR‰È"),
	 _T("ŽR‰È, ŒÎ¼ü, ‹ß]‰–’Ã"),
	 _T("’¼]’Ã,–k—¤ü,‹ß]‰–’Ã,ŒÎ¼ü, ŽR‰È"),
	 _T("‹ß]‰–’Ã,ŒÎ¼ü, ŽR‰È, “ŒŠC“¹ü,‘åã"),
	 _T("ŽR‰È, ŒÎ¼ü, ‹ß]‰–’Ã, –k—¤ü, ’¼]’Ã"),
	 _T("‘åã,“ŒŠC“¹ü, ŽR‰È, ŒÎ¼ü, ‹ß]‰–’Ã"),
	 _T("‹ß]‰–’Ã,–k—¤ü,•ÄŒ´,“ŒŠC“¹ü,‘åã"),
	 _T("‹ß]‰–’Ã,–k—¤ü,•ÄŒ´"),
	 _T("•Ÿˆä,–k—¤ü,•ÄŒ´"),
	 _T("•ÄŒ´,–k—¤ü,•Ÿˆä"),
	 _T("‹ß]‰–’Ã,–k—¤ü,•ÄŒ´,“ŒŠC“¹ü,ŽR‰È"),		// NG
	 _T("’¼]’Ã,–k—¤ü,•ÄŒ´,“ŒŠC“¹ü,ŽR‰È"),
	 _T("‰ºŠÖ,ŽR—zü,Šâ‘,Šâ“¿ü,ŸŠÔ"),
	 _T("”ö“¹,ŽR—zü,‹ùƒ–•l,Šâ“¿ü,ŸŠÔ"),
//	 _T(",,,,,,,,,"),
	_T(""),
	};
	TCHAR buffer[1024];
	int i;
	vector<RouteItem> replace_route;
	for (i = 0; '\0' != *route_def[i]; i++) {
		route.removeAll();
		replace_route.clear();
		_tcscpy_s<1024>(buffer, route_def[i]);
		setup_route(buffer);
		int cnt = Route::ReRouteRule69j(route.routeList(), &replace_route);
		show_route();
		_ftprintf(os, _T(">>>>>>>>>>>%d\n"), cnt);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		show_route();
		_ftprintf(os, _T("-----------\n\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////////////
static void test_a86(void)
{
	TCHAR *route_def[] = {
    _T("•úo,‚¨‚¨‚³‚©“Œü,‹v•óŽ›,ŠÖ¼ü,‰¤Žq,˜a‰ÌŽRü,˜a‰ÌŽR,‹I¨ü,‹TŽR,ŠÖ¼ü,–¼ŒÃ‰®,“ŒŠC“¹ü,‹àŽR(’†)"),
	_T("‹àŽR(’†),“ŒŠC“¹ü,–¼ŒÃ‰®,ŠÖ¼ü,‹TŽR,‹I¨ü,˜a‰ÌŽR,˜a‰ÌŽRü,‰¤Žq,ŠÖ¼ü,‹v•óŽ›,‚¨‚¨‚³‚©“Œü,•úo"),
	_T("’†–ì,’†‰›“Œü,‰–K,’†‰›¼ü,‹àŽR(’†),“ŒŠC“¹ü,–¼ŒÃ‰®"),
	_T("‚”ö,’†‰›“Œü,‰–K,’†‰›¼ü,‹àŽR(’†),“ŒŠC“¹ü,–¼ŒÃ‰®"),
	_T("•iì,ŽRŽèü,‘ãX–Ø,’†‰›“Œü,‰–K,’†‰›¼ü,‹àŽR(’†),“ŒŠC“¹ü,–¼ŒÃ‰®"),
	_T("Š—“c,“ŒŠC“¹ü,•iì,ŽRŽèü,‘ãX–Ø,’†‰›“Œü,‰–K,’†‰›¼ü,‹àŽR(’†),“ŒŠC“¹ü,–¼ŒÃ‰®"),
	_T("’†–ì,’†‰›“Œü,‰–K,’†‰›¼ü,‹àŽR(’†)"),
	_T("‚”ö,’†‰›“Œü,‰–K,’†‰›¼ü,‹àŽR(’†)"),
	_T("•iì,ŽRŽèü,‘ãX–Ø,’†‰›“Œü,‰–K,’†‰›¼ü,‹àŽR(’†)"),
	_T("Š—“c,“ŒŠC“¹ü,•iì,ŽRŽèü,‘ãX–Ø,’†‰›“Œü,‰–K,’†‰›¼ü,‹àŽR(’†)"),
	_T("Š—“c,“ŒŠC“¹ü,•iì,ŽRŽèü,‘ãX–Ø,’†‰›“Œü,Œä’ƒƒm…,‘•ü(‹ÑŽ…’¬-Œä’ƒƒm…),H—tŒ´,“Œ–kü,“c’[,ŽRŽèü,’r‘Ü,Ô‰Hü(é‹žü),Ô‰H,“Œ–kü(é‹žü),‘å‹{,‚èü,‚è,ã‰zü,‹{“à,M‰zü(ŽÂƒmˆä-VŠƒ),V’Ã,‰H‰zü,H“c"),
	_T("H“c,‰H‰zü,V’Ã,M‰zü(ŽÂƒmˆä-VŠƒ),‹{“à,ã‰zü,‚è,‚èü,‘å‹{,“Œ–kü(é‹žü),Ô‰H,Ô‰Hü(é‹žü),’r‘Ü,ŽRŽèü,“c’[,“Œ–kü,H—tŒ´,‘•ü(‹ÑŽ…’¬-Œä’ƒƒm…),Œä’ƒƒm…,’†‰›“Œü,‘ãX–Ø,ŽRŽèü,•iì,“ŒŠC“¹ü,Š—“c"),
	_T("Š—“c,“ŒŠC“¹ü,•iì,ŽRŽèü,‘ãX–Ø,’†‰›“Œü,Œä’ƒƒm…,‘•ü(‹ÑŽ…’¬-Œä’ƒƒm…),H—tŒ´,“Œ–kü,·‰ª"),
	_T("·‰ª,“Œ–kü,H—tŒ´,‘•ü(‹ÑŽ…’¬-Œä’ƒƒm…),Œä’ƒƒm…,’†‰›“Œü,‘ãX–Ø,ŽRŽèü,•iì,“ŒŠC“¹ü,Š—“c"),
	_T("Š—“c,“ŒŠC“¹ü,“Œ‹ž,“Œ–kü,·‰ª"),
	_T("Ô‰H,“Œ–kü,·‰ª"),
	_T("Ô‰H,“Œ–kü(é‹žü),‘å‹{,“Œ–kü,·‰ª"),
	_T("Š—“c,“ŒŠC“¹ü,“Œ‹ž,’†‰›“Œü,¼‘•ªŽ›,•‘ –ìü,•‘ ‰Y˜a,“Œ–kü(é‹žü),‘å‹{,“Œ–kü,·‰ª"),
	_T("•‚ŠÔM“n,“Œ–kü(é‹žü),Ô‰H,Ô‰Hü(é‹žü),’r‘Ü,ŽRŽèü,Vh,’†‰›“Œü,¼–{"),
	_T("•‚ŠÔM“n,“Œ–kü(é‹žü),‘å‹{,“Œ–kü,·‰ª"),
	_T("·‰ª,“Œ–kü,‘å‹{,“Œ–kü(é‹žü),•‚ŠÔM“n"),
	_T("•‚ŠÔM“n,“Œ–kü(é‹žü),Ô‰H,“Œ–kü,·‰ª"),
	_T("•‚ŠÔM“n,“Œ–kü(é‹žü),•‘ ‰Y˜a,•‘ –ìü,“ì‰Y˜a,“Œ–kü,·‰ª"),
	_T("•‚ŠÔM“n,“Œ–kü(é‹žü),•‘ ‰Y˜a,•‘ –ìü,V¼ŒË,í”Öü,ŠâÀ,“Œ–kü,·‰ª"),
	_T("”Â‹´,Ô‰Hü(é‹žü),Ô‰H,“Œ–kü(”ö‹vŒo—R),“ú•é—¢,“Œ–kü,“Œ‹ž,“ŒŠC“¹VŠ²ü,–¼ŒÃ‰®"),
	_T("”Â‹´,Ô‰Hü(é‹žü),Ô‰H,“Œ–kü(”ö‹vŒo—R),“ú•é—¢,“Œ–kü,H—tŒ´,‘•ü(‹ÑŽ…’¬-Œä’ƒƒm…),Œä’ƒƒm…,’†‰›“Œü,‘ãX–Ø,ŽRŽèü,•iì,“ŒŠC“¹VŠ²ü,–¼ŒÃ‰®"),
	_T("Š—“c,“ŒŠC“¹ü,ìè,“ì•ü,—§ì,’†‰›“Œü,¼–{"),
	_T("¼‘åˆä,“ŒŠC“¹ü(¼‘åˆäŒo—R),•‘ ¬™,“ì•ü,•{’†–{’¬,•‘ –ìü,V¼ŒË,í”Öü,ŠâÀ"),
	 _T("‚ ‚¢‚Ì—¢Œö‰€,ŽDÀü,ŒK‰€,”ŸŠÙü,ŒÜ—ÅŠs,]·ü,–ØŒÃ“à,ŠC‹¬ü,’†¬‘,’ÃŒyü,ÂX,‰œ‰Hü,VÂX,“Œ–kVŠ²ü, “Œ‹ž,‹ž—tü,Žsì‰–•l,‹ž—tü(Žsì‰–•l-¼‘D‹´),¼‘D‹´,‘•ü,‹ÑŽ…’¬"),
	 _T("a’J,ŽRŽèü,‚“c”nê"),
	 _T("¬”,¬ŠCü,ŠCK"),
	 _T("V\’Ãì,ŽDÀü,ŒK‰€,”ŸŠÙü,ŒÜ—ÅŠs,]·ü,–ØŒÃ“à,ŠC‹¬ü,’†¬‘,’ÃŒyü,ÂX,‰œ‰Hü,VÂX,“Œ–kVŠ²ü, “Œ‹ž,‹ž—tü,Žsì‰–•l,‹ž—tü(Žsì‰–•l-¼‘D‹´),¼‘D‹´,•‘ –ìü,“ì‰Y˜a"),
	 _T("¼‰¬ŒE,’†‰›“Œü,Vh,ŽRŽèü,’r‘Ü,Ô‰Hü(é‹žü),Ô‰H,“Œ–kü,“Œ‹ž,‹ž—tü,Š‹¼—ÕŠCŒö‰€"),
	 _T("‰¡•l,“ŒŠC“¹ü,“Œ‹ž,‹ž—tü,‘h‰ä,ŠO–[ü,‘å–Ô,“Œ‹àü,¬“Œ,‘•ü,²‘q,¬“cü,‰ä‘·Žq,í”Öü,ŠâÀ"),
	 _T("’·’Ã“c,‰¡•lü,V‰¡•l,“ŒŠC“¹VŠ²ü,–¼ŒÃ‰®"),
	 _T("’·’Ã“c,‰¡•lü,V‰¡•l,“ŒŠC“¹VŠ²ü,ŽOŒ´"),
	 _T("V‰¡•l,“ŒŠC“¹VŠ²ü,ŽOŒ´"),
	 _T("•iì,ŽRŽèü,‘ãX–Ø,’†‰›“Œü,Œä’ƒƒm…,‘•ü(‹ÑŽ…’¬-Œä’ƒƒm…),H—tŒ´,“Œ–kü,“c’[,ŽRŽèü,’r‘Ü,Ô‰Hü(é‹žü),Ô‰H,“Œ–kü(é‹žü),‘å‹{,“Œ–kü,·‰ª"),
	 _T("‚ ‚¢‚Ì—¢Œö‰€,ŽDÀü,ŒK‰€,”ŸŠÙü,ŒÜ—ÅŠs,]·ü,–ØŒÃ“à,ŠC‹¬ü,’†¬‘,’ÃŒyü,ÂX,‰œ‰Hü,VÂX,“Œ–kVŠ²ü, “Œ‹ž"),
	 _T("‚ ‚¢‚Ì—¢Œö‰€,ŽDÀü,ŒK‰€,”ŸŠÙü,ŒÜ—ÅŠs,]·ü,–ØŒÃ“à,ŠC‹¬ü,’†¬‘,’ÃŒyü,ÂX,‰œ‰Hü,VÂX,“Œ–kVŠ²ü, “Œ‹ž,‹ž—tü,Žsì‰–•l,‹ž—tü(Žsì‰–•l-¼‘D‹´),¼‘D‹´,•‘ –ìü,“ì‰Y˜a"),
	 _T("“Œ‰Y˜a,•‘ –ìü,¼‘D‹´,‹ž—tü(Žsì‰–•l-¼‘D‹´),Žsì‰–•l,‹ž—tü,“Œ‹ž,“Œ–kVŠ²ü,VÂX,‰œ‰Hü,ÂX,’ÃŒyü,’†¬‘,ŠC‹¬ü,–ØŒÃ“à,]·ü,ŒÜ—ÅŠs,”ŸŠÙü,ŒK‰€,ŽDÀü,‚ ‚¢‚Ì—¢Œö‰€"),
	 _T("‚ ‚¢‚Ì—¢Œö‰€,ŽDÀü,ŒK‰€,”ŸŠÙü,ŒÜ—ÅŠs,]·ü,–ØŒÃ“à,ŠC‹¬ü,’†¬‘,’ÃŒyü,ÂX,‰œ‰Hü,VÂX,“Œ–kVŠ²ü, “Œ‹ž,‹ž—tü,Žsì‰–•l,‹ž—tü(Žsì‰–•l-¼‘D‹´),¼‘D‹´,‘•ü,‹ÑŽ…’¬"),
	 _T("‚ ‚¢‚Ì—¢Œö‰€,ŽDÀü,ŒK‰€,”ŸŠÙü,’·–œ•”,Žº—–ü,Àƒm’[,çÎü,”’Î(”Ÿ),”ŸŠÙü,•c•ä"),
	 _T("ŠCK,¬ŠCü,¬•£‘ò,’†‰›“Œü,”ª‰¤Žq,‰¡•lü,’·’Ã“c"),
//	 _T(",,,,,,,,,"),
	_T(""),
	};
	TCHAR buffer[1024];
	int i;
	vector<RouteItem> replace_route;
	for (i = 0; '\0' != *route_def[i]; i++) {
		route.removeAll();
		replace_route.clear();
		_tcscpy_s<1024>(buffer, route_def[i]);
		setup_route(buffer);
		int cnt = Route::ReRouteRule86j87j(route.routeList(), &replace_route);
		show_route();
		_ftprintf(os, _T(">>>>>>>>>>>%d\n"), cnt);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		show_route();
		_ftprintf(os, _T("-----------\n\n"));
	}
	//


	_ftprintf(os, _T("\n/////////////// 86->69 //////////////////////////////////////////////////\n"));

	
	for (i = 0; '\0' != *route_def[i]; i++) {
		route.removeAll();
		_tcscpy_s<1024>(buffer, route_def[i]);
		setup_route(buffer);
		show_route();

		int cnt = Route::ReRouteRule86j87j(route.routeList(), &replace_route);
		_ftprintf(os, _T(">>>>>>>>>>>%d\n"), cnt);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		cnt = Route::ReRouteRule69j(route.routeList(), &replace_route);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		show_route();
		_ftprintf(os, _T("-----------\n\n"));
	}

	_ftprintf(os, _T("\n/////////////// 69 -> 86 //////////////////////////////////////////////////\n"));

	
	for (i = 0; '\0' != *route_def[i]; i++) {
		route.removeAll();
		_tcscpy_s<1024>(buffer, route_def[i]);
		setup_route(buffer);
		show_route();

		int cnt = Route::ReRouteRule69j(route.routeList(), &replace_route);
		_ftprintf(os, _T(">>>>>>>>>>>%d\n"), cnt);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		cnt = Route::ReRouteRule86j87j(route.routeList(), &replace_route);
		route.routeList().clear();
		route.routeList().assign(replace_route.cbegin(), replace_route.cend());
		route.startStationId = route.routeList().cbegin()->stationId;
		show_route();
		_ftprintf(os, _T("-----------\n\n"));
	}
}


/////////////////////////////////////////////////////////////////////////////////////

//	TEST Main
//	InitDlg‚©‚çŒÄ‚Î‚ê‚é
//
int test_exec(void)
{
	fopen_s(&os, "test_result.txt", "w");

	_ftprintf(os, _T("\n---69---------------------------------------------------------\n"));
	test_a69();
	_ftprintf(os, _T("\n---86---------------------------------------------------------\n"));
	test_a86();

	fclose(os);
	return 1;
}


#endif






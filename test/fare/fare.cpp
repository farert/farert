#include <stdio.h>
#include <assert.h>
#define ASSERT assert
#define FALSE 0
#define TRUE  !FALSE
#define NumOf(c) (sizeof(c)/sizeof(c[0]))

const int fare_l_km[] = {
0,
11,
16,
21,
24,
29,
33,
38,
42,
47,
56,
65,
74,
83,
92,
101,
111,
129,
147,
165,
183,
201,
220,
238,
256,
274,
292,
311,
329,
347,
365,
383,
401,
420,
438,
456,
474,
492,
511,
529,
547,
583,
620,
656,
692,
729,
765,
801,
838,
874,
911,
947,
983,
1020,
1056,
1092,
1129,
1165,
1201,
};

const int fare_l_ckm[] = {
0,		// 0,
-1,     // 11,
-1,     // 16,
-1,     // 21,
-1,     // 24,
30,     // 29,
-1,     // 33,
39,     // 38,
-1,     // 42,
-1,     // 47,
-1,     // 56,
-1,     // 65,
-1,     // 74,
-1,     // 83,
96,     // 92,
-1,    // 101,
119,    // 111,
137,    // 129,
155,    // 147,
173,    // 165,
191,    // 183,
210,    // 201,
228,    // 220,
246,    // 238,
264,    // 256,
282,    // 274,
-1,    // 292,
319,    // 311,
337,    // 329,
355,    // 347,
373,    // 365,
391,    // 383,
410,    // 401,
428,    // 420,
446,    // 438,
464,    // 456,
482,    // 474,
501,    // 492,
519,    // 511,
537,    // 529,
564,    // 547,
601,    // 583,
637,    // 620,
673,    // 656,
710,    // 692,
746,    // 729,
782,    // 765,
819,    // 801,
855,    // 838,
892,    // 874,
928,    // 911,
964,    // 947,
1001,   // 983,
1037,   // 1020,
1073,   // 1056,
1110,   // 1092,
1146,   // 1129,
1182,   // 1165,
0,      // 1201,
};

const int fare_l_spcf[] = {
0,		// 0,
237,     // 11,
324,     // 16,
410,     // 21,
496,     // 24,
30,     // 29,
669,     // 33,
0,     // 38,
842,     // 42,
972,     // 47,
1144,     // 56,
1317,     // 65,
1490,     // 74,
1663,     // 83,
0,     // 92,
1944,    // 101,
0,    // 111,
0,    // 129,
0,    // 147,
0,    // 165,
0,    // 183,
0,    // 201,
0,    // 220,
0,    // 238,
0,    // 256,
0,    // 274,
5616,    // 292,
0,    // 311,
0,    // 329,
0,    // 347,
0,    // 365,
0,    // 383,
0,    // 401,
0,    // 420,
0,    // 438,
0,    // 456,
0,    // 474,
0,    // 492,
0,    // 511,
0,    // 529,
0,    // 547,
0,    // 583,
0,    // 620,
0,    // 656,
0,    // 692,
0,    // 729,
0,    // 765,
0,    // 801,
0,    // 838,
0,    // 874,
0,    // 911,
0,    // 947,
0,   // 983,
0,   // 1020,
0,   // 1056,
0,   // 1092,
0,   // 1129,
0,   // 1165,
0,      // 1201,
};
#define _TAX	5	/* è¡îÔê≈ [%] */

/* round up on 5 */
#define round(d) (((d) + 5) / 10 * 10)
#define round_up(d)     (((d) + 9) / 10 * 10)
#define round_down(d)   ((d) / 10 * 10)

/* è¡îÔê≈(éléÃå‹ì¸)â¡éZ */
#define taxadd(fare, tax)    (fare + ((fare * 1000 * tax / 100000) + 5) / 10 * 10)
#define taxadd_ic(fare, tax) (fare + (fare * 1000 * tax / 100000))



#define KM(kmx10) ((kmx10 + 9) / 10)	/* kmíPà Ç≈í[êîÇÕêÿÇËè„Ç∞ */

class FARE_INFO
{
public:
	static int Fare_basic(int km, int tax);
	static int Fare_sub(int km, int tax);
	static int Fare_tokyo(int km, int tax);
	static int Fare_osaka(int km, int tax);
	static int Fare_yamate(int km, int tax);
	static int Fare_osakakan(int km, int tax);
	static int Fare_hokkaido_basic(int km, int tax);
	static int Fare_shikoku(int km, int tax);
	static int Fare_kyusyu(int km, int tax);
	static int Fare_hokkaido_sub(int km, int tax);

	static int Fare_table_l(int km);
};


int FARE_INFO::Fare_table_l(int km)
{
	int i;
	
	for (i = 0; i < NumOf(fare_l_km); i++) {
		if (km < fare_l_km[i]) {
			break;
		}
	}
	if (NumOf(fare_l_km) < i) {
		printf("!!!!!!!!!!!!!!!Error!!!%i!!!!!!!\n", i);
		return 0;
	}
	if (fare_l_ckm[i - 1] == -1) {;
		return -(i - 1);
	} else {
		return fare_l_ckm[i - 1];
	}
}

//	â^í¿åvéZ(äÓñ{)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    åvéZÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz(JRìåICÇÕÇªÇÃÇ‹Ç‹ÅAëºÇÕÅAround()Ç∑ÇÈ
//
int FARE_INFO::Fare_basic(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 140;
		} else {
			return 144;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 180;
		} else {
			return 185;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 190;
		} else {
			return 195;
		}
	}
	if (6000 < km) {						// 600kmâzÇ¶ÇÕ40ÉLÉççèÇ›
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600kmÇÕ20ÉLÉççèÇ›
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmÇÕ10ÉLÉççèÇ›
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmÇÕ5ÉLÉççèÇ›
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	if (6000 <= c_km) {
		fare = 1620 * 3000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1620 * 3000 + 1285 * (c_km - 3000);
	} else {
		fare = 1620 * c_km;
	}
	if (c_km <= 1000) {						// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, tax);	// tax = +5%, éléÃå‹ì¸
}

//	sub: ínï˚åí ê¸
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    âcã∆ÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_sub(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 140;
		} else {
			return 144;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 180;
		} else {
			return 185;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 200;
		} else {
			return 206;
		}
	}
	
	if (12000 < km) {		// 1200kmâzÇ¶ÇÕï ï\ëÊ2çÜÉCÇÃ4Ç…Ç»Ç¢
		ASSERT(FALSE);
		return -1;
	}
	
	c_km = Fare_table_l(KM(km));
	if (c_km == 0) {
		ASSERT(FALSE);
		return -1;
	}
	if (c_km < 0) {
		return fare_l_spcf[-c_km];
	}
	
	c_km *= 10;

	if (5460 <= c_km) {
		fare = 1780 * 2730 + 1410 * 2730 + 770 * (c_km - 5460);
	} else if (2730 < c_km) {
		fare = 1780 * 2730 + 1410 * (c_km - 2730);
	} else {
		fare = 1780 * c_km;
	}
	if (c_km <= 1000) {						// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, tax);
}

//	b: ìdé‘ì¡íËãÊä‘(ìåãû)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    âcã∆ÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_tokyo(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 130;
		} else {
			return 133;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 150;
		} else {
			return 154;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 160;
		} else {
			return 165;
		}
	}
	
	if (6000 < km) {						// 600kmâzÇ¶ÇÕ40ÉLÉççèÇ›
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-600kmÇÕ20ÉLÉççèÇ›
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmÇÕ10ÉLÉççèÇ›
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmÇÕ5ÉLÉççèÇ›
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	if (3000 < c_km) {
		fare = 1530 * 3000 + 1215 * (c_km - 3000);
	} else {
		fare = 1530 * c_km;
	}
	if (c_km <= 1000) {						// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, tax);
}

//	C: ìdé‘ì¡íËãÊä‘(ëÂç„)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    âcã∆ÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_osaka(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 120;
		} else {
			return 120;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 170;
		} else {
			return 180;
		}
	}
	if (6000 < km) {						// 600kmâzÇ¶ÇÕ40ÉLÉççèÇ›
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-600kmÇÕ20ÉLÉççèÇ›
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmÇÕ10ÉLÉççèÇ›
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmÇÕ5ÉLÉççèÇ›
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	
	if (3000 < c_km) {
		fare = 1530 * 3000 + 1215 * (c_km - 3000);
	} else {
		fare = 1530 * c_km;
	}
	if (c_km <= 1000) {						// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, tax);	// tax = +5%, éléÃå‹ì¸
}

//	d: ìdé‘ì¡íËãÊä‘(éRéËê¸)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    âcã∆ÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_yamate(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 130;
		} else {
			return 133;
		} 
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 150;
		} else {
			return 154;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 160;
		} else {
			return 165;
		}
	}
	if (3000 < km) {						// 300kmâzÇ¶ÇÕñ¢íËã`
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-300kmÇÕ20ÉLÉççèÇ›
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmÇÕ10ÉLÉççèÇ›
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmÇÕ5ÉLÉççèÇ›
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	fare = 1325 * c_km;
	
	if (c_km <= 1000) {	// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, tax);	// tax = +5%, éléÃå‹ì¸
}

//	e: ìdé‘ì¡íËãÊä‘(ëÂç„ä¬èÛê¸)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    âcã∆ÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_osakakan(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 120;
		} else {
			return 120;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 170;
		} else {
			return 180;
		}
	}
	if (3000 < km) {						// 300kmâzÇ¶ÇÕñ¢íËã`
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-300kmÇÕ20ÉLÉççèÇ›
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmÇÕ10ÉLÉççèÇ›
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmÇÕ5ÉLÉççèÇ›
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	fare = 1325 * c_km;

	if (c_km <= 1000) {						// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, tax);	// tax = +5%, éléÃå‹ì¸
}

//	f: JRñkäCìπä≤ê¸
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    åvéZÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_hokkaido_basic(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 160;
		} else {
			return 170;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 200;
		} else {
			return 210;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 210;
		} else {
			return 220;
		}
	}

	if (6000 < km) {						// 600kmâzÇ¶ÇÕ40ÉLÉççèÇ›
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600kmÇÕ20ÉLÉççèÇ›
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmÇÕ10ÉLÉççèÇ›
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmÇÕ5ÉLÉççèÇ›
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	
	if (6000 < c_km) {
		fare = 1785 * 2000 + 1620 * 1000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1785 * 2000 + 1620 * 1000 + 1285 * (c_km - 3000);
	} else if (2000 < c_km) {
		fare = 1785 * 2000 + 1620 * (c_km - 2000);
	} else {
		fare = 1785 * c_km;
	}
	if (c_km <= 1000) {						// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, tax);	// tax = +5%, éléÃå‹ì¸
}

//	g: JRélçë
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    åvéZÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_shikoku(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 200;
		} else {
			return 210;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 210;
		} else {
			return 220;
		}
	}

	if (6000 < km) {						// 600kmâzÇ¶ÇÕ40ÉLÉççèÇ›
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600kmÇÕ20ÉLÉççèÇ›
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmÇÕ10ÉLÉççèÇ›
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmÇÕ5ÉLÉççèÇ›
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	
	if (6000 <= c_km) {
		fare = 1821 * 1000 + 1620 * 2000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1821 * 1000 + 1620 * 2000 + 1285 * (c_km - 3000);
	} else if (1000 < c_km) {
		fare = 1821 * 1000 + 1620 * (c_km - 1000);
	} else {
		fare = 1821 * c_km;
	}
	
	if (c_km <= 1000) {						// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, tax);	// tax = +5%, éléÃå‹ì¸
}

//	h: JRã„èB
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    åvéZÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_kyusyu(int km, int tax)
{
	int fare;
	int c_km;

	// 1961kmà»â∫ÇÕÉeÅ[ÉuÉãéQè∆Ç»ÇÃÇ≈ïsê≥ämÇ≈ÇÊÇ¢
	if (km <= 100) {						// Ç∆ÇÕÇ¢Ç¡ÇƒÇ‡100à»â∫ÇæÇØÇè»ó™
		return 0;
	}
	
	if (6000 < km) {						// 600kmâzÇ¶ÇÕ40ÉLÉççèÇ›
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600kmÇÕ20ÉLÉççèÇ›
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmÇÕ10ÉLÉççèÇ›
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmÇÕ5ÉLÉççèÇ›
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	if (6000 <= c_km) {
		fare = 1775 * 3000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1775 * 3000 + 1285 * (c_km - 3000);
	} else if (1000 < c_km) {
		fare = 1775 * c_km;
	} else {
		fare = 0;
	}

	if (c_km <= 1000) {							// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {									// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, tax);	// tax = +5%, éléÃå‹ì¸
}

//	JRñkäCìπínï˚åí ê¸
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    âcã∆ÉLÉç
//	@param [in] tax   ê≈ó¶
//	@return â^í¿äz
//
int FARE_INFO::Fare_hokkaido_sub(int km, int tax)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (tax == 5) {
			return 160;
		} else {
			return 170;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (tax == 5) {
			return 200;
		} else {
			return 210;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (tax == 5) {
			return 220;
		} else {
			return 230;
		}
	}
	
	if (12000 < km) {		// 1200kmâzÇ¶ÇÕï ï\ëÊ2çÜÉCÇÃ4Ç…Ç»Ç¢
		ASSERT(FALSE);
		return -1;
	}
	
	c_km = Fare_table_l(KM(km));
	if (c_km == 0) {
		ASSERT(FALSE);
		return -1;
	}
	if (c_km < 0) {
		return fare_l_spcf[-c_km];
	}
	
	c_km *= 10;

	if (5460 <= c_km) {
		fare = 1780 * 2730 + 1410 * 2730 + 770 * (c_km - 5460);
	} else if (2730 < c_km) {
		fare = 1780 * 2730 + 1410 * (c_km - 2730);
	} else {
		fare = 1780 * c_km;
	}
	if (c_km <= 1000) {						// 100kmà»â∫ÇÕêÿÇËè„Ç∞
		// 1ÇÃà ÇêÿÇËè„Ç∞
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100áqâzÇ¶ÇÕéléÃå‹ì¸
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, tax);
}

int main(int argc, char** argv)
{
	int i;
	int c[2];
	int b[2];
	
	b[0] = 0;
	b[1] = 0;

	printf("JRäÓ  IC  ÉLÉç: 5%%  8%%í èÌ 8%%IC\n");
	for (i = 0; i < 40000; i++) {
		c[0] = FARE_INFO::Fare_basic(i, 5);
		c[1] = FARE_INFO::Fare_basic(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRä≤ê¸ IC %5.1f:%4d (%4d %4d)\n", (float)i/10, round(c[0]), round(c[1]), c[1]);
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}
	printf("\n\n");
	printf("JRín  IC  ÉLÉç: 5%%  8%%í èÌ 8%%IC\n");
	for (i = 0; i <= 12000; i++) {
		c[0] = FARE_INFO::Fare_sub(i, 5);
		c[1] = FARE_INFO::Fare_sub(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRínï˚ IC %5.1f:%4d (%4d %4d)\n", (float)i/10, round(c[0]), round(c[1]), c[1]);
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}
	printf("\n\n");
	printf("JRìdìå IC  ÉLÉç: 5%%  8%%í èÌ 8%%IC\n");
	for (i = 0; i < 5000; i++) {
		c[0] = FARE_INFO::Fare_tokyo(i, 5);
		c[1] = FARE_INFO::Fare_tokyo(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRìdìå IC %5.1f:%4d (%4d %4d)\n", (float)i/10, round(c[0]), round_up(c[1]), c[1]);
//                                                                                  !!!!!!!!!!!!!
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}
	printf("\n\n");
	printf("JRç„ìd     ÉLÉç: 5%%  8%%\n");
	for (i = 0; i < 5000; i++) {
		c[0] = FARE_INFO::Fare_osaka(i, 5);
		c[1] = FARE_INFO::Fare_osaka(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRç„ìd    %5.1f:%4d %4d\n", (float)i/10, c[0], c[1]);
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}
	printf("\n\n");
	printf("JRéRéË IC  ÉLÉç: 5%%  8%%í èÌ 8%%IC\n");
	for (i = 0; i < 3000; i++) {
		c[0] = FARE_INFO::Fare_yamate(i, 5);
		c[1] = FARE_INFO::Fare_yamate(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRéRéË IC %5.1f:%4d (%4d %4d)\n", (float)i/10, round(c[0]), round_up(c[1]), c[1]);
//                                                                                   ^^^^^^^^
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}

	printf("\n\n");
	printf("JRç„ä¬     ÉLÉç: 5%%  8%%í èÌ\n");
	for (i = 0; i < 3000; i++) {
		c[0] = FARE_INFO::Fare_osakakan(i, 5);
		c[1] = FARE_INFO::Fare_osakakan(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRç„ä¬    %5.1f:%4d %4d\n", (float)i/10, c[0], c[1]);
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}

	printf("\n\n");
	printf("JRñkä≤     ÉLÉç: 5%%  8%%í èÌ\n");
	for (i = 0; i < 30000; i++) {
		c[0] = FARE_INFO::Fare_hokkaido_basic(i, 5);
		c[1] = FARE_INFO::Fare_hokkaido_basic(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRñkä≤    %5.1f:%4d %4d\n", (float)i/10, c[0], c[1]);
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}

	printf("\n\n");
	printf("JRélä≤     ÉLÉç: 5%%  8%%í èÌ\n");
	for (i = 0; i < 5000; i++) {
		c[0] = FARE_INFO::Fare_shikoku(i, 5);
		c[1] = FARE_INFO::Fare_shikoku(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRélä≤    %5.1f:%4d %4d\n", (float)i/10, c[0], c[1]);
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}
	printf("\n\n");
	printf("JRã„ä≤     ÉLÉç: 5%%  8%%í èÌ\n");
	for (i = 0; i < 5000; i++) {
		c[0] = FARE_INFO::Fare_kyusyu(i, 5);
		c[1] = FARE_INFO::Fare_kyusyu(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRã„ä≤    %5.1f:%4d %4d\n", (float)i/10, c[0], c[1]);
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}
	printf("\n\n");
	printf("JRñkín     ÉLÉç: 5%%  8%%í èÌ\n");
	for (i = 0; i < 5000; i++) {
		c[0] = FARE_INFO::Fare_hokkaido_sub(i, 5);
		c[1] = FARE_INFO::Fare_hokkaido_sub(i, 8);
		if (b[0] != c[0]) {
			if (b[1] != c[1]) {
				printf("JRñkín    %5.1f:%4d %4d\n", (float)i/10, c[0], c[1]);
			} else {
				printf("Error: \n");
			}
		} else if (b[1] != c[1]) {
			printf("error: \n");
		}
		b[0] = c[0];
		b[1] = c[1];
	}

	return 0;
}


#include <stdio.h>
#include <assert.h>
#define ASSERT assert
#define FALSE 0
#define TRUE  !FALSE

#define _TAX	5	/* Á”ïÅ [%] */

/* Á”ïÅ(lÌŒÜ“ü)‰ÁZ */
#define taxadd(fare) (fare + ((fare * 1000 * _TAX / 100000) + 5) / 10 * 10)

#define KM(kmx10) ((kmx10 + 9) / 10)	/* km’PˆÊ‚Å’[”‚ÍØ‚èã‚° */

class FARE_INFO
{
public:
	static int Fare_a(int km);
	static int Fare_b(int km);
	static int Fare_c(int km);
	static int Fare_d(int km);
	static int Fare_e(int km);
	static int Fare_f(int km);
	static int Fare_g(int km);
	static int Fare_h(int km);

};

//	‰^’ÀŒvZ(Šî–{)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ŒvZƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_a(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 140;
	}
	if (km < 61) {							// 4 to 6km
		return 180;
	}
	if (km < 101) {							// 7 to 10km
		return 190;
	}
	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	if (6000 <= c_km) {
		fare = 1620 * 3000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1620 * 3000 + 1285 * (c_km - 3000);
	} else {
		fare = 1620 * c_km;
	}
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	b: “dÔ“Á’è‹æŠÔ(“Œ‹)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_b(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 130;
	}
	if (km < 61) {							// 4 to 6km
		return 150;
	}
	if (km < 101) {							// 7 to 10km
		return 160;
	}
	
	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	if (3000 < c_km) {
		fare = 1530 * 3000 + 1215 * (c_km - 3000);
	} else {
		fare = 1530 * c_km;
	}
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	C: “dÔ“Á’è‹æŠÔ(‘åã)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_c(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 120;
	}
	if (km < 61) {							// 4 to 6km
		return 160;
	}
	if (km < 101) {							// 7 to 10km
		return 170;
	}
	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	
	if (3000 < c_km) {
		fare = 1530 * 3000 + 1215 * (c_km - 3000);
	} else {
		fare = 1530 * c_km;
	}
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	d: “dÔ“Á’è‹æŠÔ(Rèü)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_d(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 130;
	}
	if (km < 61) {							// 4 to 6km
		return 150;
	}
	if (km < 101) {							// 7 to 10km
		return 160;
	}
	if (3000 < km) {						// 300km‰z‚¦‚Í–¢’è‹`
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-300km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	fare = 1325 * c_km;
	
	if (c_km <= 1000) {	// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	e: “dÔ“Á’è‹æŠÔ(‘åãŠÂóü)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_e(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 120;
	}
	if (km < 61) {							// 4 to 6km
		return 160;
	}
	if (km < 101) {							// 7 to 10km
		return 170;
	}
	if (3000 < km) {						// 300km‰z‚¦‚Í–¢’è‹`
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-300km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	fare = 1325 * c_km;

	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	f: JR–kŠC“¹Š²ü
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ŒvZƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_f(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 160;
	}
	if (km < 61) {							// 4 to 6km
		return 200;
	}
	if (km < 101) {							// 7 to 10km
		return 210;
	}

	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
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
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	g: JRl‘
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ŒvZƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_g(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 160;
	}
	if (km < 61) {							// 4 to 6km
		return 200;
	}
	if (km < 101) {							// 7 to 10km
		return 210;
	}

	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
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
	
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	h: JR‹ãB
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ŒvZƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_h(int km)
{
	int fare;
	int c_km;

	// 1961kmˆÈ‰º‚Íƒe[ƒuƒ‹QÆ‚È‚Ì‚Å•s³Šm‚Å‚æ‚¢
	if (km <= 100) {						// ‚Æ‚Í‚¢‚Á‚Ä‚à100ˆÈ‰º‚¾‚¯‚ğÈ—ª
		return 0;
	}
	
	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
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

	if (c_km <= 1000) {							// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {									// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

int main(int argc, char** argv)
{
	int i;
	int a;
	int b;
	
	a = 0;
	for (i = 0; i < 7000; i++) {
		a = FARE_INFO::Fare_a(i);
		if (b != a) {
			printf("%5.1f:%4d\n", (float)i/10, a);
		}
		b = a;
	}
	return 0;
}


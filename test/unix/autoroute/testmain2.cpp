#include "stdafx.h"

#define DBPATH "../../../db/jrdb2017.db"

int g_tax = 8;
Route route;

#define RANDOM(l, h)	(rand() % (h - l + 1) + l)

static int32_t getrandom(void)
{
	static int c = 0;
	tstring s;
	int32_t t;

	++c;

	do {
		do {
			t = RANDOM(1, MAX_STATION);
			s = RouteUtil::StationName(t);
		} while (s.empty());
 	} while (((c % 13) != 0) && !STATION_IS_JUNCTION(t));

	return t;
}


#if 0
   Usage:
   	./test >/dev/null
   	or
   	./test >log
#endif

#define NumOf(c) (sizeof(c) / sizeof(c[0]))

int main(int argc, char** argv)
{
	int c = 0;
	int t = 0;
	int e[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int b = 0;
	int rc;
	int n;
	
	srand((unsigned)time(NULL));

	if (! DBS::getInstance()->open(DBPATH)) {
		printf("Can't db open\n");
		return -1;
	}

	for (c = 1; 1; c++) {	/* infinite */
		t = getrandom();
		for (n = 0; n < NumOf(e); n++) {
			e[n] = getrandom();
		}
		for (b = 0; b < 2; b++) {
			rc = route.add(t);
			if (rc <= 0) {
				fprintf(stderr, "error start terminal add %s: %d\n", RouteUtil::StationName(t).c_str(), rc);
				return -1;
			}
			for (n = 0; rc == 1 && n < 10; n++) {
				fprintf(stderr, "%6d-%02d auto route result(%s)%s->%s. ", 
				                                                c, n + 1,
				                                                b == 0 ? "nba" : "bal", 
																n != 0 ? "" : RouteUtil::StationName(t).c_str(), 
																RouteUtil::StationName(e[n]).c_str());
				fflush(stderr);
				rc = route.changeNeerest(b, e[n]);
				fprintf(stderr, "%d\n", rc);
			}
		}
	}
	return 0;
}

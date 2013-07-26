#include <stdio.h>

static int _b_km = 0, _c_km = 9, _e_km = 0, _bc1 = 0, _bc2 = 0, _bc3 = 0;

void fare(int km)
{
	int step;
	int dm;
	int b_km, e_km;
	int fare;
	int c_km;
	int bc1;
	int bc2;
	int bc3;
	
	step = 10;
	dm = 10;
	e_km = 0;
	
	while (e_km < km) {
		if (e_km < 60) {
			step = 30;
			dm = 10;
		} else if (e_km < 100) {
			step = 40;
			dm = 0;
		} else if (e_km < 500) {
			step = 50;
			dm = 10;
		} else if (e_km < 1000) {
			step = 100;
			dm = 0;
		} else if (e_km < 6000) {
			step = 200;
		} else {
			step = 400;
		}
		e_km += step;
	}
	b_km = e_km - step + 10;
	c_km = (e_km - (step - dm) / 2);	// calc km

	if (3000 <= c_km) {
		bc1 = 3000;
		if (6000 <= c_km) {
			bc2 = 3000;
			bc3 = c_km - 6000;
		}
		else {
			bc2 = c_km - 3000;
			bc3 = 0;
		}
	}
	else {
		bc1 = c_km;
		bc2 = bc3 = 0;
	}

	if (b_km != _b_km || 
	    c_km != _c_km || 
	    e_km != _e_km || 
	    bc1 != _bc1 || 
	    bc2 != _bc2 || 
	    bc3 != _bc3) {
		printf("%4d b_km=%5d, c_km=%5d, e_km=%5d, (%3d) bc1=%4d, bc2=%4d, bc3=%4d\n", km, b_km, c_km, e_km, e_km - b_km, bc1, bc2, bc3);
		
		_b_km = b_km;
	    _c_km = c_km;
	    _e_km = e_km;
	    _bc1 = bc1;
	    _bc2 = bc2;
	    _bc3 = bc3;
	}
}

int main(void)
{
	int i = 10;
	for (i = 10; i < 10000; i++) {
		fare(i);
	}
}

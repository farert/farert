#include "stdafx.h"

extern int test_exec(void);

int main(int argc, char** argv)
{
	if (! DBS::getInstance()->open("../../db/jr.db")) {
		printf("Can't db open\n");
		return -1;
	}
	test_exec();
	return 0;
}


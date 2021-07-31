#include <stdio.h>

typedef unsigned short u16_t;
typedef unsigned char u8_t;

typedef struct {
  u16_t name[32];
  u8_t lines[255];
} company_t;

company_t company[] = {
{L"JR�k�C��", {0, 1, 2, 3,}},
{L"JR�����{", {0}},
{L"JR���C", {0}},
{L"JR�����{", {0}},
{L"JR�l��", {0}},
{L"JR��B", {0}},
{L"�������X�S��",{0}},
{L"IGR�����", {0}},
};

void func(unsigned int u)
{
	int i;
	char p[] = "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh";
	for (i = 0; i < sizeof(p); i++) {
		if (p[i] == (u >> 24)) {
			break;
		}
	}
}

int main(int argc, char** argv)
{
	unsigned int n;
	printf("%s, %d, %d, %d, %d\n", company[0].name, company[0].lines[0], company[0].lines[1], company[0].lines[2], company[0].lines[3]);
	
	for (n = 0; n < 999999; n++) {
		func(n);
	}
	
	return 0;
}

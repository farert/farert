#include "stdafx.h"

extern int test_exec(void);
extern void test_route(const TCHAR *route_def[], int round = 0);
extern void test_autoroute(const TCHAR *route_def[]);
static void from_stream(char* file, int option_num);

int g_tax = 8;

char tbuf[1024];
char tbuf2[64];
const TCHAR* tr[] = { tbuf,  _T("") };
const TCHAR* tr_a[] = { tbuf,  tbuf2, _T("") };
// _T("../../../db/jrdb2018.db")

/*
	Usage
   コマンドラインh指定経路で、先頭に数字を付加するとそのまま test_routeの第2引数
   となる。
   0: 全結果出す
   1: 往復なし
   2: 特例適用なし
   3: 特定適用、往復なし
   4: 特例非適用なし
   5: 特例非適用＋往復なし
   6〜 無効(0指定とおなじ)
   ※ 最短経路、引数なしの標準テストでは無効

   引数1つで渡す定義ファイルは、空行と'#'で始まる行はスキップ.
   '/'で始まる行はそこで終了(続きは見ない)
*/
void usage(const char* prgm)
{
	fprintf(stderr, "Usage: %s [-h|-num] [<path>|<station1 ... stationN>]\n", prgm);
	fprintf(stderr, "      -h : show this.\n");
	fprintf(stderr, "      -num : result format.\n");
	fprintf(stderr, "             0: all\n");
	fprintf(stderr, "             1: no return\n");
	fprintf(stderr, "             2: no rule\n");
	fprintf(stderr, "             3: no rule + no return.\n");
	fprintf(stderr, "             4: no no_rule.\n");
	fprintf(stderr, "             5: no no_rule + no return.\n");
	fprintf(stderr, "             other: Non-available(default 0)\n");
	fprintf(stderr, "      path : route description file.\n");
	fprintf(stderr, "      　　　　　If the first column is '#', it represents a comment.\n");
	fprintf(stderr, "      　　　　　If the blank line, it's skip.\n");
	fprintf(stderr, "      　　　　　If the first column is '/', it end of line.\n");
	fprintf(stderr, "      station1 ... : route.\n");
	fprintf(stderr, "                     Number of odd: normal. \n");
	fprintf(stderr, "                     Number of even: auto route. \n");
}


int main(int argc, char** argv)
{
	int option_num = 0;
#if defined _WINDOWS
	_tsetlocale(LC_ALL, _T(""));	// tstring
#endif
	char* dbpath = getenv("farertDB");
	if (! DBS::getInstance()->open(dbpath)) {
		printf("Can't db open\n");
		return -1;
	}
	if (argc < 2) {
		test_exec();
	} else {
		if ('-' == *(*(argv + 1))) {
			if (('h' == *((*(argv + 1)) + 1)) || ('H' == *((*(argv + 1)) + 1))) {
				usage(*argv);
				return -1;
			}
			option_num = atoi((*(argv + 1)) + 1);
			++argv;
			--argc;
		}
		if (argc == 2) {
			from_stream(*++argv, option_num);
		} else {
			char *t = tbuf;

			for (int i = 1; i < argc; i++) {
				if (((argc % 2) != 0) && (i == (argc - 1))) {
					strcpy(tbuf2, *++argv);
				} else {
					strcpy(t, *++argv);
					t = tbuf + strlen(tbuf);
					*t++ = _T(' ');
					*t = _T('\0');
				}
			}
			if ((argc % 2) == 0) {
				test_route(tr, option_num);
			} else {
				test_autoroute(tr);
			}
		}
	}
	return 0;
}

static void rtrim(char* str)
{
	int len = strlen(str);

	for (len--; 0 <= len; --len) {
		if ((unsigned char)str[len] <= ' ') {
			str[len] = '\0';
		} else {
			break;
		}
	}
}

static void from_stream(char* file, int option_num)
{
	FILE* fp;
	fp = fopen(file, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open file: %s\n", file);
		return;
	}
	while (fgets(tbuf, sizeof(tbuf), fp)) {
		rtrim(tbuf);
		if (('\0' == tbuf[0]) || ('#' == tbuf[0])) {
			continue;
		} else if ('/' == tbuf[0]) {
			break;
		}
		test_route(tr, option_num);
	}
	fclose(fp);
}

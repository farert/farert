#include "stdafx.h"

extern int test_exec(void);
extern void test_route(const TCHAR *route_def[], int round = 0);
extern void test_autoroute(const TCHAR *route_def[], int option = 0);
static void from_stream(char* file, int option_num);
static const char* subword(const char* src_str, int num);
static int num_of_word(const char* buf);
static void rtrim(char* str);
static void remove_comment(char* str);

//int g_tax = 8;
int g_tax = 10;

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
	fprintf(stderr, "      -exec : Execute the all test.\n");
	fprintf(stderr, "      -num : (test_route).\n");
	fprintf(stderr, "               result format.\n");
	fprintf(stderr, "               0: all(default)\n");
	fprintf(stderr, "               1: no return\n");
	fprintf(stderr, "               2: no rule\n");
	fprintf(stderr, "               3: no rule + no return.\n");
	fprintf(stderr, "               4: no no_rule.\n");
	fprintf(stderr, "               5: no no_rule + no return.\n");
	fprintf(stderr, "               other: Non-available(default 0)\n");
	fprintf(stderr, "           (auto route only).\n");
	fprintf(stderr, "               1: zairaisen only\n");
	fprintf(stderr, "               2: with shinkansen\n");
	fprintf(stderr, "               3: with company line\n");
	fprintf(stderr, "               4: with company line and shinkansen\n");
	fprintf(stderr, "               0 or 4<: all(default)\n");
	fprintf(stderr, "              +10: One result for rule apllied only.\n");
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

	if (!dbpath) {
		fprintf(stderr, "Should be set environment variable the 'farertDB' for use database.\n");
		return -1;
	}

	if (!DBS::getInstance()->open(dbpath)) {
		fprintf(stderr, "Can't db open\n");
		return -1;
	}
	if (argc < 2) {
		/* show usage */
		usage(*argv);
		return -1;
	} else {
		if ('-' == *(*(argv + 1))) {
			/* Usage */
			if (0 == strncmp(*(argv + 1), "-exec", 5)) {
				/* normal: execute all test pattern */
				test_exec();
				return 0;
			} else {
				/* result variation */
				option_num = atoi((*(argv + 1)) + 1);
				++argv;
				--argc;
			}
		}
		if (argc == 2) {
			/* route from read file */
			from_stream(*++argv, option_num);
		} else {
			/* route as command line direct */
			/* even of num = auto, odd of num 0 route(start, line, end) */
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
				// option_num  0 or 5~9 線パターン  1 在来線のみ 2 新幹線使う 3 会社線もつかう 4 新幹線も会社線もつかう
				//        +10 特例適用のみ表示

				test_autoroute(tr_a, option_num);
			}
		}
	}
	return 0;
}


static void from_stream(char* file, int option_num)
{
	FILE* fp;
	int c;
	const char* lp;

	fp = fopen(file, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open file: %s\n", file);
		return;
	}
	while (fgets(tbuf, sizeof(tbuf), fp)) {
		remove_comment(tbuf);
		rtrim(tbuf);
		if (('\0' == tbuf[0]) || ('#' == tbuf[0])) {
			continue;
		} else if ('/' == tbuf[0]) {
			break;
		}
		c = num_of_word(tbuf);
		if ((c % 2) != 0) {
			test_route(tr, option_num);
		} else {
			lp = subword(tbuf, c);
			strcpy(tbuf2, lp);
			tbuf[lp - tbuf] = '\0';
			test_autoroute(tr_a, option_num);
		}
	}
	fclose(fp);
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

static void remove_comment(char* str)
{
	int len = strlen(str);

	for (len--; 0 <= len; --len) {
		if ('#' == str[len]) {
			str[len] = '\0';
		}
	}
}

static int num_of_word(const char* buf)
{
	char* p;
	const char* sep = " \t";
	int c = 0;
	char* str = (char*)malloc(strlen(buf) + 1);

	if (NULL == str) {
		return 0;
	}

	strcpy(str, buf);

	p = strtok(str, sep);
	while (p != NULL) {
		c++;
		p = strtok(NULL, sep);
	}
	free(str);
	return c;
}

// numワードめの文字列ポインタを返す. numは1~Nで、1ならstrを返す
static const char* subword(const char* src_str, int num)
{
	char* p;
	const char* sep = " \t";
	int c = 0;
	char* str = (char*)malloc(strlen(src_str) + 1);

	if (NULL == str) {
		return src_str + strlen(src_str); // empty string
	}

	strcpy(str, src_str);

	p = strtok(str, sep);
	while (p != NULL) {
		c++;
		if (num <= c) {
			return src_str + (p - str);
		}
		p = strtok(NULL, sep);
	}
	free(str);
	return src_str + strlen(src_str); // empty
}

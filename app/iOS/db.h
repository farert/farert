#if !defined _SQLITEOPE_H_
#define _SQLITEOPE_H_

#ifdef _windows
#include <windows.h>
#include <tchar.h>
#include <AtlBase.h>
#include <AtlConv.h>
#endif
#include <stdio.h>
#include "sqlite3.h"

#include <string>
#include <map>
using namespace std;

typedef std::basic_string<TCHAR> tstring;


/*
 win32 unicode build.
 
 sqlite3 database class.
 copyright(c) sutezo9@me.com / 2012. allrights reserverd.
 
*/

//#define TRACE	printf

#if 0
	CT2A a(s, CP_UTF8);
	file.Write(a, strlen(a));

	CA2T t(buf, CP_UTF8);
#endif

#if !defined NULL
#define NULL ((void*)0)
#endif

// UNICODE ビルドのみしか使用しなければ、以下マクロは使用せずともCT2A, CA2Tのみを使用で可
#ifdef UNICODE
#define t2utf8(s, d)	CT2A d(s, CP_UTF8)
#define utf82t(s, d)	CA2T d(s, CP_UTF8)
#else
#define t2utf8(s, d)	CW2A d(CA2W(s), CP_UTF8)
#define utf82t(s, d)	CW2A d(CA2W(s, CP_UTF8))
#endif


/* -- database object class define -- */

class DBO
{
	sqlite3_stmt* m_stmt;
	bool m_bEof;
	bool m_lock;
public:
	DBO() 
	{
		m_stmt = NULL;
		m_bEof = false;
		m_lock = false;
	}
	DBO& operator=(const DBO& rdbo)
	{
		try {
			finalize();
		} catch (...) {

		}
		if (m_stmt) {
			finalize();
		}
		m_bEof = rdbo.m_bEof;
		m_stmt = rdbo.m_stmt;
		m_lock = rdbo.m_lock;
		//const_cast<DBO&>(rdbo).m_stmt = 0;
		//const_cast<DBO&>(rdbo).m_lock = false;

		return *this;
	}

	DBO(const DBO& rdbo) 
	{
		m_bEof = rdbo.m_bEof;
		m_stmt = rdbo.m_stmt;
		m_lock = rdbo.m_lock;
		//const_cast<DBO&>(rdbo).m_stmt = 0;
	}

	DBO(sqlite3_stmt* stmt, bool en_cache) 
	{
		m_bEof = false;
		m_stmt = stmt;
		m_lock = en_cache;
	}
	
	~DBO() {
		finalize();
	}

	operator bool() const { return m_stmt != NULL; }
	operator sqlite3_stmt*() const { return m_stmt; }

	bool isvalid() const { return (m_stmt != NULL) && (m_bEof == false); }

	bool setParam(int index, int value);
	
///	bool setParam(int index, LPCTSTR value);
	bool setParam(int index, const char* value);	/* UTF-8 */
	
	int colType(int index);
	int getNumOfCol();

	bool moveNext();

	tstring getText(int col);

	int getInt(int col);
	
	bool finalize(); 

	bool reset();
};


/* -- database source class define -- */
class DBS
{
	sqlite3* 	  m_db;

	static map<string, sqlite3_stmt*> cache_pool;

	void cleanup(void) {
		if (m_db) {
			DBS::cache_cleanup(false);
			sqlite3_close(m_db);
			m_db = NULL;
		}
	}
	
	DBS(DBS* pDB) {
		m_db = pDB->m_db;
	}

	DBS() {
		// CT2A utf8_dbname(_T(":memory:"), CP_UTF8);
		if (0 != sqlite3_open_v2(":memory:", &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)) {
			TRACE("DB Can't open\n");
			m_db = NULL;
		}
	}
	~DBS() {
		cleanup();
		}
public:
	static DBS* getInstance() {
		static DBS s_db;
		return &s_db;
#if 0
		static DBS* pThis = 0;
		if (!pThis) {
			pThis = new DBS();
		}
		return pThis;
#endif
	}

	static void cache_cleanup(bool special);

	operator sqlite3*() { return m_db; }

	bool open(LPCTSTR dbpath);// {

#if 0
	compileSql()の戻されたDBOはテンポラリです。デストラクタ実行でクローズされます.
	 恒久的に使用する場合, cache=true で実行する必要があります.
	 また、sql文の行頭を空コメント("/**/")にしておくと、キャッシュ削除するcache_cleanup()関数
	 をtrue呼び出ししたときにそのsql文のみ削除(クリーンアップ)されます
#endif
	DBO compileSql(const char* sql, bool cache = false) 
	{
		int rc;
		sqlite3_stmt* stmt;

		if (cache) {
			stmt = cache_pool[sql];
#ifdef DEB_PRINT
			TRACE("reuse stmt=%d\n", stmt);
#endif
			if (0 != stmt) {
				DBO dbo(stmt, true);
				dbo.reset();
				return dbo;
			}
		}
		rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, 0); /* single statement only */
		if (0 != rc) {
			TRACE("Error: prepare=%d, %s\n", rc, sqlite3_errmsg(m_db));
			stmt = NULL;
		} else if (cache) {
			if (0 == cache_pool[sql]) {
				cache_pool[sql] = stmt;
			}
		}
#ifdef DEB_PRINT
		TRACE("new stmt=%d\n", stmt);
#endif
		return DBO(stmt, cache);
	}

	/* ex.)
		int	func(void* param, int nCol, char** colsValues, char** colNames)
		{
			for (int i = 0; i < nCol; i++) {
				CA2T s(*(colsValues + i), CP_UTF8);
				_tprintf(_T("%s |"), s);
			}
			_tprintf(_T("\n"));
			return 0;
		}
	 */
	int exec(const char* sql, 
			 int(*callback)(void* param, int nCol, char** colsValues, char** colNames),
			 void *param) {
		char *pErrmsg;
		int rc;
		rc =sqlite3_exec(m_db, sql, callback, param, &pErrmsg);
		if (0 != rc) {
			TRACE("Error: %s\n", pErrmsg);
		}
		return rc;
	}

	const char* errmsg() { return sqlite3_errmsg(m_db); }
};

#endif	// _SQLITEOPE_H_


#if !defined _SQLITEOPE_H_
#define _SQLITEOPE_H_

#ifdef _WINDOWS
#include <windows.h>
#include <tchar.h>
#include <AtlBase.h>
#include <AtlConv.h>
#endif
#include <stdio.h>
#include "sqlite3.h"

#include <string>
#include <map>
#include <vector>

#define MAX_STMT_POOL 20	// *** MAX OF CACHE POOL NUM ***

typedef unsigned int uint32_t;
typedef int int32_t;

using namespace std;
uint32_t get_time_msec(void);

typedef std::basic_string<TCHAR> tstring;

#if !defined _WINDOWS || defined TEST
#define READ_ONLY_FILE_DIRECT
#else
#undef READ_ONLY_FILE_DIRECT
#endif


#if 0
extern vector<string> sqlstr;
#endif

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


struct STMT_CACHE 
{
    uint32_t tmstamp;
    uint32_t  count;
    sqlite3_stmt* stmt;
    STMT_CACHE(sqlite3_stmt* _stmt) {
    	stmt = _stmt;
    	count = 0;
    	tmstamp = get_time_msec();
    }
    ~STMT_CACHE() {
    	sqlite3_finalize(stmt);
    }
};






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
	

#ifdef UNICODE
    bool setParam(int index, LPCTSTR value);
#endif
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

	static map<string, STMT_CACHE*> cache_pool;

	void cleanup(void) {
		if (m_db) {
			DBS::cache_cleanup();
			sqlite3_close(m_db);
			m_db = NULL;
		}
	}

	DBS(DBS* pDB) {
		m_db = pDB->m_db;
	}

	DBS() {
#ifndef READ_ONLY_FILE_DIRECT
		// CT2A utf8_dbname(_T(":memory:"), CP_UTF8);
		if (0 != sqlite3_open_v2(":memory:", &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)) {
			TRACE("DB Can't open\n");
			m_db = NULL;
		}
#endif
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

	static void cache_cleanup();

	operator sqlite3*() { return m_db; }

	bool open(LPCTSTR dbpath);
    void close(void) { cleanup(); }

#if 0
	compileSql()の戻されたDBOはテンポラリです。デストラクタ実行でクローズされます.
	 恒久的に使用する場合, cache=true で実行する必要があります.
#endif
	DBO compileSql(const char* sql, bool cache = true) {
		int rc;
#if 0
        sqlstr.push_back(sql);
#endif
		if (cache && (cache_pool.find(sql) != cache_pool.end())) {
				// found
		    STMT_CACHE* cache = (STMT_CACHE*)cache_pool[sql];
		    cache->count++;
		    cache->tmstamp = get_time_msec();

			DBO dbo(cache->stmt, true);
			dbo.reset();
			return dbo;

		} else {
			sqlite3_stmt* stmt;

			rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, 0); /* single statement only */
			if (0 != rc) {
				TRACE("Error: prepare=%d, %s\n", rc, sqlite3_errmsg(m_db));
				return DBO();
			}
			if (cache) {
				/* garbage collection(oldist cache remove) */
				cache_restrict(sql);
			    /* regist cache entry */
			    cache_pool[sql] = new STMT_CACHE(stmt);
			}
         	return DBO(stmt, cache);
		}
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
private:
	void cache_restrict(const char* sql);
};

#endif	// _SQLITEOPE_H_


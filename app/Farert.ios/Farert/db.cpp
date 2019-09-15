#include "stdafx.h"
#include "db.h"
#if !defined _WINDOWS	// get_time_msec()
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#endif

/*!	@file db.cpp
 *	Copyright(c) sutezo9@me.com 2012.
 */
#if 0
vector<string> sqlstr;
#endif

//static 
map<string, STMT_CACHE*> DBS::cache_pool;


bool DBS::open(LPCTSTR dbpath) 
{
    sqlite3_shutdown();
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    sqlite3_initialize();
#ifdef READ_ONLY_FILE_DIRECT
#ifdef _WIN32
	CT2A sjispath(dbpath);
	if (0 != sqlite3_open_v2(sjispath, &m_db, SQLITE_OPEN_READONLY, 0)) {
#else
        TRACE("dbpath=%s\n", dbpath);
	if (0 != sqlite3_open_v2(dbpath, &m_db, SQLITE_OPEN_READONLY|SQLITE_OPEN_NOMUTEX|SQLITE_OPEN_PRIVATECACHE, 0)) {
#endif
		TRACE("Database can't open\n");
		cleanup();
		return false;
	}
#else
	int rc;
	int rtc;

	sqlite3_backup* pBackup;
	sqlite3* dbtmp;
#ifdef UNICODE
	CT2A utf8_dbpath(dbpath, CP_UTF8);
	if (0 != sqlite3_open_v2(utf8_dbpath, &dbtmp, SQLITE_OPEN_READONLY, 0)) {
		TRACE("temporary database can't open\n");
		cleanup();
		return false;
	}
#else
	if (0 != sqlite3_open_v2(dbpath, &dbtmp, SQLITE_OPEN_READONLY, 0)) {
		TRACE("temporary database can't open\n");
		cleanup();
		return false;
	}
#endif
	pBackup = sqlite3_backup_init(m_db, "main", dbtmp, "main");
	if (pBackup == 0) {
		TRACE("error: %s\n", errmsg());
		cleanup();
		return false;
	}
	rtc = 0;
	while ((rc = sqlite3_backup_step(pBackup, 100)) == SQLITE_OK ||
			rc == SQLITE_BUSY) {
		if (rc == SQLITE_BUSY) {
			if (3 <= rtc++) {
				break;
			}
			sqlite3_sleep(100);
		}
	}
	sqlite3_backup_finish(pBackup);
	sqlite3_close(dbtmp);

	if (rc != SQLITE_DONE) {
		if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
			TRACE("Error: source database is busy\n");
		} else {
			TRACE("Error %d, %s\n", rc, errmsg());
		}
		return false;
	}
#endif
	return true;
}


//static 
void DBS::cache_cleanup(void)
{
	map<string, STMT_CACHE*>::iterator itr = cache_pool.begin();
	while (itr != cache_pool.end()) {
		//printf("delete %s ", itr->first.c_str());
		delete itr->second;
		itr++;
	}
	cache_pool.clear();
}

void DBS::cache_restrict(const char* sql)
{
	while (MAX_STMT_POOL <= cache_pool.size()) {
		unsigned int current = get_time_msec();
		unsigned int history = 0;
		string key;
		map<string, STMT_CACHE*>::iterator itr = cache_pool.begin();
		while (itr != cache_pool.end()) {
			STMT_CACHE* cache = (STMT_CACHE*)itr->second;
			if (history < (current - cache->tmstamp)) {
				history = (current - cache->tmstamp);
				key = itr->first;	// remove candidate
			}// else {
			//	printf("%d\n", cache->tmstamp);
			//}
			itr++;
		}
		if (key == "") {
			ASSERT(FALSE);	/* OK */
		} else {
			delete cache_pool[key];
			cache_pool.erase(key);
		}
	}
}


bool DBO::setParam(int index, int value) 
{
	int rc;
	rc = sqlite3_bind_int(m_stmt, index, value);
	if (rc != SQLITE_OK) {
		TRACE("Error: bind(int)=%d, %s\n", rc, DBS::getInstance()->errmsg());
		return false;
	}
	return true;
}

#ifdef UNICODE
bool DBO::setParam(int index, LPCTSTR value) 
{
	int rc;
	CT2A utf8_value(value, CP_UTF8);

	rc = sqlite3_bind_text(m_stmt, index, utf8_value, -1, SQLITE_TRANSIENT);
	if (rc != SQLITE_OK) {
		TRACE("Error: bind(text)=%d, %s\n", rc, DBS::getInstance()->errmsg());
		return false;
	}
	return true;
}
#endif

bool DBO::setParam(int index, const char* value) 
{
	int rc;

	rc = sqlite3_bind_text(m_stmt, index, value, -1, SQLITE_TRANSIENT);
	if (rc != SQLITE_OK) {
		TRACE("Error: bind(text)=%d, %s\n", rc, DBS::getInstance()->errmsg());
		return false;
	}
	return true;
}

bool DBO::moveNext() 
{
	int rc;
	if (m_bEof) {
		return false;
	}
	rc = sqlite3_step(m_stmt);
	if ((SQLITE_DONE != rc) && (SQLITE_ROW != rc)) {
		TRACE("Error: step=%d, %s\n", rc, DBS::getInstance()->errmsg());
		return false;
	}
	if (SQLITE_DONE == rc) {
		m_bEof = true;
		return false;
	}
	return true;
}

tstring DBO::getText(int col) 
{
#if defined _WINDOWS // (UNICODE only)
	CA2T text(reinterpret_cast<LPCSTR>(sqlite3_column_text(m_stmt, col)), CP_UTF8);
    return tstring(text);
#else
    return (char*)sqlite3_column_text(m_stmt, col);
#endif
}

int DBO::getInt(int col) 
{
	return sqlite3_column_int(m_stmt, col);
}

bool DBO::finalize() 
{
	int rc;
	if (m_lock || !m_stmt) {
#ifdef DEB_PRINT
		TRACE("fin lock %d\n", m_stmt);
#endif
		return true;
	}
	rc = sqlite3_finalize(m_stmt);
	if (SQLITE_OK != rc) {
		TRACE("Error: sqlite3 finalize()=%d, %s\n", rc, DBS::getInstance()->errmsg());
		return false;
	}
#ifdef DEB_PRINT
	TRACE("fin=%d\n", m_stmt);
#endif
	m_stmt = 0;
	m_bEof = false;
	return true;
}

bool DBO::reset() 
{
	int rc = sqlite3_reset(m_stmt);
	if (SQLITE_OK != rc) {
		TRACE("Error: sqlite3 reset()=%d, %s\n", rc, DBS::getInstance()->errmsg());
		return false;
	} else {
		m_bEof = false;
		return true;
	}
}

int DBO::getNumOfCol() 
{
	return sqlite3_column_count(m_stmt);
}

/*	index: Number of Column(0 to sqlite3_column_count(m_stmt) - 1)
 *	@return SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, SQLITE_NULL
 */
int DBO::colType(int index) 
{
	return sqlite3_column_type(m_stmt, index);
}



#if !defined _WINDOWS
// MAC, Unix

/*! @brief Returns the current time in millisecond.
 *
 *  @return the current time (millisecond)
 */
uint32_t get_time_msec(void)
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    return (uint32_t)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}
#else
// _WINODWS
uint32_t get_time_msec(void)
{
	return (uint32_t)GetTickCount();
}

#endif

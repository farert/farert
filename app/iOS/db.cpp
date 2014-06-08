
#include "stdafx.h"
#include "db.h"

/*!	@file db.cpp
 *	Copyright(c) sutezo9@me.com 2012.
 */

//static 
map<string, sqlite3_stmt*> DBS::cache_pool;


bool DBS::open(LPCTSTR dbpath) 
{
	int rc;
	int rtc;

	sqlite3_backup* pBackup;
	sqlite3* dbtmp;

	if (0 != sqlite3_open_v2(dbpath, &dbtmp, SQLITE_OPEN_READONLY, 0)) {
		TRACE("temporary database can't open\n");
		cleanup();
		return false;
	}
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
	return true;
}

//static 
void DBS::cache_cleanup(bool special)
{
	map<string, sqlite3_stmt*>::iterator itr = cache_pool.begin();
	while (itr != cache_pool.end()) {
						/* sqli∂CAcsi邃｢C邃｢AAaUi°eicsEREAEiEgCAeIcaAAELEEEbEVEOcIeuC∑CE
						 */
		if (!special || (itr->first.substr(0, 4).compare("/**/") == 0)) {
			DBO dbo(itr->second, false);
			cache_pool.erase(itr++);
		} else {
			itr++;
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

#if 0
bool DBO::setParam(int index, LPCTSTR value) 
{
	int rc;

	rc = sqlite3_bind_text(m_stmt, index, value, -1, SQLITE_TRANSIENT);
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
    return (char*)sqlite3_column_text(m_stmt, col);
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



#include "stdafx.h"
#include "alpfile.h"

const LPCTSTR FILE_HISTORY = _T("history.txt");


///////////////////////////////////////////////////////////////
#if HISTORY_SEPARATE_MEMORY
HistoryFile::HistoryFile(bool bkind)
{
	b_kind = bkind;
}
#endif


///////////////////////////////////////////////////////////////
HistoryFile::HistoryFile()
{
	b_kind = false;
}



///////////////////////////////////////////////////////////////
//static
void HistoryFile::remove()
{
	DeleteFile(FILE_HISTORY);
}


///////////////////////////////////////////////////////////////
bool HistoryFile::save(LPCTSTR szStation)
{
	if (file.m_hFile != CFile::hFileNull) {
		file.Close();
	}

#if HISTORY_SEPARATE_MEMORY
	CString type(b_kind ? _T("’…") : _T("”­"));
#endif
	if (file.Open(FILE_HISTORY, CFile::modeReadWrite | CFile::modeNoTruncate)) {
		while (file.ReadString(record)) {
#if HISTORY_SEPARATE_MEMORY
			if (record.Left(record.FindOneOf(_T("\t "))) == type) {
				if (0 ==
					record.Mid(record.FindOneOf(_T("\t "))).TrimLeft().Compare(szStation)) {
					return true;
				}
			}
#else
			if (record == szStation) {
				return true;
			}
#endif
		}
		file.SeekToEnd();
	} else if (!file.Open(FILE_HISTORY, CFile::modeWrite | CFile::modeCreate)) {
		return false;
	}
#if HISTORY_SEPARATE_MEMORY
	record = type + _T("\t");
	record += szStation;
#else
	record = szStation;
#endif
	record += _T("\n");
	file.WriteString(record);

	return true;
}


///////////////////////////////////////////////////////////////
bool HistoryFile::first()
{
	if (file.m_hFile != CFile::hFileNull) {
		file.Close();
	}
	return TRUE == file.Open(FILE_HISTORY, CFile::modeRead | CFile::typeText);
}

///////////////////////////////////////////////////////////////
LPCTSTR HistoryFile::next()
{
#if HISTORY_SEPARATE_MEMORY
	CString type(b_kind ? _T("’…") : _T("”­"));
#endif
	if ((file.m_hFile != CFile::hFileNull) && file.ReadString(record)) {
		int pos;

#if HISTORY_SEPARATE_MEMORY
		if (record.Left(record.FindOneOf(_T("\t "))) == type) {
			record = record.Mid(record.FindOneOf(_T("\t "))).TrimLeft();
			pos = record.Find(_T('['));
			if (0 <= pos) {
				record = record.Left(pos);
			}
		}
#else
		pos = record.Find(_T('['));
		if (0 <= pos) {
			record = record.Left(pos);
		}
#endif
		return record.GetBuffer();
	} else {
		return 0;
	}
}

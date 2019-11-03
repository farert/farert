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

//
const TCHAR* INI_FILE = _T("farert.ini");

bool isKeyExist(const CString& key)
{
	try {
		CString s;
		CStdioFile file;
		CString fname(INI_FILE);
		if (file.Open(fname, CFile::modeRead)) {
			while (file.ReadString(s)) {
				if (0 <= s.Find(key)) {
					return true;
				}
			}
		}
	}
	catch (CFileException* e) {
		CString s;
		e->GetErrorMessage(s.GetBuffer(260), 260);
		s.ReleaseBuffer();
		AfxMessageBox(s, MB_ICONSTOP);
		e->Delete();
	}
	return false;
}

void putKey(const CString& key)
{
	if (isKeyExist(key)) {
		return;
	}
	try {
		CString s;
		CStdioFile file;
		CString fname(INI_FILE);
		if (file.Open(fname, CFile::modeReadWrite| CFile::modeCreate | CFile::modeNoTruncate)) {
			file.SeekToEnd();
			if (key.Right(1) != '\n') {
				s = key + _T("\n");
			}
			else {
				s = key;
			}
			file.WriteString(s);
		}
	}
	catch (CFileException* e) {
		CString s;
		e->GetErrorMessage(s.GetBuffer(260), 260);
		s.ReleaseBuffer();
		AfxMessageBox(s, MB_ICONSTOP);
		e->Delete();
	}
}

void removeKey(const CString& key)
{
	try {
		CString s;
		CStdioFile file_old;
		CStdioFile file_new;
		CString fname(INI_FILE);
		CString ftemp;

		GetTempFileName(_T("./"), _T("~~"), 20191022,
			ftemp.GetBuffer(MAX_PATH));
		ftemp.ReleaseBuffer();

		if (file_new.Open(ftemp, CFile::modeWrite | CFile::modeCreate)) {
			if (file_old.Open(INI_FILE, CFile::modeRead)) {
				while (file_old.ReadString(s)) {
					if (s.Find(key) < 0) {
						file_new.WriteString(key);
					}
				}
				file_old.Close();
				DeleteFile(INI_FILE);
				file_new.Close();
				MoveFile(ftemp, INI_FILE);
			}
		}
		else {
			AfxMessageBox(_T("Failed remove key"), MB_ICONERROR);
		}
	}
	catch (CFileException* e) {
		CString s;
		e->GetErrorMessage(s.GetBuffer(260), 260);
		s.ReleaseBuffer();
		AfxMessageBox(s, MB_ICONSTOP);
		e->Delete();
	}
}


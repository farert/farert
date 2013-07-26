#if !defined _ALPFILE_H__
#define _ALPFILE_H__


class HistoryFile
{
	CString record;
	CStdioFile file;
	bool b_kind;
public:
#if HISTORY_SEPARATE_MEMORY
	HistoryFile(bool bkind);
#endif
	HistoryFile();
	
	static void remove();
	bool save(LPCTSTR szStation);

	/* iterator */
	bool first();
	LPCTSTR next();
};


#endif	/* _ALPFILE_H__ */


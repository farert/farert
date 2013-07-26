-- 2012-5-9

-- ‰ïĞü˜Hü‚©‚Ç‚¤‚©”»’è(0‚ÅJRA”ñ0‚Å‰ïĞü)
select count(*) from t_lines where line_id=? and (lflg & (1 << 23))!=0;

-- ‰ïĞü˜Hüˆê——‚ğ“¾‚ç‚ê‚é
select * from t_line where rowid in (select line_id from t_lines where (lflg & (1 << 23))!=0);
  -- or
select * from t_line n where exists (select * from t_lines l where l.line_id=n.rowid and (lflg & (1 << 23))!=0);



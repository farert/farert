-- 2012-5-9

-- ��А��H�����ǂ�������(0��JR�A��0�ŉ�А�)
select count(*) from t_lines where line_id=? and (lflg & (1 << 23))!=0;

-- ��А��H���ꗗ�𓾂���
select * from t_line where rowid in (select line_id from t_lines where (lflg & (1 << 23))!=0);
  -- or
select * from t_line n where exists (select * from t_lines l where l.line_id=n.rowid and (lflg & (1 << 23))!=0);



#!python3.0.1
# -*- coding: utf-8 -*-


#  sql 実行テスト用テンプレート


import sys
import os
import jrdb
import sqlite3

jrdb.con = sqlite3.connect('../jrdb2015.db')  # , isolation_level=None)


sql = """
select case when(select line_id from t_hzline where rowid=(
		select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?2)) > 0 then
    (select line_id from t_hzline where rowid=(
		select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?2))
     else 0 end,
    (select count(*) from t_lines where line_id=?1 and station_id=?2 and 0=(lflg&((1<<31)|(1<<17))))
	 union all
	 select distinct line_id, 0 from t_hzline h join (
		select (lflg>>19)&15 as x from t_lines
		where ((lflg>>19)&15)!=0 and (lflg&((1<<31)|(1<<17)))=0	and line_id=?1 and 
		case when (select sales_km from t_lines where line_id=?1 and station_id=?2)<
		          (select sales_km from t_lines where line_id=?1 and station_id=?3)
		then
		sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2) and
		sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3) 
		else
		sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2) and
		sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)
		end
	 order by
	 case when
	 (select sales_km from t_lines where line_id=?1 and station_id=?3) <
	 (select sales_km from t_lines where line_id=?1 and station_id=?2) then
	 sales_km
	 end desc,
	 case when
	 (select sales_km from t_lines where line_id=?1 and station_id=?3) >
	 (select sales_km from t_lines where line_id=?1 and station_id=?2) then
	 sales_km
	 end asc
	) as y on y.x=h.rowid 
	 union all
	 select case when(select line_id from t_hzline where rowid=(
		select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?3)) > 0 then
     (select line_id from t_hzline where rowid=(
		select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?3))
     else 0 end,   
     (select count(*) from t_lines where line_id=?1 and station_id=?3 and 0=(lflg&((1<<31)|(1<<17))));
"""



# result list
id1 = jrdb.line_id(sys.argv[1])
id2 = jrdb.station_id(sys.argv[2])
id3 = jrdb.station_id(sys.argv[3])
if id1 == 0 or id2 == 0:
	print("Error!! arguments.")
	exit(-1)

if id3 <= 0:
 id3 = -1
 
rslt = jrdb.sqlexec(sql, [ id1, id2, id3 ] )
if len(rslt) < 3:
	print("<<<", len(rslt), ">>>")

for inf in rslt:
	if len(inf) != 2:
		print("Error query:", len(inf))
		exit(-1)

	if inf[0] == 0 and inf[1] == 1:
		print("-1")

	lid = inf[0] & 0xffff
	sid = inf[0] >> 16
	print("{0}({1}) - {2}({3})  {4}".format(jrdb.line_name(lid), lid, jrdb.station_name(sid), sid, inf[1]))

"""
	if 0 < inf[1]:
		# 発／着
		if inf[0] <= 0:
			print("{0} \t{1}".format(inf[0], inf[1]))
		else:
			if 0x10000 <= inf[0]:
				h = inf[0] >> 16
				l = inf[0] & 0xffff
				if l <= 0:
					print("{0}/{1}({2})\t{3}".format(l, jrdb.station_name(h), h, inf[1]))
				else:
					print("{0}({1})/{2}({3})\t{4}".format(jrdb.line_name(l), l, jrdb.station_name(h), h, inf[1]))
			else:
				print("{0}({1}) \t{2}".format(jrdb.line_name(inf[0]), inf[0], inf[1]))
	else:
		# 中間
		if 0x10000 <= inf[0]:
			h = inf[0] >> 16
			l = inf[0] & 0xffff
			if l <= 0:
				print("|{0}/{1}({2})".format(l, jrdb.station_name(h), h))
			else:
				print("|{0}({1})/{2}({3})".format(jrdb.line_name(l), l, jrdb.station_name(h), h))
		else:
			print("|{0}({1})".format(jrdb.line_name(inf[0]), inf[0]))
"""
print("fin")


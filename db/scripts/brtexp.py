#!python3.0.1
# -*- coding: utf-8 -*-

"""
route mark
１つの線の２駅間に２駅間が重なる部分の距離を返す
まったく重なっていなければ、0を返す

"""
import sys
import os
import jrdb

sql1 = """
select ifnull(max(sales_km) - min(sales_km), 0), ifnull(max(calc_km) - min(calc_km), 0)
 from t_lines
 where line_id=?1 and sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)
                  and sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)
                  and sales_km>=(select min(sales_km)
                                  from t_lines
                                  where line_id=?1 and (station_id=?4 or station_id=?5))
                  and sales_km<=(select max(sales_km)
                                  from t_lines
                                  where line_id=?1 and (station_id=?4 or station_id=?5));
"""

sql2 = """
-- sql1を駅名でしめしすと、。。。
select name from t_station where rowid in (select station_id from t_lines where line_id=?1
and (sales_km=
(select min(sales_km)
 from t_lines
 where line_id=?1 and sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)
                  and sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)
                  and sales_km>=(select min(sales_km)
                                  from t_lines
                                  where line_id=?1 and (station_id=?4 or station_id=?5))
                  and sales_km<=(select max(sales_km)
                                  from t_lines
                                  where line_id=?1 and (station_id=?4 or station_id=?5))
) or sales_km=
(select max(sales_km)
 from t_lines
 where line_id=?1 and sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)
                  and sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)
                  and sales_km>=(select min(sales_km)
                                  from t_lines
                                  where line_id=?1 and (station_id=?4 or station_id=?5))
                  and sales_km<=(select max(sales_km)
                                  from t_lines
                                  where line_id=?1 and (station_id=?4 or station_id=?5))
)))
"""

# 範囲内ならOK、すこしでもはみだせばNG
sql3 = """
 select case when 2=count(*) then 1 else 0 end
 from t_lines
 where line_id=?1 and sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)
                  and sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)
                  and (station_id=?4 or station_id=?5)

"""

#実験用 他にもなんかいいやり方ない？？
sql4 = """
 select t1.station_id, t2.station_id
 from t_lines t1 join t_lines t2 on t1.line_id=t2.line_id and t1.station_id=t2.station_id
  where t1.line_id=?1 and t1.sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)
                  and t1.sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)
                  or t1.station_id=?4 or t2.station_id=?5

"""

id1 = jrdb.station_id("前谷地")
id2 = jrdb.station_id("柳津")
line_id = jrdb.line_id("気仙沼線(BRT)")

if sys.argv[1] == "1":
    sel_sql = sql1;
elif sys.argv[1] == "2":
    sel_sql = sql2;
elif sys.argv[1] == "3":
    sel_sql = sql3;
elif sys.argv[1] == "4":
    sel_sql = sql4;
else:
    print("arg error!!!")
    exit(-1)

sys.argv.pop(0) # shift

# result list
for inf in jrdb.sqlexec(sel_sql, [ line_id, id1, id2,
								jrdb.station_id(sys.argv[1]), jrdb.station_id(sys.argv[2]) ] ):
	#print(jrdb.station_name(inf[0]), inf[1])
    s = ""
    for r in inf:
        if s == "":
            s = s + str(r)
        else:
            s = s + ", " + str(r)

    print(s)


"""
[results]

気仙沼線BRT
前谷地 のの岳 柳津 志津川 本吉 気仙沼

mbthtl:scripts ntake$ python3 brtexp.py 志津川 気仙沼
0
mbthtl:scripts ntake$ python3 brtexp.py 志津川 本吉
0
mbthtl:scripts ntake$ python3 brtexp.py のの岳 前谷地
62
mbthtl:scripts ntake$ python3 brtexp.py 志津川 前谷地
175
mbthtl:scripts ntake$ python3 brtexp.py 柳津 前谷地
175


前谷地 0
和渕 32
のの岳 62
陸前豊里 103
御岳堂 136
柳津 175

"""

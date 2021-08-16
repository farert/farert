#!python3.0.1
# -*- coding: utf-8 -*-

import sys
import os
import jrdb

"""
会社線料金取得(全会社線の全駅を調べる-db整合性チェック)
Mar.22, 2019 created.
"""

def company_fare(stationid1, stationid2):
    fare_sql = """
    select fare from t_clinfar
     where
     ((station_id1=?1 and station_id2=?2) or
      (station_id1=?2 and station_id2=?1))
    """
    s = "%s %d-%d" % (fare_sql, stationid1, stationid2)
    j = jrdb.sqlexec(fare_sql, [stationid1, stationid2])
    try:
        a = int(j[0][0])
    except:
        a = 0

    return a


lines_sql = """
--select l.name, s.name from t_lines n join t_line l on l.rowid=n.line_id join t_station s on s.rowid=n.station_id where (lflg & (1<<18))!=0
select line_id, station_id from t_lines where (lflg & (1<<18))!=0 order by line_id, station_id
"""

compg = {}
for lin in jrdb.sqlexec(lines_sql, []):
    #s = "%s %s" % (jrdb.line_name(lin[0]), jrdb.station_name(lin[1]))
    #print(s)
    if lin[0] in compg:
        compg[lin[0]].append(lin[1])
    else:
        compg[lin[0]] = [lin[1]]

# compg['IGRいわて銀河']: ['盛岡', '好摩', '目時']


# 1,2    1,2
# 1,3    1,3
# 2,3    1,4
#        2,3
#        2,4
#        3,4
for key in compg.keys():
    for a in range(len(compg[key]) - 1):
        for b in range(len(compg[key]) - a - 1):
            #print("%d %d %d" % (key, a+1, b+a+2))
            fare = company_fare(compg[key][a], compg[key][b + a + 1])
            print("%s %s-%s %dyen" % (jrdb.line_name(key),
                                jrdb.station_name(compg[key][a]),
                                jrdb.station_name(compg[key][b + a + 1]),
                                fare))

"""
# result list
for inf in jrdb.sqlexec(sql, [ jrdb.station_id(sys.argv[1]), jrdb.station_id(sys.argv[2]) ] ):
		print(inf[0])
"""

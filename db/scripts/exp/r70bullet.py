#!python3.0.1
# -*- coding: utf-8 -*-

"""
t_r70bullet test
"""

import sys
import os
import jrdb


sql = """
select * from t_r70bullet
 where
 ((station_id1=?1 and station_id2=?2) or
  (station_id1=?2 and station_id2=?1)) and
   ((station70_id1=?3 and station70_id2=?4) or
    (station70_id1=?4 and station70_id2=?3));
"""


# result list
for inf in jrdb.sqlexec(sql, [ jrdb.station_id(sys.argv[1]), jrdb.station_id(sys.argv[2]),
								jrdb.station_id(sys.argv[3]), jrdb.station_id(sys.argv[4]) ] ):
	#print(jrdb.station_name(inf[0]), inf[1])
    s = ""
    for r in inf:
        if s == "":
            s = s + str(r)
        else:
            s = s + ", " + str(r)

    print(s)

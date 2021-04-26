#!python3.0.1
# -*- coding: utf-8 -*-

# Mar.17, 2012 created by N.Takeda.
#
# JR最短ルートの検索
# python jroute.py <発駅> <着駅>
#
#
#

import os
import sqlite3
import jrdb
import sys
import re
import time
from collections import defaultdict

if 2 != len(sys.argv):
  print("Usage:" + sys.argv[0] + " dbname.db")
  exit(-1)

def attributes(jctid1, jctid2):
    attr = 0
    station_id1 = jrdb.station_id_from_id(jctid1)
    station_id2 = jrdb.station_id_from_id(jctid2)

    if jrdb.is_densya_specific_line(station_id1, station_id2):
        attr = 1
                    # 電車をさきに訊かないと、拝島-八王子 などがあるので。
    if jrdb.is_local_line(station_id1, station_id2):
        attr = 2

    return attr


n = 0
items = []
for jct in jrdb.jct_list(): # 分岐駅リスト
	for node in jrdb.node_next(jct):   # 分岐駅の隣り合うノード長
		items.append([node[2], node[1], jct, node[0]]) # line, cost, jct, neer
		# print(jrdb.station_from_jctid(jct), jrdb.station_from_jctid(node[0]), node[1], jrdb.line_name(node[2]))
		n += 1
print(n, "affected.")

assert(n == len(items))

items.sort()

b_item = ""
items2 = []
for item in items:
	t_item = b_item.split('/')
	if t_item[0] == str(item[0]) and t_item[1] == str(item[1]) and \
	   t_item[3] == str(item[2]) and t_item[2] == str(item[3]):
		# 除外するもの　: (jct, neer),(neer, jct)があるので、重複経路は覗く
		#print(jrdb.station_from_jctid(item[2]), jrdb.station_from_jctid(item[3]), item[1], jrdb.line_name(item[0]))
		pass
	else:
		items2.append(item)
	b_item = '/'.join(map(str, item))

print("------------");
#for item in items2:
#	print(jrdb.station_from_jctid(item[2]), jrdb.station_from_jctid(item[3]), item[1], jrdb.line_name(item[0]))

###########################################
jrdb.con.execute("""
--if exists (select * from sqlite_master where type='table' and name='t_node')
drop table if exists t_node;
""")
###########################################
jrdb.con.execute("""
create table t_node (
	jct_id	integer not null references t_jct(id),
	neer_id	integer not null references t_jct(id),
	cost	integer not null,
	line_id	integer not null references t_line(rowid),
    attr    integer not null default (0),

	primary key (jct_id, neer_id, line_id)
	);
""")
###########################################
items = list(map(lambda x:[x[2], x[3], x[1], x[0], attributes(x[2], x[3])], items2))
items.sort()
# items = dict(zip(["jct", "neer", "cost", line_id"], items))
for item in items:
	print(jrdb.station_from_jctid(item[0]) + "\t" +
		  jrdb.station_from_jctid(item[1]) + "\t" +
		  str(item[2]) + "\t" + jrdb.line_name(item[3]))

jrdb.con.executemany('insert into t_node values(?, ?, ?, ?, ?)', items)
jrdb.con.commit()
print("registerd t_node {0} affected.".format(len(items)))
assert(len(items2) == len(items))

#!python3.0.1
# -*- coding: utf-8 -*-

import sqlite3
import os

# database
#con = sqlite3.connect('jr.db')  # , isolation_level=None)
try:
	dbpath = os.environ['farertDB']
except:
	dbpath = ''

con = sqlite3.connect(dbpath)  # , isolation_level=None)

#con.text_factory = '日本語'   # MAX-OSでUTF-８日本語を使用する場合必要(python2系では必要だがPython3系では日本語文字列返すクエリで"TypeError: 'str' object is not callable"になる)

# sql
#
def sqlexec(sqlstr, param):
  cur = con.cursor()
  cur.execute(sqlstr, param)
  return list(cur)


# 路線IDは新幹線か？
#
# True|False = f(line_id)
#
def is_shinkansen(line_id):
	return 0 < line_id and line_id <= 15  # 15:Maximum shinkansen ID


#	並行在来線を得る
#
#	lineId = getHZLine(line_id, station_id)
#
#	line_idは新幹線であること
#	lineIdは並行在来線、0は並行在来線はないことを示す
#
def getHZline(line_id, id_station, station_id2 = -1):
	cur = con.cursor()
	try:
	 cur.execute("select line_id from t_hzline where rowid=(select (lflg>>19)&15 from t_lines where line_id=? and station_id=?)", [line_id, id_station])
	 lid = cur.fetchone()[0]
	 if 0 < lid and lid < 32768:
	  return lid
	 cur.execute("""
	 select line_id from t_hzline where line_id<32767 and rowid in (
		select ((lflg>>19)&15) from t_lines where ((lflg>>19)&15)!=0 and line_id=?1 and
		case when (select sales_km from t_lines where line_id=?1 and station_id=?2)<
		          (select sales_km from t_lines where line_id=?1 and station_id=?3)
		then
		sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?2)
		else
		sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?2)
		end
	) limit(1);""", [line_id, id_station, station_id2]);
	 return cur.fetchone()[0]
	except:
	 return 0;


# 駅(station_id)の所属する路線IDを得る.
# 駅は非分岐駅で, 路線は1つしかないと解っていることを前提とする.
#
# line_id = f(station_id)
#
def line_from_station_id(id_station):
  cur = con.cursor()
  cur.execute("""
  select line_id
  from t_lines
  where station_id=?
  and 0<=sales_km
  and 0=(lflg & ((1 << 31)|(1 << 17)))""", [id_station])
  return cur.fetchone()[0]

# 駅ID(分岐駅)の最寄りの分岐駅を得る(全路線）
#
# [neer_jct_id, calc_km, line_id][N] = f(jct_id)
#
def node_next(jct_id):
  nodes = []
  sql = """
select 	id,
 (select case when calc_km>0 then calc_km else sales_km end
  from t_jct j left join t_lines l on j.station_id=l.station_id
  where line_id=?1 and id=?2 and 0<=sales_km and 0=(lflg&((1<<31)|(1<<17))))-case when calc_km>0 then calc_km else sales_km end as cost
from 	t_jct j left join t_lines l on j.station_id=l.station_id
where 	line_id=?1
and 	sales_km=(select max(sales_km)
				  from t_jct j left join t_lines l on j.station_id=l.station_id
				  where line_id=?1 and 0=(lflg&((1<<31)|(1<<17)))
				  and 0<=sales_km and sales_km<
				  (select sales_km from t_jct j left join t_lines l on
				   j.station_id=l.station_id where line_id=?1 and id=?2
				   and 0<=sales_km and 0=(lflg&((1<<31)|(1<<17)))))
union
select id, case when calc_km>0 then calc_km else sales_km end -
 (select case when calc_km>0 then calc_km else sales_km end
  from t_jct j left join t_lines l on j.station_id=l.station_id
  where 0<=sales_km and line_id=?1 and id=?2 and 0=(lflg&((1<<31)|(1<<17)))) as cost
from 	t_lines l join t_jct j on j.station_id=l.station_id
where 	line_id=?1
and 	sales_km=(select min(sales_km)
				  from t_jct j left join t_lines l on j.station_id=l.station_id
				  where line_id=?1 and 0=(lflg&((1<<31)|(1<<17)))
				  and 0<=sales_km
				  and sales_km>(select sales_km
						  		from t_jct j left join t_lines l on j.station_id=l.station_id
						  		where line_id=?1 and 0=(lflg&((1<<31)|(1<<17)))
						  		and id=?2 and 0<=sales_km));
"""
  cur = con.cursor()
  cur2 = con.cursor()

  cur.execute("""
  select line_id
  from t_lines l left join t_jct j on j.station_id=l.station_id
  where id=?                and 0<=sales_km and 0=(lflg&((1<<31)|(1<<17)))""", [jct_id])
  for lin in cur:
    cur2.execute(sql, [lin[0], jct_id])
    for st in cur2:
      #if not is_shinkansen(lin[0]) or (0 == getHZline(lin[0], station_id_from_id(st[0])) and 0 == getHZline(lin[0], station_id_from_id(jct_id))):
      # if is_shinkansen(lin[0]):
      #   print("$", line_name(lin[0]), station_from_jctid(st[0]), station_from_jctid(jct_id))
       nodes.append([st[0], st[1], lin[0]]) # neer_jct, cost, line_id
  return nodes


# 駅ID(分岐駅)の最寄りの分岐駅を得る(全路線）
# 詳細はnode_list.py
# [jct_id, calc_km, line_id][N] = f(jct_id)
#
def node_next_db(jct_id):
  cur = con.cursor()
  cur.execute("""
	select case jct_id when ?1 then neer_id else jct_id end, cost, line_id
	from t_node
	where jct_id=?1 or neer_id=?1
	""", [jct_id])
  return list(cur)

# 駅ID(非分岐駅前提)から両隣の分岐駅とそこまでの計算キロを得る
#
#  [station_id, calc_km][2 or 1]
#
def neer_node(id_station):
	sql = """
select 	station_id , abs((
	select case when calc_km>0 then calc_km else sales_km end
	from t_lines
	where 0<=sales_km and 0=(lflg&((1<<31)|(1<<17)))
	and line_id=(select line_id
				 from	t_lines
				 where	station_id=?1
				 and	0<=sales_km
				 and	0=(lflg&((1<<31)|(1<<17))))
	and station_id=?1)-case when calc_km>0 then calc_km else sales_km end) as cost
from 	t_lines
where 	0<=sales_km and 0=(lflg&((1<<31)|(1<<17)))
 and	line_id=(select	line_id
 				 from	t_lines
 				 where	station_id=?1
 				 and	0<=sales_km
 				 and	0=(lflg&((1<<31)|(1<<17))))
 and	sales_km in ((select max(y.sales_km)
					  from	t_lines x left join t_lines y
					  on	x.line_id=y.line_id
					  where	0<=x.sales_km and 0=(x.lflg&((1<<31)|(1<<17)))
					  and	0<=y.sales_km and (1<<15)=(y.lflg&((1<<31)|(1<<17)|(1<<15)))
					  and	x.station_id=?1
					  and	x.sales_km>y.sales_km
					 ),
					 (select min(y.sales_km)
					  from	t_lines x left join t_lines y
					  on	x.line_id=y.line_id
					  where 0<=x.sales_km and 0=(x.lflg&((1<<31)|(1<<17)))
					  and	0<=y.sales_km and (1<<15)=(y.lflg&((1<<31)|(1<<17)|(1<<15)))
					  and	x.station_id=?1
					  and	x.sales_km<y.sales_km));
"""
	result = []
	cur = con.cursor()
	cur.execute(sql, [id_station])
	for st in cur:
		result.append([st[0], st[1]])

	return result

#

# 路線IDの駅1から駅2の営業キロ、計算キロを得る
# 駅1、駅2は路線にあること
#
# [sales_km, calc_km] = f(line_id, station_id1, station_id2)
#
def distance_station_between(line_id, station_id1, station_id2):
  sql = """
select l1.sales_km - l2.sales_km, l1.calc_km - l2.calc_km
from t_lines l1
join t_lines l2
where l1.line_id=?1 and l2.line_id=l1.line_id
and l1.sales_km>l2.sales_km
and ((l1.station_id=?2 and l2.station_id=?3)
or (l1.station_id=?3 and l2.station_id=?2))
"""
  cur = con.cursor()
  cur.execute(sql, [line_id, station_id1, station_id2])
  try:
   return cur.fetchone()
  except:
   return [];


# 駅名より駅IDを返す
#
# station_id = f("駅名")
#
def station_id(station_s):
  if 0 <= station_s.find('('):
    name = station_s[:station_s.find('(')]
    samename = station_s[station_s.find('('):]
  else:
  	name = station_s
  	samename = ''

  cur = con.cursor()
  cur.execute("select rowid from t_station where name=? and samename=?", [name, samename])
  try:
   return cur.fetchone()[0]
  except:
   return 0;

# 駅IDより駅名を返す
#
# "駅名" = f(station_id)
#
def station_name(id_station):
  cur = con.cursor()
  cur.execute("select name from t_station where rowid=?", [id_station])
  try:
   return cur.fetchone()[0]
  except:
   return "";

# 路線IDより路線名を返す
#
# "路線名" = f(line_id)
#
def line_name(line_id):
  cur = con.cursor()
  cur.execute("select name from t_line where rowid=?", [line_id])
  try:
   return cur.fetchone()[0]
  except:
   return "";

# 路線名より路線IDを返す
#
# line_id = f("路線名")
#
def line_id(line_name):
  cur = con.cursor()
  cur.execute("select rowid from t_line where name=?", [line_name])
  try:
   return cur.fetchone()[0]
  except:
   return "";

# 分岐IDより駅名を返す
#
# "駅名" = f(jct_id)
#
def station_from_jctid(id):
  cur = con.cursor()
  cur.execute("select t.name from t_jct j join t_station t on j.station_id=t.rowid where j.id=?", [id])
  try:
   return cur.fetchone()[0]
  except:
   return "";


# 駅IDより分岐ID(1 to 30x)を返す(駅が非分岐駅なら0を返す)
#
# jct_id = f(station_id)
#
def id_from_station_id(id_station):
  cur = con.cursor()
  cur.execute("select id from t_jct where station_id=?", [id_station])
  try:
   return cur.fetchone()[0]
  except:
   return 0;

# 駅名から分岐駅ID(1 to 30x)を返す(駅が非分岐駅なら0を返す)
#
# jct_id = f("駅名")
#
def id_from_station(station):
  if 0 <= station.find('('):
    name = station[:station.find('(')]
    samename = station[station.find('('):]
  else:
  	name = station
  	samename = ''

  cur = con.cursor()
  cur.execute("""
  select id
  from t_jct j join t_station t on t.rowid=j.station_id
  where t.name=?
  and t.samename=?""", [name, samename])
  try:
   return cur.fetchone()[0]
  except:
   return 0;

# 分岐駅IDから駅IDを得る
#
# station_id = f(jct_id)
#
def station_id_from_id(id):
  cur = con.cursor()
  cur.execute("select station_id from t_jct where id=?", [id])
  return cur.fetchone()[0]
  # throw fatal error


# 分岐駅の総数を返す
#
# n = f()
#
def n_jct():
  cur = con.cursor()
  cur.execute("select count(*) from t_jct")
  return cur.fetchone()[0]


# 分岐ID一覧を返す
#
#
def jct_list():
  cur = con.cursor()
  cur.execute("select id from t_jct")
  return list(map(lambda x:x[0], cur))


# 地方交通線？
#
def is_local_line(station_id1, station_id2):
  sql = """
  select a.line_id from t_lines a join t_lines b on a.line_id=b.line_id
  	where a.station_id=? and b.station_id=?
  """
  cur = con.cursor()
  cur.execute(sql, [station_id1, station_id2])
  line_id = cur.fetchone()[0]
  distance = distance_station_between(line_id, station_id1, station_id2)
  return 0 < distance[1]

# 東京or大阪 特定電車区間?

def is_densya_specific_line(station_id1, station_id2):
  sql = """
  select sum(sflg>>10 & 3) from t_station where rowid=? or rowid=?
  """
  cur = con.cursor()
  cur.execute(sql, [station_id1, station_id2])
  c = cur.fetchone()[0]
  return 2 == c or 4 == c


if __name__ == "__main__":   # not module
	pass

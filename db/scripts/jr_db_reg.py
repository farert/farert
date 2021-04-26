#!python3.0.1
# -*- coding: utf-8 -*-

"""

データベース登録

Copyright sutezo9@me . com Allrightreserve. Jun.2011 - 2012,2013.

Copyright (C) 2014 Sutezo (sutezo666@gmail.com)

   'Farert' is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    'Farert' is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 'Farert'.  If not, see <http://www.gnu.org/licenses/>.

このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェ
ア財団によって発行された GNU 一般公衆利用許諾契約書(バージョン3か、希
望によってはそれ以降のバージョンのうちどれか)の定める条件の下で再頒布
または改変することができます。

このプログラムは有用であることを願って頒布されますが、*全くの無保証*
です。商業可能性の保証や特定の目的への適合性は、言外に示されたものも含
め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。

あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部
受け取ったはずです。もし受け取っていなければ、フリーソフトウェア財団ま
で請求してください

"""
import os
import sqlite3
import sys
import re
from collections import defaultdict

import platform

if platform.system() == 'Windows':
	ENCODE = 'shift_jisx0213'
else:
	ENCODE = 'utf-8'

COL_COMMENT = 0
COL_PREFECT = 0
COL_COMPANY = 1
COL_LINE = 2
COL_LINE_KANA = 3
COL_STATION = 4
COL_STATION_KANA = 5
COL_SALES_KM = 6
COL_CALC_KM = 7
COL_JUNCTION = 8
COL_SAME_STATION = 9
COL_SPE_CITY = 10
COL_SHINKTRSALW = 11
COL_LOCAL_AREA = 12
COL_METRO = 13
COL_YAMATE = 14
COL_RULE70 = 15
COL_SHINZAI = 16
COL_COMPANY_BORDER = 17
COL_RULE69 = 18
COL_SHINZAI_JCT = 19
COL_SPE_JCT = 20
COL_NUM = 21

def IntValue(txt):
	a = 0
	try:
		a += int(txt)
	except:
		a = 0

	return a

def same_staion(station_name):
	if 0 <= station_name.find('('):
		name = station_name[:station_name.find('(')]		# 同名駅がある駅の場合
		same = station_name[station_name.find('('):]
	else:
		name = station_name								# 同名駅でない場合
		same = ''
	return (name, same)

if 3 == len(sys.argv):
  fn = sys.argv[1]
  db_name = sys.argv[2]
else:
  print("Usage:" + sys.argv[0] + " dbname.txt name")
  exit(-1)

class Dbreg:

	def __init__(self, dbname):
		if os.access(dbname, os.F_OK):
			os.unlink(dbname)

		self.con = sqlite3.connect(dbname)  # , isolation_level=None)
		self.cur = self.con.cursor()

		self.n_station = 0
		self.branch = []
		self.num_of_line = 0
		self.tax1 = 0	# for t_clinfar
		self.tax2 = 0	# for t_clinfar


	def create_tables(self):
		###########################################
		sql = """
		create table t_global (
			max_station integer not null,
			max_line integer not null,
			max_jct integer not null,
			max_station_chr integer not null,
			max_line_chr integer not null,
			max_compnpass integer not null
		);
		"""
		self.con.execute(sql)
		self.con.execute("insert into t_global values(0, 0, 0, 0, 0, 0);")
		###########################################
		sql = """
		create table t_coreareac (
			name text not null primary key
		);
		"""
		self.con.execute(sql)
		###########################################
		sql = """
		create table t_dbsystem(
			name text,
			db_createdate text not null
		);
		"""
		self.con.execute(sql)
		self.con.executemany("insert into t_dbsystem values(?, datetime('now'))", [[db_name]])
		###########################################
		sql = """
		create table t_company (
			name text not null primary key
		);
		"""
		self.con.execute(sql)
		###########################################
		sql = """
		create table t_prefect(
			name text not null primary key
		);
		"""
		self.con.execute(sql)
		###########################################
		sql = """
		create table t_line(
			name text not null primary key,
            kana text not null
		);
		"""
		self.con.execute(sql)
		###########################################
		sql = """
		create table t_station(
			name text not null,
			kana text not null,
			company_id integer not null references t_company(rowid),
			prefect_id integer not null references t_prefect(rowid),
			samename text not null default(''),
			sflg integer not null,
			sub_company_id integer not null default(0),

			primary key(name, samename)
		);
		"""
		self.con.execute(sql)
		###########################################
		sql = """
		create table t_lines (
			line_id 	integer not null references t_line(rowid),
			station_id 	integer not null references t_station(rowid),
			sales_km	integer not null,
			calc_km		integer not null,
			lflg		integer not null default(0),

			primary key (line_id, station_id)
		);
		"""
		self.con.execute(sql)
		###########################################
		sql = """
		create table t_jct (
			id integer primary key autoincrement,
			station_id 	integer not null unique references t_station(rowid)
		);
		"""
		self.con.execute(sql)
		###########################################
		sql = """
		create table t_clinfar (
			station_id1 integer not null,
			station_id2 integer not null,
			fare integer not null,
			academic integer not null default(0),
			flg integer not null default(0),
			tax integer not null,
			-- line_id integer not null references t_line(rowid),
			primary key (station_id1, station_id2, tax)
		);
		"""
		self.con.execute(sql)
		###########################################
		self.con.execute("""
		create table t_rule69 (
			id		integer not null,
			station_id1 integer not null references t_station(rowid),
			station_id2 integer not null references t_station(rowid),
			line_id integer not null references t_line(rowid),
			ord	integer not null default(0),

			primary key (id, station_id1)
		);
		""")
		###########################################
		self.con.execute("""
		create table t_rule70 (
			station_id1 integer not null references t_station(rowid),
			station_id2 integer not null references t_station(rowid),
			sales_km integer not null,

			primary key (station_id1, station_id2)
		);
		""")
		###########################################
		self.con.execute("""
		create table t_r70bullet (
			station70_id1 integer not null references t_station(rowid),
			station70_id2 integer not null references t_station(rowid),
			station_id1 integer not null references t_station(rowid),
			station_id2 integer not null references t_station(rowid),

			primary key (station70_id1, station70_id2, station_id1, station_id2)
		);
		""")
		###########################################
		self.con.execute("""
		create table t_rule86 (
			line_id1 integer not null references t_line(rowid),
			station_id integer not null references t_station(rowid),
			line_id2 integer not null references t_line(rowid),
			city_id integer not null,

			primary key (line_id1, city_id));
		""")
		###########################################
		self.con.execute("""
		create table t_farebspekm (
			km	integer primary key,
			ha	integer not null,
			sa	integer not null,
			ka	integer not null,
			h8	integer not null,
			s8	integer not null,
			k8	integer not null,
			h5	integer not null,
			s5	integer not null,
			k5	integer not null);
		""")
		###########################################
		self.con.execute("""
		create table t_farelspekm (
			km	integer primary key,
			ckm	integer not null,
			ea	integer not null,
			ha	integer not null,
			e8	integer not null,
			h8	integer not null,
			e5	integer not null,
			h5	integer not null);
		""")
		###########################################
		self.con.execute("""
		create table t_farels (
			dkm	integer primary key,
			skm	integer not null,
			sa	integer not null,
			ka	integer not null,
			s8	integer not null,
			k8	integer not null,
			s5	integer not null,
			k5	integer not null);
		""")
		###########################################
		self.con.execute("""
		create table t_fareadd (
			km	integer primary key,
			ha5	integer not null,
			sa5	integer not null,
			ka5	integer not null,
			ha8	integer not null,
			sa8	integer not null,
			ka8	integer not null,
			haa	integer not null,
			saa	integer not null,
			kaa	integer not null
			);
		""")
		###########################################
		self.con.execute("""
		create table t_farehla (
			km	integer primary key,
			ha5	integer not null,
			ha8	integer not null)
		""")
		###########################################
		self.con.execute("""
		create table t_farespp (
			station_id1	integer not null references t_station(rowid),
			station_id2	integer not null references t_station(rowid),
			fare10p		integer not null,
			fare8p		integer not null,
			fare5p		integer not null,
			kind		integer not null,

			primary key (station_id1, station_id2, kind)
		);
		""")
		###########################################
		#self.con.execute("""
		#create table t_hzline(
		#	line_id	integer not null references t_line(rowid)
		#);
		#""")
		self.con.execute("""
		create table t_hzline(
			line_id	integer not null
		);
		""")
		###########################################
		self.con.execute("""
		create table t_jctspcl(
			id integer primary key autoincrement,
			type integer not null,
			jctsp_line_id1 integer not null,
			jctsp_station_id1 integer not null,
			jctsp_line_id2 integer not null default(0),
			jctsp_station_id2 integer not null default(0)
		);
		""")
		###########################################
		self.con.execute("""
		create table t_compnpass(
			station_id1 integer not null default(0),
			station_id2 integer not null default(0),
			en_line_id integer not null default(0),
			en_station_id1 integer not null default(0),
			en_station_id2 integer not null default(0)
		);
		""")
		###########################################
		self.con.execute("""
		create table t_compnconc(
			station_id	integer primary key,
			pass		integer not null default(1)
		);
		""")
		###########################################
		self.con.execute("""
			create table t_brtsp(
				line_id integer not null,
				station_id1 integer not null,
				station_id2 integer not null,
				type integer not null default(0),

				primary key (line_id, station_id1, station_id2)
			);
		""")
		###########################################

	#n_line_Index_of_CompanyLine = 0
	#n_line_Index_of_Shinkansen = 0


	def first_regist(self):
		#県
		prefects = [
		['北海道'], ['青森県'], ['秋田県'], ['岩手県'], ['山形県'], ['宮城県'], ['福島県'], ['新潟県'],
		['栃木県'], ['群馬県'], ['茨城県'], ['千葉県'], ['埼玉県'], ['東京都'], ['神奈川県'], ['静岡県'],
		['山梨県'], ['長野県'], ['岐阜県'], ['富山県'], ['福井県'], ['石川県'], ['愛知県'], ['三重県'],
		['滋賀県'], ['京都府'], ['大阪府'], ['和歌山県'], ['奈良県'], ['兵庫県'], ['鳥取県'], ['島根県'],
		['岡山県'], ['広島県'], ['山口県'], ['香川県'], ['徳島県'], ['愛媛県'], ['高知県'], ['福岡県'],
		['佐賀県'], ['長崎県'], ['大分県'], ['熊本県'], ['宮崎県'], ['鹿児島県']]
		self.con.executemany('insert into t_prefect values(?)', prefects)
		print("registered t_prefect {0} affected.".format(len(prefects)))

		coreAreaNames = [
						['東京都区内[区]'],
						['横浜市内[浜]'],
						['名古屋市内[名]'],
						['京都市内[京]'],
						['大阪市内[阪]'],
						['神戸市内[神]'],
						['広島市内[広]'],
						['北九州市内[九]'],
						['福岡市内[福]'],
						['仙台市内[仙]'],
						['札幌市内[札]'],
						['山手線内[山]'],
						['大阪・新大阪']]
		self.con.executemany('insert into t_coreareac values(?)', coreAreaNames)
		print("registered t_coreareac {0} affected.".format(len(coreAreaNames)))

		###########
		# 1 pass
		###########
		h_items = [defaultdict(int), defaultdict(lambda:"")]
		n_segment = 0

		self.num_of_line = 0
		for lin in open(fn, 'r', encoding=ENCODE):
			self.num_of_line += 1
			if lin[COL_COMMENT] == '#':
				continue		# コメントスキップ

			if lin.startswith('*line'):
				n_segment = 1
				continue
			elif lin.startswith('*'):
				break

			if not n_segment == 1:
				continue

			linitems = lin.split('\t')
			if linitems[COL_PREFECT] not in list(map(lambda x:x[0], prefects)):	# 都道府県
				continue

			for i in range(2):				# 会社(COL_PREFECT)、線区(COL_LINE)
				key = linitems[i + 1].strip();
				if i == 0 and 0 <= key.find('/'):		# 所属会社2つはシカト
					continue

				if i == 0:
					h_items[i][key] += 1
				else:	# i==1
					if not key in h_items[i]:
						if not linitems[COL_COMPANY].startswith("JR"):
							key += "|company|"		# 会社線
						h_items[i][key] = ""

					kana = linitems[COL_LINE_KANA].strip()
					if h_items[i][key] == "" and  kana != "":
						h_items[i][key] = kana

		prefects = None

		#会社
		self.con.executemany('insert into t_company values(?)', list(map(lambda x:[x], h_items[0].keys())))
		print("registered t_company {0} affected.".format(len(list(h_items[0].keys()))))

		#線区
		self.con.executemany('insert into t_line values(?, ?)', list(map(lambda x:[x, h_items[1][x]], h_items[1].keys())))
		print("registered t_line {0} affected.".format(len(list(h_items[1].keys()))))

		self.con.commit()

	def second_regist(self):
		###########
		# 2 pass
		###########

		# convert line_id
		#新幹線 0x1000~
		#会社線 0x2000~
		#BRT    0x4000~   BRTは、~0x4000 & id として使うので。

		self.con.execute("""
		update t_line set rowid=rowid + 0x1000 where name like '%新幹線';
		""")

		self.con.execute("""
		update t_line set rowid=rowid + 0x2000, name=substr(name, 1, length(name) - length('|company|')) where name like '%|company|';
		""")

		## begin exp
		cur = self.con.cursor()
		cur.execute("""
		select substr(name, 0, length("(BRT)")) from t_line where name like '%(BRT)'
		""")
		for c in cur:
			 lname = c[0]
			 self.con.execute("""
			 update t_line set rowid=(select rowid from t_line where name=?1)+0x4000 where name=?2
			 """, [lname, lname + "(BRT)"])
	   
		## end exp

		dispatch = {
			'line'			: self.reg_line,
			't_clinfar'		: self.reg_t_clinfar,
			't_rule69'		: self.reg_t_rule69,
			'rule70'		: self.reg_rule70,
			'r70bullet'		: self.reg_r70bullet,
			'rule86'		: self.reg_rule86,
			't_farebspekm'	: self.reg_t_farebspekm,
			't_farelspekm'	: self.reg_t_farelspekm,
			't_farels'		: self.reg_t_farels,
			't_fareadd'		: self.reg_t_fareadd,
			't_farespp'		: self.reg_t_farespp,
			't_farehla'		: self.reg_t_farehla,
			't_compnpass'	: self.reg_t_compnpass,
			't_compnconc'	: self.reg_t_compnconc,
			't_brtsp'       : self.reg_t_brtsp,
		}

		self.num_of_line = 0
		proc_stage = ''
		for lin in open(fn, 'r', encoding=ENCODE):
			self.num_of_line += 1
			if lin[0] == '#':
				continue		# コメントスキップ

			if lin.strip() == '':
				continue		# 空行＝コメントスキップ

			if lin.startswith("//end"):
				proc_stage = ''
				continue		# //end は、ブロック終了

			linitems = lin.split('\t')

			if lin.startswith('*'):
				proc_stage = re.match(r'\w+', linitems[COL_COMMENT][1:]).group(0)

			elif proc_stage in dispatch:
				dispatch[proc_stage](proc_stage, linitems, lin)

		self.reg_last_line()

	#
	#
	#
	#
	def reg_line(self, label, linitems, lin):
	# 路線
			# 0:路線、1:駅、2:分岐駅、3:営業キロ、4:分岐路線、5:同名駅, 6:分岐路線2/分岐駅2, 7:lflg
		if 0 <= linitems[COL_COMPANY].find("branch"):
			self.branch.append([linitems[COL_LINE].strip(), linitems[COL_STATION].strip(), linitems[COL_STATION_KANA].strip(),
						   linitems[COL_SALES_KM].strip(), linitems[COL_CALC_KM].strip(), linitems[COL_SAME_STATION].strip(),
						   linitems[COL_JUNCTION].strip(), 0 if linitems[COL_JUNCTION].strip() == '' else (1 << 15)])
			return			# 分岐特例はあとで

		if linitems[COL_STATION].strip().startswith("-"):
			return

		if COL_NUM != len(linitems):
			print(self.num_of_line, lin)
			raise ValueError

##debug################################################
#		print(self.num_of_line, lin)
######################################################

		# BSRJCTFLG: 分岐フラグ
		tmp = linitems[COL_JUNCTION].strip()
		sflg = 0
		lflg = 0
		if (tmp == 'x'):
			sflg = (1 << 14)	 	# jctflg
		elif (tmp != ''):
			lflg = (1 << 15)		# jctflg

		# 規則69条 t_rule69.id
		# BSR69NOMASK, BSR69TERM, BSR69CONT
		tmp = int(linitems[COL_RULE69])
		if 0 < tmp:
			if 100 <= tmp:
				lflg |= (1 << 23)		# BSR69CONT
				tmp -= 100
			lflg |= (1 << 24)			# BSR69TERM
			lflg |= ((0x0f & tmp) << 0)	# BSR69NOMASK(20-17)


		# BSRBORDER: 本州3社、3島会社境界駅
		tmp = int(linitems[COL_COMPANY_BORDER])
		if 0 < tmp:
			lflg |= (1 << 16)

		# BSRVIRJCT: 新幹線で新幹線の駅の無い在来線分岐駅
		tmp = int(linitems[COL_SHINZAI_JCT])
		if 0 < tmp:
			lflg |= (1 << 17)

		# BSRHZLIN: 新幹線＜－＞並行在来線乗換
		tmp = linitems[COL_SHINZAI].strip()
		if tmp[0] != '!' and tmp != "":
			if tmp.startswith('-'):
				tmp = tmp[1:]
				ix = tmp.find('/')   # 路線あり
				if 0 <= ix:
					stationId = tmp[:ix]
					lineId = tmp[ix + 1:]  # 路線(長岡における信越線(直江津-新潟)とか）
				else:
					lineId = ""
					stationId = tmp

				#print("####{0}, {1}###".format(stationId, lineId));
				# bit16-13	境界駅=高崎
				if lineId == "":
					self.cur.execute('select rowid from t_hzline where line_id=(select 65536*rowid from t_station where name=?)', [stationId])
					row = self.cur.fetchone()
					if None == row:
						self.con.execute('insert into t_hzline (line_id) values((select 65536*rowid from t_station where name=?))', [stationId])
						self.cur.execute('select rowid from t_hzline where line_id=(select 65536*rowid from t_station where name=?)', [stationId])
						row = self.cur.fetchone()
				else:
					self.cur.execute(
					'select rowid from t_hzline where line_id=(select (select rowid from t_line where name=?1) + (select rowid * 65536 from t_station where name=?2))', [lineId, stationId])
					row = self.cur.fetchone()
					if None == row:
						self.con.execute(
						'insert into t_hzline values((select (select rowid from t_line where name=?1) + (select rowid * 65536 from t_station where name=?2)))', [lineId, stationId])
						self.cur.execute(
						'select rowid from t_hzline where line_id=(select (select rowid from t_line where name=?1) + (select rowid * 65536 from t_station where name=?2))', [lineId, stationId])
						row = self.cur.fetchone()
			else:
				self.cur.execute('select rowid from t_hzline where line_id=(select rowid from t_line where name=?)', [tmp])
				row = self.cur.fetchone()
				if None == row:
					self.con.execute('insert into t_hzline values((select rowid from t_line where name=?))', [tmp])
					self.cur.execute('select rowid from t_hzline where line_id=(select rowid from t_line where name=?)', [tmp])
					row = self.cur.fetchone()

			tmp = row[0]
			tmp &= 0x0f
			lflg |= (tmp << 19)		# BSRHZLIN: bit16-13 -> 22-19

		# BCRULE70:規則70条
		tmp = int(linitems[COL_RULE70])
		if 0 != tmp:
			sflg |= (1 << 6)

		# BCYAMATE
		tmp = int(linitems[COL_YAMATE])
		if 0 != tmp:
			sflg |= (1 << 5)

		# BCOSMSP, BCTKMSP
		tmp = int(linitems[COL_METRO])
		if 1 == tmp:
			sflg |= (1 << 10)	# BCTKMSP: 東京電車特定区間
		elif 2 == tmp:
			sflg |= (1 << 11)	# BCOSMSP: 大阪電車特定区間

		# BCSUBURB: 近郊区間
		tmp = int(linitems[COL_LOCAL_AREA])
		if (10 < tmp):	# 新幹線を含む近郊区間
			sflg |= (1 << 13)
			tmp -= 10

		tmp &= 0x07
		sflg |= (tmp << 7)	# bit9-7

		# BSRSHINKTRSALW: 新幹線、在来線乗換
		# (新幹線のみ)
		tmp = int(linitems[COL_SHINKTRSALW])
		if 10 <= tmp and tmp <= 13:
#140516			if 0 != int(linitems[COL_COMPANY_BORDER]):			###!!!なんの意味があったのか？
#				print(self.num_of_line, lin)
#				raise ValueError
			#lflg |= ((1 << 27) | ((tmp - 10) << 25))
			lflg |= ((tmp - 10) << 19)

		# BCCITYCT, BCCITYNO
		tmp = int(linitems[COL_SPE_CITY])
		if 100 <= tmp:
			sflg |= (1 << 4)	# BCCITYCT
			tmp -= 100

		tmp &= 0x0f
		sflg |= (tmp << 0)	# BCCITYNO: bit3-0

		# BSRCOMPANY
		# (会社線)
		#if not linitems[COL_COMPANY].strip().startswith("JR"):
		#	lflg |= (1 << 18)

		# BSRJCTSP_B: 分岐特例
		tmp = linitems[COL_SPE_JCT].strip()
		if tmp[0] != '!' and tmp != "":
			tmps = tmp.split('/')
			while len(tmps) < 5:
				tmps.append('')

			if int(tmps[0]) < 1 or 5 < int(tmps[0]):  # あえてisdigit()を使わず例外スローさせる
				print(self.num_of_line, lin)
				raise ValueError

			self.con.execute("""
			insert into t_jctspcl(type, jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2) values(
			?1,
			(select case when (select rowid from t_line where name=?2) is null then 0
			else              (select rowid from t_line where name=?2) end),
			(select case when (select rowid from t_station where name=?3 and samename=?4) is null then 0
			else              (select rowid from t_station where name=?3 and samename=?4) end),
			(select case when (select rowid from t_line where name=?5) is null then 0
			else              (select rowid from t_line where name=?5) end),
			(select case when (select rowid from t_station where name=?6 and samename=?7) is null then 0
			else              (select rowid from t_station where name=?6 and samename=?7) end))
			""",
			[tmps[0], tmps[1], *same_staion(tmps[2]),
			 tmps[3], *same_staion(tmps[4])])

			self.cur.execute('select seq from sqlite_sequence where name=\'t_jctspcl\'')
			lflg &= 0xffffff00
			lflg |= (0xff & self.cur.fetchone()[0])
			lflg |= (1 << 29)

		self.cur.execute('select rowid from t_prefect where name=?', [linitems[COL_PREFECT].strip()])	# retrive 都道府県id
		prefect_id = self.cur.fetchone()[0]

		company_name = linitems[COL_COMPANY].strip().split('/')[0]
		self.cur.execute('select rowid from t_company where name=?', [company_name])	# retrive 会社id
		company_id = self.cur.fetchone()[0]

		if 0 < linitems[COL_COMPANY].strip().find('/'):
			company_name = linitems[COL_COMPANY].strip().split('/')[1]
			self.cur.execute('select rowid from t_company where name=?', [company_name])	# retrive 会社id
			sub_company_id = self.cur.fetchone()[0]
		else:
			sub_company_id = 0

		self.cur.execute('select rowid from t_line where name=?', [linitems[COL_LINE].strip()])		# retrive 路線id
		line_id = self.cur.fetchone()[0]

		station_name = linitems[COL_STATION].strip()
		samename = linitems[COL_SAME_STATION].strip()
		self.cur.execute('select rowid from t_station where name=? and samename=?', [station_name, samename])
		row = self.cur.fetchone()
		if None != row:
			station_id = row[0]			# 登録済み
		else:
			self.con.execute('insert into t_station values(?, ?, ?, ?, ?, ?, ?)', \
						[ station_name, linitems[COL_STATION_KANA].strip(), company_id, prefect_id, \
						  samename, sflg, sub_company_id])
			self.cur.execute('select rowid from t_station where name=? and samename=?', [station_name, samename])
			station_id = self.cur.fetchone()[0]
			self.n_station += 1

		sales_km = int(float(linitems[COL_SALES_KM]) * 10)
		calc_km = int(float(linitems[COL_CALC_KM]) * 10)

		self.con.execute('insert into t_lines values(?, ?, ?, ?, ?)', [line_id, station_id, sales_km, calc_km, lflg] )

#------------------------------------------------------------------------------
	def reg_t_clinfar(self, label, linitems, lin):
# 料金表 各路線(N-1)*N/2 N=駅数
		tmp = linitems[0].strip()
		if tmp == "taxValue":
			self.tax1 = int(linitems[1])
			self.tax2 = int(linitems[2])
			return

		station1 = same_staion(tmp)
		station2 = same_staion(linitems[1].strip())

		sql = "insert into " + label + """
 values((select rowid from t_station where name=? and samename=?),
 		(select rowid from t_station where name=? and samename=?), ?, ?, ?, ?);
"""
		if 0 < int(linitems[6]):  # 小児
			tmp = 1
		else:
			tmp = 0

		if 0 < int(linitems[7]):  # 併算割引
			tmp |= 0x02

		self.con.execute(sql, [*station1, *station2, int(linitems[2]), int(linitems[4]), tmp, self.tax1])
		self.con.execute(sql, [*station1, *station2, int(linitems[3]), int(linitems[5]), tmp, self.tax2])
		#                                                                     fare            academic         flg
#------------------------------------------------------------------------------
	def reg_t_rule69(self, label, linitems, lin):
		# t_rule69
		ident = int(linitems[0])

		self.con.execute("""
insert into t_rule69(id, station_id1, station_id2, line_id, ord) values(
 ?,
 (select rowid from t_station where name=? and samename=?),
 (select rowid from t_station where name=? and samename=?),
 (select rowid from t_line where name=?), ?)""",
		 [ident, *same_staion(linitems[1].strip()), *same_staion(linitems[2].strip()), 
			  linitems[3], int(linitems[4])])

#------------------------------------------------------------------------------
	def reg_rule70(self, label, linitems, lin):
	# rule70

		self.con.execute("""
insert into t_rule70 values(
 (select rowid from t_station where name=? and samename=?),
 (select rowid from t_station where name=? and samename=?),
 ?);""",
		[*same_staion(linitems[0].strip()), *same_staion(linitems[1].strip()), int(linitems[2])])

#------------------------------------------------------------------------------
	def reg_r70bullet(self, label, linitems, lin):
	# rule70

		self.con.execute("""
insert into t_r70bullet values(
 (select rowid from t_station where name=? and samename=?),
 (select rowid from t_station where name=? and samename=?),
 (select rowid from t_station where name=? and samename=?),
 (select rowid from t_station where name=? and samename=?));""",
		[*same_staion(linitems[0].strip()), *same_staion(linitems[1].strip()), 
		 *same_staion(linitems[2].strip()), *same_staion(linitems[3].strip())])

#------------------------------------------------------------------------------

	def reg_rule86(self, label, linitems, lin):
	# rule86
		if linitems[0].strip() != "":
			if not db_name in linitems[0]:
				return
		#print(lin, end='')
		self.con.execute("""
insert into t_rule86 values(
 (select rowid from t_line where name=?),
 (select rowid from t_station where name=? and samename=?),
 (select rowid from t_line where name=?), ?);""",
		[linitems[1].strip(), *same_staion(linitems[2].strip()), linitems[3].strip(), int(linitems[4])])

#------------------------------------------------------------------------------
	def reg_t_farebspekm(self, label, linitems, lin):
	# t_farebspekm: 3島会社幹線例外
		self.con.execute("""
insert into t_farebspekm(km,h8,s8,k8,h5,s5,k5,ha,sa,ka) values(?,?,?,?,?,?,?,?,?,?)""",
		[int(linitems[0].replace(',', '')), int(linitems[1].replace(',', '')),
		 int(linitems[2].replace(',', '')), int(linitems[3].replace(',', '')),
		 int(linitems[4].replace(',', '')), int(linitems[5].replace(',', '')),
		 int(linitems[6].replace(',', '')), int(linitems[7].replace(',', '')),
		 int(linitems[8].replace(',', '')), int(linitems[9].replace(',', ''))])
#------------------------------------------------------------------------------
	def reg_t_farelspekm(self, label, linitems, lin):
	# t_farelspekm:地方交通線例外
		self.con.execute("""
insert into t_farelspekm(km,ckm,e8,h8,e5,h5,ea,ha) values(?, ?, ?, ?, ?, ?, ?, ?)""",
		[int(linitems[0].replace(',', '')), int(linitems[1].replace(',', '')),
		 int(linitems[2].replace(',', '')), int(linitems[3].replace(',', '')),
		 int(linitems[4].replace(',', '')), int(linitems[5].replace(',', '')),
		 int(linitems[6].replace(',', '')), int(linitems[7].replace(',', ''))])
#------------------------------------------------------------------------------
	def reg_t_farels(self, label, linitems, lin):
	# t_farels: 2島会社地方交通線
		if linitems[1].replace(',', '').isdigit():
			skm = int(linitems[1].replace(',', ''))
		else:
			skm = -1

		if linitems[2].replace(',', '').isdigit():
			k = int(linitems[2].replace(',', ''))
		else:
			k = -1

		if linitems[3].replace(',', '').isdigit():
			l = int(linitems[3].replace(',', ''))
		else:
			l = -1

		if linitems[4].replace(',', '').isdigit():
			m = int(linitems[4].replace(',', ''))
		else:
			m = -1

		if linitems[5].replace(',', '').isdigit():
			n = int(linitems[5].replace(',', ''))
		else:
			n = -1

		if linitems[6].replace(',', '').isdigit():
			ma = int(linitems[6].replace(',', ''))
		else:
			ma = -1

		if linitems[7].replace(',', '').isdigit():
			na = int(linitems[7].replace(',', ''))
		else:
			na = -1

		self.con.execute("""
insert into t_farels values(?, ?, ?, ?, ?, ?, ?, ?)""",
		[int(linitems[0].replace(',', '')), skm, ma, na, k, l, m, n])
# a, 8, 5
#------------------------------------------------------------------------------
	def reg_t_fareadd(self, label, linitems, lin):
	# t_fareadd: 本州3社+3島加算

		self.con.execute("insert into " + label + " values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
		[int(linitems[0].replace(',', '')), int(linitems[1].replace(',', '')),
		 int(linitems[2].replace(',', '')), int(linitems[3].replace(',', '')),
		 int(linitems[4].replace(',', '')), int(linitems[5].replace(',', '')),
		 int(linitems[6].replace(',', '')), int(linitems[7].replace(',', '')),
		 int(linitems[8].replace(',', '')), int(linitems[9].replace(',', '')) ])
		# km, ha5, sa5, ka5, ha8, sa8, ka8, haa, saa, kaa

#------------------------------------------------------------------------------
	def reg_t_farespp(self, label, linitems, lin):
		# t_farespp: 特定区間通過加算

		station1 = same_staion(linitems[0].strip())
		station2 = same_staion(linitems[1].strip())

		if int(db_name) <= 2017 and (station1[0] == "摩耶" or station2[0] == "摩耶"):
			return
		
		if int(db_name) < 2019 and (station1[0] == "あしかがフラワーパーク" or 
									 station2[0] == "あしかがフラワーパーク"):
			return

		kind = int(linitems[4].replace(',', ''))

		if db_name != "2014" or kind != 2:
			#print(lin, end='')
			self.con.execute("""
insert into t_farespp(station_id1, station_id2, fare10p, fare8p, fare5p, kind) values(
 (select rowid from t_station where name=? and samename=?),
 (select rowid from t_station where name=? and samename=?),
 ?, ?, ?, ?)""",
				[*station1, *station2,
				 int(linitems[5].replace(',', '')), int(linitems[2].replace(',', '')), int(linitems[3].replace(',', '')),
				 kind])
		# station_id1, station_id2, fare10p, fare8p, fare5p, kind

#------------------------------------------------------------------------------
	def reg_t_farehla(self, label, linitems, lin):
		# t_farehla : 本州3社+JR北地方交通線会社加算
		# t_farehla5p : 本州3社+JR北地方交通線会社加算(消費税5%版)

		self.con.execute("insert into " + label + " values(?, ?, ?)",
		[int(linitems[0].replace(',', '')), int(linitems[1].replace(',', '')), int(linitems[2].replace(',', ''))])
		# km, ha5, ha8
#------------------------------------------------------------------------------
	def reg_t_compnpass(self, label, linitems, lin):
		# t_compnpass
		# 会社線通過連絡運輸

		assert(label == "t_compnpass")

		# 路線 or 会社
		linename = linitems[2]
		en_line_id = 0
		if len(linename) <= 0:
			en_line_id = 0
		elif 'c' != linename[0]:
			# 路線
			try:
				self.cur.execute('select rowid from t_line where name=?', [linename])	# retrive 会社id
				en_line_id = self.cur.fetchone()[0]
			except:
				en_line_id = 0
		else:
			#会社
			en_line_id = 0
			for i in range(len(linename[1:])):
				cname = linename[1 + i]
				cc = { "北": "JR北海道", "東": "JR東日本", "海": "JR東海", "西": "JR西日本", "四": "JR四国","九": "JR九州"}
				try:
					cname = cc[cname]
					self.cur.execute('select rowid from t_company where name=?', [cname])	# retrive 会社id
					cn = self.cur.fetchone()[0]
					en_line_id |= (1 << cn)
				except:
					pass

				en_line_id |= 0x80000000

		##print(*same_staion(linitems[0].strip()), *same_staion(linitems[1].strip()), en_line_id, 
		##	 *same_staion(linitems[3].strip()), *same_staion(linitems[4].strip()))
		#              station_id1, station_id2, en_line_id, en_station_id1, en_station_id2
		# primary key (station_id1, station_id2, en_line_id)
		self.cur.execute("""
			insert into t_compnpass values(
							(select rowid from t_station where name=?1 and samename=?2),
							(select rowid from t_station where name=?3 and samename=?4), 
							?5,
							(select case when (select rowid from t_station where name=?6 and samename=?7) is null then 0
										 else (select rowid from t_station where name=?6 and samename=?7) end),
							(select case when (select rowid from t_station where name=?8 and samename=?9) is null then 0
										 else (select rowid from t_station where name=?8 and samename=?9) end)
							)""",
			[*same_staion(linitems[0].strip()), *same_staion(linitems[1].strip()), en_line_id, 
			 *same_staion(linitems[3].strip()), *same_staion(linitems[4].strip())])

#------------------------------------------------------------------------------
	def reg_t_compnconc(self, label, linitems, lin):
		# t_compnconc
		# 会社線乗継チェック

		assert(label == "t_compnconc")

		allow = int(linitems[1])
		self.cur.execute("""
			insert into t_compnconc values(
				(select rowid from t_station where name=? and samename=?), ?)""", 
		[*same_staion(linitems[0].strip()), allow])

#------------------------------------------------------------------------------
	def reg_t_brtsp(self, label, linitems, lin):
		# t_brtsp
		# BRT乗継チェック

		assert(label == "t_brtsp")

		type = int(linitems[0]) # type
		line = linitems[1].strip() # 路線

		# 駅1, 駅2は下りから並べないといけない。
		self.cur.execute("""
			insert into t_brtsp(station_id1, station_id2, type, line_id) values(
				(select station_id from t_lines where line_id=(select rowid from t_line where name=?6) and sales_km=
					(select min(sales_km) from t_lines where line_id=(select rowid from t_line where name=?6) and (
						station_id=
						(select rowid from t_station where name=?1 and samename=?2) or station_id=
						(select rowid from t_station where name=?3 and samename=?4)))),
				(select station_id from t_lines where line_id=(select rowid from t_line where name=?6) and sales_km=
					(select max(sales_km) from t_lines where line_id=(select rowid from t_line where name=?6) and (
						station_id=
						(select rowid from t_station where name=?1 and samename=?2) or station_id=
						(select rowid from t_station where name=?3 and samename=?4)))),
				?5, (select rowid from t_line where name=?6)
			)""",
			[*same_staion(linitems[2].strip()), *same_staion(linitems[3].strip()), type, line])

#------------------------------------------------------------------------------
	def reg_last_line(self):
		# 分岐特例
		for bitem in self.branch:
			#		self.branch.append([linitems[2].strip(), linitems[3].strip(), linitems[4].strip(),
			#					   linitems[5].strip(), linitems[6].strip(), linitems[8].strip(), linitems[7].strip(), lflg])
			# 0:路線、1:駅、2:分岐駅、3:営業キロ、4:分岐路線、5:同名駅, 6:分岐路線2/分岐駅2, 7:lflg

			if bitem[6]:
				bline2 = bitem[6][:bitem[6].find('/')]					### 分岐駅まで2路線以上(日田彦山線‐小倉の例)
				bstation2t = bitem[6][bitem[6].find('/') + 1:]

				self.con.execute("""
				insert into t_jctspcl(type, jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2) values(
				2,
				(select rowid from t_line where name=?),
				(select rowid from t_station where name=? and samename=?),
				(select rowid from t_line where name=?),
				(select rowid from t_station where name=? and samename=?))
				""",
				[bitem[4], *same_staion(bitem[2]), bline2, *same_staion(bstation2t)])
			else:
				self.con.execute("""
				insert into t_jctspcl(type, jctsp_line_id1, jctsp_station_id1) values(
				1,
				(select rowid from t_line where name=?),
				(select rowid from t_station where name=? and samename=?))
				""",
				[bitem[4], *same_staion(bitem[2])])

			self.con.execute("""insert into t_lines values(
			(select rowid from t_line where name=?),
			(select rowid from t_station where name=? and samename=?),
			?,
			(select seq from sqlite_sequence where name='t_jctspcl'),
			((1 << 31) | (4294967040 & ?) | (255 & (select seq from sqlite_sequence where name='t_jctspcl'))))""",
			[bitem[0], bitem[1], bitem[5], bitem[3], bitem[7]])
			# b31=special_t_lines ※ lflgはb31=1なのと、b31=1の場合はb15のみ使用される
			# 0xffffff00=4294967040

		# 分岐フラグ(lflg.b15=純粋な分岐駅(Excel8列の乗換路線欄空欄でもxでもない駅), sflg.b12=分岐特例も含めた分岐駅)
		# lflg.b15の方は既に設定済み、sflg.b12は仮値として複数路線に所属するが分岐駅としたくない駅(新今宮)はonに設定されているのでここでoffする
		# edit jct_flg
		cur2 = self.con.cursor()
		cur3 = self.con.cursor()
		self.cur.execute("select rowid from t_line")
		for rec in self.cur:
			lineid = rec[0]
			cur2.execute("""select station_id from t_lines where
							line_id=?1 and (lflg&(1<<17))=0 and station_id in
							(select station_id from t_lines where line_id!=?1)""", [lineid])
			for rec2 in cur2:
				stationid = rec2[0]
				cur3.execute("select sflg from t_station where rowid=?", [stationid])
				sflg = cur3.fetchone()[0]
				if 0 == (sflg & ((1 << 12) | (1 << 14))):	# 'x':新今宮を除く
					sflg |= (1 << 12)     # 分岐特例の含めた分岐駅
					self.con.execute("update t_station set sflg=?1 where rowid=?2", [sflg, stationid])

		# make t_jct
		self.con.execute("""
		insert into t_jct(station_id) select rowid from t_station
		where rowid in (select station_id from t_lines where (lflg & (1 << 15))!=0)
		""")
		# select id from t_jct j join t_station s on s.rowid=j.station_id where name='森';

		##self.con.execute('create view lines as select p.name as prefect, c.name as company, n.name as line, s.name as station, s.kana as station_kana, sales_km, calc_km, spe_route, jctflg, sameflg, cityflg from t_lines l join t_station s on s.rowid=l.station_id join t_line n on n.rowid=l.line_id join t_prefect p on p.rowid=s.prefect_id join t_company c on c.rowid=s.company_id')
		##self.con.execute('create view same_station as select name, count(*) as ''num'' from t_station where sameflg=1 group by name')
		##self.con.execute('create view junctions as select p.name as prefect, c.name as company, n.name as line, s.name as station, s.kana as station_kana, sales_km, calc_km, spe_route, jctflg, sameflg, cityflg from t_lines l join t_station s on s.rowid=l.station_id join t_line n on n.rowid=l.line_id join t_prefect p on p.rowid=s.prefect_id join t_company c on c.rowid=s.company_id where jctflg=1 order by line_id, sales_km')
		##self.con.execute('create view jct_station as select name, cityflg from t_station where jctflg=1')

#------------------------------------------------------------------------------
	def last_regist(self):

		self.con.commit()

		self.cur.execute('select max(rowid) from t_station')
		n_db_station = self.cur.fetchone()[0]
		self.cur.execute('select count(*) from t_station')
		assert(n_db_station == self.cur.fetchone()[0])
		assert(n_db_station == self.n_station);

		self.cur.execute('select count(*) from t_line')
		n_line = self.cur.fetchone()[0]

		self.cur.execute('select count(*) from t_lines where line_id in (select rowid from t_line)')
		n_lines = self.cur.fetchone()[0]

		self.cur.execute('select count(*) from t_lines')
		n_dblines = self.cur.fetchone()[0]

		#except sqlite3.IntegrityError:
		#	print("Sqlite3 Integrity Error: current line={0}".format(n_lin))
		#	exit(-1)
		##########################################################

		print("complete success. line={0}, stations={1} affected, lines={2}({3}) affected.".format(n_line, self.n_station, n_lines, n_dblines))

		print()
		print("/* --------------------------------------- */")
		print("#define MAX_STATION	{0}".format(self.n_station))
		print("#define MAX_LINE	{0}".format(n_line))

		self.cur.execute("update t_global set max_station={0}".format(self.n_station))
		self.cur.execute("update t_global set max_line={0}".format(n_line))

		self.cur.execute("select count(*) from t_jct")
		n = self.cur.fetchone()[0]
		print("#define MAX_JCT	{0}".format(n))
		self.cur.execute("update t_global set max_jct={0}".format(n))

		self.cur.execute("select max(length(name)) from t_station");
		n = self.cur.fetchone()[0]
		print("#define MAX_STATION_CHR	{0}".format((n+1)*2))
		self.cur.execute("update t_global set max_station_chr={0}".format(n))

		self.cur.execute("select max(length(name)) from t_line");
		n = self.cur.fetchone()[0]
		print("#define MAX_LINE_CHR	{0}".format((n+1)*2))
		self.cur.execute("update t_global set max_line_chr={0}".format(n))

		self.cur.execute("select max(y.num) from (select count(*) as num from t_compnpass group by station_id1, station_id2) y")
		n = self.cur.fetchone()[0]
		if not n:
			n = 0
		print("#define MAX_COMPNPASSSET	{0}".format(n))
		self.cur.execute("update t_global set max_compnpass={0}".format(n))

		"""
		select t.name from t_station where company_id in (select rowid from t_company name not like 'JR%')

		"""

dbreg = Dbreg(fn[0:fn.find(".")] + ".db")
dbreg.create_tables()
dbreg.first_regist()
dbreg.second_regist()
dbreg.last_regist()

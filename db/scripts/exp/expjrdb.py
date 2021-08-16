#!python3.0.1
# -*- coding: utf-8 -*-

"""
  sql 実行テスト用テンプレート

"""
import sys
import os
import jrdb
import sqlite3

jrdb.con = sqlite3.connect('../jrdb2015.db')  # , isolation_level=None)

sql = """
select station_id from t_lines where line_id=?
"""


# result list
for inf in jrdb.sqlexec(sql, [ jrdb.line_id(sys.argv[1]) ] ):
	print(jrdb.station_name(inf[0]))



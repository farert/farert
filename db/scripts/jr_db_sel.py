#!python3.0.1
# -*- coding: utf-8 -*-

import sqlite3

dbname = 'jr.db'

c = con.cursor()
c.execute("select * from t_prefect")
for row in c: # rowはtuple
    print(row[0])

for row in c: # rowはtuple
    print(row[0])

c.execute("select * from t_line")
for row in c: # rowはtuple
    print(row[0])

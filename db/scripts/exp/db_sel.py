#!python3.0.1
# -*- coding: utf-8 -*-
import sqlite3
con = sqlite3.connect("data.db")


c = con.cursor()
c.execute("select * from t_emp")
for row in c: # row��tuple
    print(row[0], row[1], row[2])


#!python3.0.1
# -*- coding: utf-8 -*-

import io

for ll in io.open('index.txt', 'r'):
	print(ll, end='')

print('@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@')
io.close()
for ll in io.open('index.txt', 'r'):
	print(ll, end='')

print("aaa");
print("bbb", end='');
print("ccc");
print("ddd");





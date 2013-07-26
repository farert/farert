# -*- coding: shift-jis -*-

"""
�H����Ӊ�
JR�k�C��	���ٖ{��	0		���ىw	�͂�����	 
JR�k�C��	���ٖ{��	3.4		�ܗŊs�w	����傤����	���]����
JR�k�C��	���ٖ{��	8.3		�j�[�w	�����傤	 
 :

�k�C��	JR�k�C��	���ِ�	���ىw	�͂�����	0	0	 	

�e�t�B�[���h�̍ő啶�������擾(UTF-8�o�C�g�j


"""
import sys

if 1 < len(sys.argv):
  fn = sys.argv[1] 
else:
  fn = 'jr.txt'

ln = 0
max_width_company = 0
max_width_prefect = 0
max_width_line = 0
max_width_station = 0
max_width_station_kana = 0
max_width_jct = 0
max_width_jct_num = 0

for lin in open(fn, 'r'):
	ln += 1
	if ln == 1: continue
	linitems = lin.split('\t')

	width = len(linitems[1].strip().encode()) # company
	if max_width_company < width:
		max_width_company = width

	width = len(linitems[0].strip().encode()) # prefect
	if max_width_prefect < width:
		max_width_prefect = width

	width = len(linitems[2].strip().encode()) # ��
	if max_width_line < width:
		max_width_line = width

	width = len(linitems[3].strip().encode()) # �w
	if max_width_station < width:
		max_width_station = width

	width = len(linitems[4].strip().encode()) # �wkana
	if max_width_station_kana < width:
		max_width_station_kana = width

	width = len(linitems[7].strip().encode()) # jct
	if max_width_jct < width:
		max_width_jct = width

	train_lines = [y for y in linitems[7].split('/') if y.strip() != '']
	train_lines = list(map(lambda r:r.strip(), train_lines))

	if max_width_jct_num < len(train_lines):
		max_width_jct_num = len(train_lines)


print("company max width {0} bytes.".format(max_width_company))
print("prefect max width {0} bytes.".format(max_width_prefect))
print("line max width {0} bytes.".format(max_width_line))
print("station max width {0} bytes.".format(max_width_station))
print("station_kana max width {0} bytes.".format(max_width_station_kana))
print("jct max width {0} bytes.".format(max_width_jct))
print("jct_num max width {0}.".format(max_width_jct_num))



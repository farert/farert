# -*- coding: shift-jis -*-

"""
�H����Ӊ�
JR�k�C��	���ٖ{��	0		���ىw	�͂�����	 
JR�k�C��	���ٖ{��	3.4		�ܗŊs�w	����傤����	���]����
JR�k�C��	���ٖ{��	8.3		�j�[�w	�����傤	 
 :

�k�C��	JR�k�C��	���ِ�	���ىw	�͂�����	0	0	 	

�w���ŁA�w�Ƃ����������������Ă���w
�d���w��

�𒊏o

"""
import sys
import re
from collections import defaultdict


if 1 < len(sys.argv):
  fn = sys.argv[1] 
else:
  fn = 'jr.txt'

ln = 0
r1 = re.compile('�w.*�w')
r2 = re.compile('.+�w\Z')
e1 = []
e2 = []

branch = defaultdict(str)
h_eki = defaultdict(int)

for lin in open(fn, 'r'):
	ln += 1
	if ln == 1: continue
	linitems = lin.split('\t')
	eki = linitems[3]		# �w
	h_eki[eki] += 1
	train_lines = []
	train_lines.append(linitems[2])    # �H�� 
	if linitems[7].strip != "":
		train_lines.extend( linitems[7].split('/'))

	train_lines = [y for y in train_lines if y.strip() != '']
	train_lines = list(map(lambda r:r.strip(), train_lines))
	
	train_lines.sort()									 # �H���A����H���Q�̔z��\�[�g
	if branch[eki] != "":
		branch[eki] += ","
  
	branch[eki] += "|".join(train_lines)   # �e�w���ɘH���A����H���Q��|��؂�̕�����ŕێ�

	if r1.match(eki):   # �w���܂މw
		e1.append("{0}({1}): {2}".format(fn, ln, eki))

	if not (r2.match(eki)): # �w�ŏI����Ă��Ȃ��w
		e2.append("{0}({1}): {2}".format(fn, ln, eki) )

print("�������w�Ƃ�������������������")
for e in e1:
   print(e)

#print("�������w�Ƃ��������ŏI����Ă��Ȃ�������")
#e2.each do |e|
#   print(e)
#end
print("�������d���w�� or ����w(�����H��������)������")
# �]����|���ِ�,�]����|���ِ�
# ���ِ�

# h_keys  �e�w�o��񐔂�ێ�
# branch  �e�w�����H����ێ�
#
for e in h_eki.keys():			# �e�w��
	bra = branch[e].split(",")   # �w��2��ȏ゠��ꍇ, bra�Ɍ����̏����H���̔z�񂪓���
	bra = list(map(lambda r:r.split("|"), bra))  #bra.map! {|r| r.split("|") }
	
	if 1 < h_eki[e]: # ����w or 2�x�ȏ�o�ꂵ���w e
		if len(bra) != h_eki[e]:
			print("!!!!!!!!!bug!!!!! #{0}:#{1}".format(e, len(bra)))

		print("{0}({1})".format(e, h_eki[e]))   # �w��(�o�ꐔ)
		other_station_lines = False
		n_eki = 0
		
		for station_lines in bra:		# �����w��
			n_eki += 1
			if False == other_station_lines:
				other_station_lines = station_lines
				if len(station_lines) <= 0:
					print("!!!!!bug!!!!!")
				elif len(station_lines)  == 1:
					print("  {0}�w{1}/{2}: �w�����Ȃ̂ɕ���w�Ȃ�(�����w)".format(e, n_eki, len(bra)))
				elif len(station_lines) != h_eki[e]:
					print("  {0}�w{1}/{2}: ����H�����̑��� {3}, {4}: {5}".format(e, n_eki, len(bra), len(station_lines), h_eki[e], '/'.join(station_lines)))

			if other_station_lines != station_lines and len(station_lines) != 1:
				print("  {0}�w{1}/{2}: ����H���s��: {3}, {4}".format(e, n_eki, len(bra), '/'.join(other_station_lines), '/'.join(station_lines)))
			elif len(station_lines) == 1:
				print("               {0}".format('/'.join(station_lines)))

	else: # �w1��(����Ȃ�)
		if 1 < len(bra[0]):
			print("�w��Ȃ̂ɕ���w�H {0}:{1}->{2}".format(e, len(bra), bra[0]))



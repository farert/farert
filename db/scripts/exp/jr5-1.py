# -*- coding: shift-jis -*-

"""
�H����Ӊ�
JR�k�C��	���ٖ{��	0		���ىw	�͂�����	 
JR�k�C��	���ٖ{��	3.4		�ܗŊs�w	����傤����	���]����
JR�k�C��	���ٖ{��	8.3		�j�[�w	�����傤	 
 :

�k�C��	JR�k�C��	���ِ�	���ىw	�͂�����	0	0	 	����H��	����H���t���O	����	�s��s��

- ����H���t���O�������Ă��邩
- �����t���O�������Ă��邩
- �s��s���ʈꗗ�\��

�D�y�s��	1
���s��	2
�s���	3
�R�����	4
���l�s��	5
���É��s��	6
���s�s��	7
���s��	8
�_�ˎs��	9
�L���s��	10
�k��B�s��	11
�����s��	12

"""
import sys
import re
from collections import defaultdict


if 1 < len(sys.argv):
  fn = sys.argv[1] 
else:
  fn = 'jr.txt'

ln = 0
toku = ([], [], [], [], [], [], [], [], [], [], [], [] )
dupstations = []

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


	i = int(linitems[9])
	if i == 1:
		dupstations.append(eki + '(' + linitems[0] + ');' + linitems[2])
	elif i != 0:
		print("{0}({1}) �����w�t���O�s��={2}".format(fn, ln, i))


	i = int(linitems[10])
	if len(toku) < i:
		print("{0}({1}) �s��s���R�[�h�s��={2}".format(fn, ln, i))
	elif 0 < i:
		toku[i - 1].append("{0}({1})".format(linitems[3], linitems[2]))


	if linitems[7].strip() != "":
		train_lines.extend( linitems[7].split('/'))
		if 1 != int(linitems[8]):
			print("{0}({1}) ����H���t���O�s��(1�ł���ׂ���{2})".format(fn, ln, int(linitems[8])))
	else:
		if 0 != int(linitems[8]):
			print("{0}({1}) ����H���t���O�s��(0�ł���ׂ���{2})".format(fn, ln, int(linitems[8])))

	train_lines = [y for y in train_lines if y.strip() != '']
	train_lines = list(map(lambda r:r.strip(), train_lines))
	
	train_lines.sort()									 # �H���A����H���Q�̔z��\�[�g
	if branch[eki] != "":
		branch[eki] += ","
  
	branch[eki] += "|".join(train_lines)   # �e�w���ɘH���A����H���Q��|��؂�̕�����ŕێ�


print("�������s��s���ꗗ������")
s = """
�D�y�s��
���s��
�s���
�R�����
���l�s��
���É��s��
���s�s��
���s��
�_�ˎs��
�L���s��
�k��B�s��
�����s��
"""
stoku = s.split('\n')[1:-1]

for i in range(len(toku)):
	print("***{0}***".format(stoku[i]))
	for e in toku[i]:
		print(e)


# h_keys  �e�w�o��񐔂�ێ�
# branch  �e�w�����H����ێ�
#
dup_e = []
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
					dup_e.append(e)
				elif len(station_lines) != h_eki[e]:
					print("  {0}�w{1}/{2}: ����H�����̑��� {3}, {4}: {5}".format(e, n_eki, len(bra), len(station_lines), h_eki[e], '/'.join(station_lines)))

			if other_station_lines != station_lines and len(station_lines) != 1:
				print("  {0}�w{1}/{2}: ����H���s��: {3}, {4}".format(e, n_eki, len(bra), '/'.join(other_station_lines), '/'.join(station_lines)))
			elif len(station_lines) == 1:
				print("               {0}".format('/'.join(station_lines)))

	else: # �w1��(����Ȃ�)
		if 1 < len(bra[0]):
			print("�w��Ȃ̂ɕ���w�H {0}:{1}->{2}".format(e, len(bra), bra[0]))


print("�����������w�ꗗ������")
dupstations.sort()
for el in dupstations:
	e = el[0:el.index('(')]
	if e in dup_e:
		print(el + '*')
		del dup_e[dup_e.index(e)]
	else:
		print(el)
		
print('=======')
for e in dup_e:
	print(e)

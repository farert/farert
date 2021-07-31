#!python3.0.1
# -*- coding: utf-8 -*-


import sys

TAX = 10

def fare(km):

	if km < 40:				# 1 to 3km
		return 147
	if km < 70:				# 4 to 6km
		return 189
	if km < 101:			# 7 to 10km
		return 199

	if (6000 < km):						# 600km越えは40キロ刻み
		c_km = (km - 1) // 400 * 400 + 200
	elif (1000 < km) :					# 100.1-600kmは20キロ刻み
		c_km = (km - 1) // 200 * 200 + 100
	elif (500 < km) :					# 50.1-100kmは10キロ刻み
		c_km = (km - 1) // 100 * 100 + 50
	elif (100 < km) :					# 10.1-50kmは5キロ刻み
		c_km = (km - 1) // 50 * 50 + 30
	else:
		assert(False)

	if (6000 <= c_km):
		fare = 1620 * 3000 + 1285 * 3000 + 705 * (c_km - 6000)
	elif (3000 < c_km) :
		fare = 1620 * 3000 + 1285 * (c_km - 3000)
	else :
		fare = 1620 * c_km

	if (c_km <= 1000) :	# 100km以下は切り上げ
		# 1の位を切り上げ
		fare = (fare + 9999) // 10000 * 10
	else:				# 100㎞越えは四捨五入
		fare = (fare + 50000) // 100000 * 100

	#fare = fare + ((fare // 10 // 2) + TAX) // 10 * 10 # tax = +5%, 四捨五入
	fare = fare + (fare * 1000 * TAX // 100000) # tax = +5%, 四捨五入

	return fare


yen_b = -1
for k in range(0, 35000):
	yen = fare(k)
	if yen_b != yen:
		print("{0}\t{1}".format((k + 9)//10, yen))
		yen_b = yen

farert : test
	mv -f test farert

# test_azusa のビルドルール
test_azusa: test_azusa.o azusa.o alpdb.o db.o sqlite3.o ../common/stdafx.o
	g++ -o test_azusa -std=c++0x test_azusa.o azusa.o alpdb.o db.o sqlite3.o ../common/stdafx.o -lpthread -ldl

test_azusa.o: test_azusa.cpp
	g++ $(CFLAGS) -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -I../../../app/alps -std=c++0x -c test_azusa.cpp -o test_azusa.o

include ../common/Makefile

# clean ターゲットを拡張（test_azusa も削除）
clean:
	rm -f *.o test farert test_azusa
	rm -f ../common/*.o

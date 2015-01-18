
OBJS = $(patsubst %.c,%.o,$(wildcard ./*.c)) $(patsubst %.cpp,%.o,$(wildcard ./*.cpp)) db.o sqlite3.o alpdb.o testmain.o

test : 

 .c.o:
	gcc -I. -I../../app/win_mfc/fjr_mfc/lib/db -c $< -o $@

.cpp.o:
	g++ -I. -std=c++11 -c $< -o $@


test : $(OBJS)
	g++ -o $@ -std=c++11 $^ -lpthread -ldl

clean:
	rm -f *.o test


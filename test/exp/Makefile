
OBJS = $(patsubst %.c,%.o,$(wildcard ./*.c)) $(patsubst %.cpp,%.o,$(wildcard ./*.cpp)) db.o sqlite3.o alpdb.o testmain.o

test : 

 .c.o:
	gcc -I. -Wall -c $< -o $@

.cpp.o:
	g++ -I. -Wall -std=c++11 -c $< -o $@


test : $(OBJS)
	g++ -o $@ -Wall -std=c++11 $^ -lpthread -ldl

clean:
	rm -f *.o test


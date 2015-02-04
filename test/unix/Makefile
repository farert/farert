CFLAGS = -Wall -D_DEBUG -g
# test_exec.cpp
vpath %cpp ../../app/win_mfc/fjr_mfc/alps_mfc

ifeq ($(EXP), 1)
vpath %cpp exp
vpath %h exp
else
vpath %cpp ../../app/win_mfc/fjr_mfc/lib/alps
vpath %h ../../app/win_mfc/fjr_mfc/lib/alps
endif
vpath %cpp ../../app/win_mfc/fjr_mfc/lib/db
vpath %h ../../app/win_mfc/fjr_mfc/lib/db
vpath %c ../../app/win_mfc/fjr_mfc/lib/db


OBJS = $(patsubst %.c,%.o,$(wildcard ./*.c)) $(patsubst %.cpp,%.o,$(wildcard ./*.cpp)) test_exec.o \
       db.o sqlite3.o alpdb.o testmain.o

test : 

 .c.o:
	gcc $(CFLAGS) -I. -I../../app/win_mfc/fjr_mfc/lib/db -c $< -o $@

.cpp.o:
	g++ $(CFLAGS) -I. -I../../app/win_mfc/fjr_mfc/lib/db -I../../app/win_mfc/fjr_mfc/lib/alps -std=c++11 -c $< -o $@


test : $(OBJS)
	g++ -o $@ -std=c++11 $^ -lpthread -ldl

clean:
	rm -f *.o test


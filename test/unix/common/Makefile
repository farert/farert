CFLAGS = -Wall -D_DEBUG -g
# test_exec.cpp
vpath %cpp ../../../app/win_mfc/fjr_mfc/alps_mfc

ifeq ($(EXP), 1)
vpath %cpp exp
vpath %h exp
else
vpath %cpp ../../../app/alps
vpath %h ../../../app/alps
endif

vpath %cpp ../../../app/win_mfc/fjr_mfc/lib/db
vpath %h ../../../app/win_mfc/fjr_mfc/lib/db
vpath %c ../../../app/win_mfc/fjr_mfc/lib/db
vpath %cpp ../common


OBJS = $(patsubst %.c,%.o,$(wildcard ./*.c)) $(patsubst %.cpp,%.o,$(wildcard ./*.cpp)) $(patsubst %.cpp,%.o,$(wildcard ../common/*.cpp)) test_exec.o \
       db.o sqlite3.o alpdb.o testmain.o

test : 

 .c.o:
	gcc $(CFLAGS) -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -c $< -o $@

.cpp.o:
	g++ $(CFLAGS) -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -I../../../app/alps -std=c++0x -c $< -o $@


test : $(OBJS)
	g++ -o $@ -std=c++0x $^ -lpthread -ldl

clean:
	rm -f *.o test


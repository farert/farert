
# test_exec.cpp
vpath %cpp ../../app/win_mfc/fjr_mfc/alps_mfc
vpath %cpp ../../app/win_mfc/fjr_mfc/lib/alps
vpath %h ../../app/win_mfc/fjr_mfc/lib/alps
vpath %cpp ../../app/win_mfc/fjr_mfc/lib/db
vpath %h ../../app/win_mfc/fjr_mfc/lib/db
vpath %c ../../app/win_mfc/fjr_mfc/lib/db

OBJS = $(patsubst %.c,%.o,$(wildcard ./*.c)) $(patsubst %.cpp,%.o,$(wildcard ./*.cpp)) test_exec.o \
       db.o sqlite3.o alpdb.o alpsmain.o

alp: 

 .c.o:
	gcc -I. -I../../app/win_mfc/fjr_mfc/lib/db -c $< -o $@

.cpp.o:
	g++ -I. -I../../app/win_mfc/fjr_mfc/lib/db -I../../app/win_mfc/fjr_mfc/lib/alps -std=c++11 -c $< -o $@


alp: $(OBJS)
	g++ -o $@ -std=c++11 $^ -lpthread -ldl

clean:
	rm -f *.o alp


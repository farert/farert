CFLAGS = -Wall -D_DEBUG -D_CONSOLE_TEST -g -fprofile-arcs -ftest-coverage

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


OBJS = $(patsubst %.c,%.o,$(wildcard ./*.c)) $(patsubst %.cpp,%.o,$(wildcard ./*.cpp)) $(patsubst %.cpp,%.o,$(wildcard ../common/*.cpp)) ../common/test_exec.o \
       db.o sqlite3.o alpdb.o testmain.o

farert : test
	mv -f test farert

test : 

 .c.o:
	gcc $(CFLAGS) -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -c $< -o $@

.cpp.o:
	g++ $(CFLAGS) -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -I../../../app/alps -std=c++0x -c $< -o $@


test : $(OBJS)
	g++ -o $@ -std=c++0x $^ -lpthread -ldl -lgcov

clean:
	rm -f *.o test *.gcda *.gcno *.gcov
	rm -rf coverage_html coverage.info

coverage: test
	./test -exec
	gcov *.cpp
	@echo "Coverage report generated. Check *.gcov files"

coverage-html: coverage
	@echo "Generating HTML coverage report with lcov..."
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.info
	lcov --remove coverage.info '*/sqlite3.c' --output-file coverage.info
	genhtml coverage.info --output-directory coverage_html
	@echo "HTML coverage report generated in coverage_html/"
	@echo "Open coverage_html/index.html in a browser"


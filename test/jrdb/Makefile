
alp: 

 .c.o:
	gcc -I.  -c $< -o $@

.cpp.o:
	g++ -I. -std=c++11 -c $< -o $@

alp: alpsmain.o db.o sqlite3.o alpdb.o
	g++ -o alp -std=c++11 $^ -lpthread -ldl


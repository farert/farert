SRCS = stdafx.cpp sqlite3.c db.cpp alpdb.cpp azusa.cpp
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

vpath %.c ../../../app/win_mfc/fjr_mfc/lib/db ../../../app/alps
vpath %.cpp ../../../app/win_mfc/fjr_mfc/lib/db ../../../app/alps ../common

CXX = g++
CXXFLAGS = -Wall -std=c++0x -D_NO_LOG -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -I../../../app/alps
LDLIBS = -lpthread -ldl

all: farecli test_azusa

farecli: $(OBJS) farecli.o
	$(CXX) -o $@ $^ $(LDLIBS)

test_azusa: $(OBJS) test_azusa.o
	$(CXX) -o $@ $^ $(LDLIBS)

test_azusa.o: test_azusa.cpp $(SRCS)
	clang -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o farecli test_azusa


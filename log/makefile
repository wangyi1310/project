CC=g++
VAR=relase
LIB=liblog.a 
ifeq ($(VAR),debug)
	CFLAG= -c -g
else 
	CFLAG= -c
endif
all:$(LIB)
$(LIB):mutex.o gettime.o Log.o 
	ar crs $(LIB) mutex.o gettime.o Log.o
mutex.o:mutex.h mutex.cpp
	$(CC) $(CFLAG) mutex.cpp mutex.h
gettime.o:gettime.h gettime.cpp
	$(CC) $(CFLAG) gettime.h gettime.cpp
Log.o:Log.h Log.cpp
	$(CC) $(CFLAG) Log.h Log.cpp
clean:
	rm -rf *.o $(LIB) *.gch

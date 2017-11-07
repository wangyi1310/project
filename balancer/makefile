CC=g++
VAR=release
LIB=libBalancer.a
ifeq ($(VAR),debug)
	CFLAG= -c -g
else 
	CFLAG= -c
endif
all:$(LIB)
$(LIB):Balancer.o md5.o hashcirle.o
	ar crs $(LIB) Balancer.o md5.o hashcirle.o
Balancer.o:Balancer.cpp Balancer.h
	$(CC) $(CFLAG) Balancer.cpp Balancer.h
md5.o:md5.cpp md5.h
	$(CC) $(CFLAG) md5.cpp md5.h
hashcirle.o:hashcirle.cpp
	$(CC) $(CFLAG) hashcirle.cpp hashcirle.h
run: $(TEST)
	./$(TEST)
clean:
	rm -rf *.o $(LIB) *.h.gch

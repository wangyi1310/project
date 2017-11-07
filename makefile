CC=g++
VAR=relase
TEST=main
LIB=log/liblog.a
LIB1=balancer/libBalancer.a
ifeq ($(VAR),debug)
	CFLAG= -c -g
else 
	CFLAG= -c
endif
all:$(TEST)
$(TEST):main.o $(LIB) $(LIB1)
	$(CC) -o $(TEST) main.o  -Lbalancer -lBalancer -Llog -llog -lpthread -ljson
main.o: main.cpp
	$(CC) $(CFLAG) main.cpp
$(LIB): log
	make -C log
$(LIB1):balancer
	make -C balancer
run: $(TEST)
	./$(TEST)
clean:
	rm -rf *.o $(TEST)

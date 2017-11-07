CC=g++
OBJ=ngx_pool.o
LIB=libngx_pool.a
NAME=ngx_pool
SRC=ngx_pool.cpp
TEST=main

all : $(TEST)

$(TEST) :  main.cpp $(LIB)
	$(CC) -o $(TEST) main.cpp  -L. -l$(NAME) -g
$(LIB) : $(OBJ)
	ar crs $(LIB) $(OBJ)
$(OBJ) : $(SRC)
	$(CC) -c $(SRC)
	
clean : 
	rm -rf $(LIB) $(OBJ) $(TEST)
 
 


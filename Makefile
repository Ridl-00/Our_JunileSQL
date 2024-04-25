obj = clever.o 

vpath %.h ./include/

clever_db : clever.c
	@gcc -o clever_db clever.c	
clever.o : clever.c
	@gcc -c clever.c -o clever.o
btree.o : btree.h
	@gcc -I./include/ -c btree.c -o btree.o

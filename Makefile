obj = clever.o print.o operating.o btree.o buffer.o

vpath %.h ./include/

canWe_win_aPrize_db.out : $(obj)
	gcc $(obj) -o canWe_win_aPrize_db.out

clever.o :  buffer.h btree.h operating.h
	gcc -I./include -c clever.c -o clever.o

print.o : print.h btree.h btree.o
	gcc -I./include -c print.c -o print.o

operating.o : operating.h btree.h info_read.h btree.o
	gcc -I./include -c operating.c -o operating.o

# info_read.o : info_read.h btree.h btree.o
# 	gcc -I./include -c info_read.c -o info_read.o -lmingw32
# 	# gcc -I./include -o info_read info_read.c -lmingw32

btree.o : btree.h
	gcc -I./include/ -c btree.c -o btree.o

buffer.o : buffer.h
	gcc -I./include -c buffer.c -o buffer.o


.PHONY : clean run
clean :
	-rm -f *.o *.out

run: 
	./canWe_win_aPrize_db.out
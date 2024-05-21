obj = clever.o print.o operating.o info_read.o btree.o buffer.o mystrptime.o

vpath %.h ./include/

canWe_win_aPrize_db.out : $(obj) repo_dir
	@gcc $(obj) -o canWe_win_aPrize_db.out; \
	rm $(obj)

clever.o :  buffer.h btree.h operating.h 
	gcc -I./include -c clever.c -o clever.o

print.o : print.h btree.h btree.o
	gcc -I./include -c print.c -o print.o

operating.o : operating.h btree.h info_read.h btree.o
	gcc -I./include -c operating.c -o operating.o

info_read.o : info_read.h btree.h mystrptime.h btree.o
	gcc -I./include -c info_read.c -o info_read.o

btree.o : btree.h
	gcc -I./include/ -c btree.c -o btree.o

buffer.o : buffer.h
	gcc -I./include -c buffer.c -o buffer.o

mystrptime.o : mystrptime.h
	gcc -I./include -c mystrptime.c -o mystrptime.o

.PHONY : clean repo_dir run
clean :
	-rm -f *.o *.out

repo_dir :
	@if [ ! -d "./repo/" ]; then \
		mkdir -p "./repo/"; \
	fi

run: 
	./canWe_win_aPrize_db.out
obj = clever.o print.o operating.o info_read.o btree.o buffer.o

vpath %.h ./include/

db.out : $(obj) repo_dir
	@gcc $(obj) -o db.out; \
	rm $(obj)

clever.o :  buffer.h btree.h operating.h 
	gcc -I./include -c clever.c -o clever.o

print.o : print.h btree.h btree.o
	gcc -I./include -c print.c -o print.o

operating.o : operating.h btree.h info_read.h btree.o
	gcc -I./include -c operating.c -o operating.o

info_read.o : info_read.h btree.h btree.o
	gcc -I./include -c info_read.c -o info_read.o

btree.o : btree.h
	gcc -I./include/ -c btree.c -o btree.o

buffer.o : buffer.h
	gcc -I./include -c buffer.c -o buffer.o



.PHONY : clean repo_dir
clean :
	-rm -f *.o *.out

repo_dir :
	@if [ ! -d "./repo/" ]; then \
		mkdir -p "./repo/"; \
	fi
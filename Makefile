obj = clever.o btree.o

vpath %.h ./include/

clever_db: $(obj)
	@gcc -o $@$^

clever.o: clever.c
	@gcc -I./include/ -c $< -o$@

btree.o: btree.c btree.h
	@gcc -I./include/ -c $< -o$@

.PHONY: clean
clean:
	rm -f $(obj) clever_db

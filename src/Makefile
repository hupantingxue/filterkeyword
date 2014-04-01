#when install the cgi, you should set a proper path where you want to intall.

OBJECTS = fkw_util.o fkw_rbtree_dict.o fkw_seg.o fkw_rbtree.o mem_collector.o test.o

all:test

$(OBJECTS):fkw_util.h fkw_rbtree_dict.h fkw_seg.h fkw_rbtree.h mem_collector.h

test: $(OBJECTS)
	gcc -W -g -o test $(OBJECTS)

.PHONY: clean
clean: 
	-rm -rf  test *.o 

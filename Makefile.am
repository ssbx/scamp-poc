t1: all
	./src/scamp2 -vvtf ./test/data/fitscat/*.cat

t2: all
	./src/scamp2 -vvt ./test/data/ascii/*.txt

test: t1 t2
run: test

.PHONY: run test

SUBDIRS=src

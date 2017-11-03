t1: all
	./src/scamp2 -vvtf ./test/data/fitscat/*.cat

t2: all
	./src/scamp2 -vvt -d 0.1 ./test/data/ascii/*.txt

test: t1 t2
run: t2

.PHONY: run test

SUBDIRS=src

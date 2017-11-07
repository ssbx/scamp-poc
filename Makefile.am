t1: all
	./src/scamp2 -vvtf ./tests/data/fitscat/*.cat

t2: all
	./src/scamp2 -vvt -d 150 ./tests/data/ascii/*.txt

test: t1 t2
run: t2

.PHONY: run test

SUBDIRS=src

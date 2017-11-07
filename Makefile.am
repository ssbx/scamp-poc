tests: all
	./tests/scamp2_tests.sh

run: all
	./src/scamp2 -vvt -d 180 ./tests/data/ascii/data*.txt

.PHONY: run test

SUBDIRS=src

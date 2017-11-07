tests: all
	./tests/scamp2_tests.sh

run: all
	./src/scamp2 -vvt -f 1.0 ./tests/data/ascii/data*.txt

.PHONY: run test

SUBDIRS = src

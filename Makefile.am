run: all
	./tests/testAngdist
#	./src/scamppoc ./tests/data/fitscat/data1.fits.cat ./tests/data/fitscat/data2.fits.cat

tests: all
	./tests/test_suite.sh

check: tests

.PHONY: run tests

SUBDIRS = src tests

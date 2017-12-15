run: all
	./tests/perfCrossmatchSingle ./tests/data/fitscat/data*cat
	
	
tests: all
	./tests/test_suite.sh

check: tests

.PHONY: run tests

SUBDIRS = src tests

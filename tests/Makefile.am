bin_PROGRAMS = \
	testChealpixNeighboursNest \
	testSingleCatCrossmatch \
	testChealpixsphereAvltree \
	perfCrossmatchSingle
	
testChealpixNeighboursNest_SOURCES= \
		test_chealpix_neighbours_nest.c \
		../src/chealpix.c \
		../src/chealpix.h

testSingleCatCrossmatch_SOURCES= \
		test_single_cat_crossmatch.c \
 		../src/catalog.c \
		../src/catalog.h \
		../src/crossmatch.c \
		../src/crossmatch.h \
		../src/chealpix.c \
		../src/chealpix.h \
		../src/pixelstore.c \
		../src/pixelstore.h \
		../src/logger.c \
		../src/logger.h \
		../src/mem.c \
		../src/mem.h

testChealpixsphereAvltree_SOURCES= \
		test_chealpixsphere_avltree.c \
 		../src/catalog.c \
		../src/catalog.h \
		../src/chealpix.c \
		../src/chealpix.h \
		../src/pixelstore.c \
		../src/pixelstore.h \
		../src/logger.c \
		../src/logger.h \
		../src/mem.c \
		../src/mem.h

perfCrossmatchSingle_SOURCES= \
		perf_crossmatch_single.c \
 		../src/catalog.c \
		../src/catalog.h \
		../src/chealpix.c \
		../src/chealpix.h \
		../src/pixelstore.c \
		../src/pixelstore.h \
		../src/crossmatch.c \
		../src/crossmatch.h \
		../src/logger.c \
		../src/logger.h \
		../src/mem.c \
		../src/mem.h
perfCrossmatchSingle_CFLAGS = --coverage

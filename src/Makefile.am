SUBDIRS = catalog/fits

bin_PROGRAMS = scamp2
scamp2_SOURCES= main.c \
		logger.h \
		logger.c \
		crossmatch.c \
		crossmatch.h \
 		catalog/catalog.c \
		catalog/catalog.h \
		catalog/object.c \
		catalog/object.h \
		catalog/objectlist.c \
		catalog/objectlist.h
scamp2_LDADD = $(srcdir)/catalog/fits/libfits.a -lm

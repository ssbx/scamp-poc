SUBDIRS = fits

bin_PROGRAMS = scamp2
scamp2_SOURCES= main.c catalog.h catalog.c logger.h logger.c datum.c datum.h \
		object.h object.c test.h test.c datumlist.c datumlist.h crossmatch.c \
		crossmatch.h
scamp2_LDADD = $(srcdir)/fits/libfits.a -lm

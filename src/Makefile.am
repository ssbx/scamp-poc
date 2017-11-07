SUBDIRS = fits

bin_PROGRAMS = scamp2
scamp2_SOURCES= main.c catalog.h catalog.c logger.h logger.c \
		object.h object.c objectlist.c  \
		objectlist.h crossmatch.c crossmatch.h
scamp2_LDADD = $(srcdir)/fits/libfits.a -lm

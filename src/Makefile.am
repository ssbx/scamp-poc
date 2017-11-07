SUBDIRS = fits

bin_PROGRAMS = scamp2
scamp2_SOURCES= main.c catalog.h catalog.c logger/logger.h logger/logger.c \
				objects/object.h objects/object.c objects/objectlist.c  \
				objects/objectlist.h crossmatch.c crossmatch.h
scamp2_LDADD = $(srcdir)/fits/libfits.a -lm

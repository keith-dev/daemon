# ---------------------------------------------------------------------------
#	Copyright (c) 2013-7 Keith Williams.
#	All rights reserved.
#
#	Redistribution and use in source and binary forms are permitted
#	provided that the above copyright notice and this paragraph are
#	duplicated in all such forms and that any documentation,
#	advertising materials, and other materials related to such
#	distribution and use acknowledge that the software was developed
#	by Keith Williams.
# ---------------------------------------------------------------------------

PROG     = daemon
SRCS     = main.c options.c daemon.c utils.c
OBJS     = $(SRCS:.c=.o)
MAN      = daemon.8

DESTDIR ?= /usr/local
CFLAGS  += -Wall -Wextra

all: $(PROG) $(MAN)

install:
	install -b -D $(DESTDIR) -s daemon /bin/daemon
	install -b -D $(DESTDIR) -m 444 daemon.8.gz /share/man/man8

uninstall:
	rm $(DESTDIR)/bin/daemon
	rm $(DESTDIR)/share/man/man8

clean:
	-rm $(PROG)
	-rm $(OBJS)

$(PROG): $(OBJS)
	gzip -cn $(MAN) > $(MAN).gz
	$(CC) $(CFLAGS) $(TARGET_ARCH) -o $@ $^ $(LDFLAGS)

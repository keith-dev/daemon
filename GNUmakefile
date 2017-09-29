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

CFLAGS   += -Wall -Wextra

all: $(PROG)

clean:
	-rm $(PROG)
	-rm $(OBJS)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(TARGET_ARCH) -o $@ $^ $(LDFLAGS)
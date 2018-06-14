########################################################################
TARGET	= correlate

SRCS	= $(TARGET).c utils.c filtersubs.c complex.c timefuncs.c

OBJS	= $(SRCS:%.c=%.o)

LIBS	= -lm -lfftw3 -L../qlib2 -lqlib2

BINDIR	= /usr/local/bin
MANDIR	= /usr/local/man
MANEXT	= 1

CC	= cc
COPT	= -g -Wall 
CFLAGS	= $(COPT) -I/opt/local/include  -I../qlib2

########################################################################

all:	$(TARGET) 

$(TARGET):	$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

install:	install_$(TARGET) 

install_$(TARGET):	$(TARGET)
	cp $(TARGET) $(BINDIR)/$(TARGET)

install_man:	$(TARGET).man 
	cp $(TARGET).man $(MANDIR)/man$(MANEXT)/$(TARGET).$(MANEXT)

clean:	
	rm -rf *.o

veryclean:	clean
	rm -rf $(TARGET) 

tar:	$(TARGET)
	(cd ..; tar cf - $(TARGET)/*.c $(TARGET)/*.h $(TARGET)/Makefile \
		$(TARGET)/*.man $(TARGET)/$(TARGET) \
		$(TARGET)/README $(TARGET)/CHANGES ) \
	| gzip > /tmp/$(TARGET).tar.gz

FORCE:

cleandepend:
	makedepend

depend:
	makedepend $(CFLAGS) *.c

########################################################################
# DO NOT DELETE THIS LINE -- make depend depends on it.

OBJECTS=mapkey.o unix.o
CFLAGS=-O -DANSIC -DUNIX -DTERMCAP
LIBES=-ltermcap

mapkey:$(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) $(LIBES) -o mapkey

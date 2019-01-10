OBJECTS=sedt.o edit.o key.o buf.o vdu.o term.o com.o coma.o comb.o comc.o comd.o ent.o file.o text.o text1.o vars.o regexp.o regsub.o unix.o
#CFLAGS=-g -DUNIX -DULTRIX -DTERMCAP
CFLAGS=-O -DUNIX -DTERMCAP -DANSIC -LINUX
LIBES=-ltermcap

sedt:$(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) $(LIBES) -o sedt

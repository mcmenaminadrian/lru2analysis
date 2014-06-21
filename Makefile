default: all

all: lru2analysis 

debug: debuglru2analysis

#get rid of junk
clean:
	rm -f *.o

# normal build
lru2analysis: runtimer.o insttree.o rbpages.o threadhandler.o \
	analysis.o
	g++ -O2 -o lru2aysis -Wall insttree.o rbpages.o analysis.o \
		threadhandler.o runtimer.o -lexpat -lpthread -lncurses

analysis.o: analysis.cpp threadhandler.h analysis.h
	g++ -O2 -o analysis.o -c -Wall analysis.cpp

insttree.o: insttree.cpp insttree.h 
	g++ -O2 -o insttree.o -c -Wall insttree.cpp

threadhandler.o: threadhandler.c threadhandler.h analysis.h
	gcc -O2 -o threadhandler.o -c -Wall threadhandler.c

rbpages.o: pages.cpp 
	g++ -O2 -o rbpages.o -c -Wall pages.cpp

runtimer.o: runtimer.c threadhandler.h 
	gcc -O2 -o runtimer.o -c -Wall runtimer.c

# debug build
debuglru2analysis: druntimer.o dinsttree.o drbpages.o dthreadhandler.o \
	danalysis.o
	g++ -g -o lru2aysis -Wall dinsttree.o drbpages.o \
		danalysis.o \
		dthreadhandler.o druntimer.o -lexpat -lpthread -lncurses

danalysis.o: analysis.cpp threadhandler.h analysis.h
	g++ -g -o danalysis.o -c -Wall analysis.cpp

dinsttree.o: insttree.cpp insttree.h 
	g++ -g -o dinsttree.o -c -Wall insttree.cpp

dthreadhandler.o: threadhandler.c threadhandler.h analysis.h
	gcc -g -o dthreadhandler.o -c -Wall threadhandler.c

drbpages.o: pages.cpp
	g++ -g -o drbpages.o -c -Wall pages.cpp

druntimer.o: runtimer.c threadhandler.h 
	gcc -g -o druntimer.o -c -Wall runtimer.c



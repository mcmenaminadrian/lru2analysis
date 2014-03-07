all: runtimer 

runtimer: runtimer.o insttree.o rbpages.o opttree.o threadhandler.o
	g++ -O2 -o runtimer -Wall insttree.o rbpages.o opttree.o \
		threadhandler.o runtimer.o -lexpat -lpthread

insttree.o: insttree.cpp insttree.h redblack.hpp
	g++ -O2 -o insttree.o -c -Wall insttree.cpp

threadhandler.o: threadhandler.c threadhandler.h redblack.hpp
	gcc -O2 -o threadhandler.o -c -Wall threadhandler.c

opttree.o: opttree.cpp redblack.hpp
	g++ -O2 -o opttree.o -c -Wall opttree.cpp

rbpages.o: pages.cpp redblack.hpp
	g++ -O2 -o rbpages.o -c -Wall pages.cpp

runtimer.o: runtimer.c 
	gcc -O2 -o runtimer.o -c -Wall runtimer.c


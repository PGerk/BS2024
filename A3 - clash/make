.PHONY: all clean

all: clash

clash: clash.o plist.o
	gcc -Wall -Werror -pedantic -std=c11 -D_XOPEN_SOURCE=700 -o clash clash.o plist.o

clash.o: clash.c plist.h
	gcc -Wall -Werror -pedantic -std=c11 -D_XOPEN_SOURCE=700 -c clash.c -o clash.o

plist.o: plist.c plist.h
	gcc -Wall -Werror -pedantic -std=c11 -D_XOPEN_SOURCE=700 -c plist.c -o plist.o

clean:
	rm -f clash clash.o plist.o
all:
	c++ -g -O2 -Wall -Werror -c -fpic ump.cpp
	c++ -shared -o libump.so ump.o

install:
	install libump.so /usr/local/lib
	install ump.h /usr/local/include

uninstall:
	rm /usr/local/include/ump.h /usr/local/lib/libump.so

clean:
	rm -rf ump.o libump.so
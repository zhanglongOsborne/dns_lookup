dns_lookup.so:dict.h dict.c dns_lookup.h dns_lookup.c
	gcc -c dict.c -o dict.o -Wincompatible-pointer-types
	gcc -c dns_lookup.c -o dns_lookup.o -Wincompatible-pointer-types
	gcc -shared -fPIC dict.o dns_lookup.o -o libdns_lookup.so 
	gcc -g test.c -o test -L. -ldns_lookup

clean:
	rm ./libdns_lookup.so dict.o dns_lookup.o test

all:
	gcc -g dict.c dns_lookup.c test.c -o test -std=c99

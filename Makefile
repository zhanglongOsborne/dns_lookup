dns_lookup.so:dict.h dict.c dns_lookup.h dns_lookup.c
	gcc -c dict.c -o dict.o
	gcc -c dns_lookup.c -o dns_lookup.o
	gcc -shared -fPIC dict.o dns_lookup.o -o dns_lookup.so

clean:
	rm ./dns_lookup.so dict.o dns_lookup.o

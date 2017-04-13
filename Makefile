test: dict.c dns_lookup.c test.c
	gcc   -g -std=c99 dict.c dns_lookup.c test.c -o test
clean:
	rm test



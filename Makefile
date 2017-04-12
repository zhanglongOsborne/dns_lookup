test: dict.c dns_lookup.c test.c
	gcc  -O3 -std=c99 dict.c dns_lookup.c test.c -o test
clean:
	rm test



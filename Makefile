libquobyte.so: libquobyte.c quobyte.h
	$(CC) -shared -fPIC -Wl,-soname,libquobyte.so.0 -o libquobyte.so libquobyte.c

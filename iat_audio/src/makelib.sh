#/bin/sh
gcc -I../include -c iat_transform.c
ar -rc libtransform.a iat_transform.o

rm *.o

mv libtransform.a ../lib

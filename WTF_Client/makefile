all: WTF

WTF: Client.o Functions.o
	gcc Client.o Functions.o -o WTF -lcrypto

	echo "Build Done!"

Client.o: Client.c
	gcc -c -Wall Client.c

Functions.o: Functions.c Functions.h
	gcc -c -Wall Functions.c -lcrypto

clean:
	rm -f WTF *.o Configure.txt *.Manifest *.Update *.txt

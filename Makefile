CC=gcc
CCFLAGS=-Wall -g
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TARGET=bash--
COMPNAME=bash-minus-minus

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) -o $@ $^ $(LDFLAGS) 

%.o: %.c %.h
		$(CC) $(CCFLAGS) -c $<

%.o: %.c
		$(CC) $(CCFLAGS) -c $<

clean:
		rm -f *.o $(TARGET)

run: all
		./$(TARGET)

wc: 
		wc -l *.c *.h

zip:
		zip $(COMPNAME).zip *.c *.h Makefile README.md

tar:
		tar -cvzf $(COMPNAME).tar *.c *.h Makefile README.md

delzip:
		rm $(COMPNAME).zip

deltar: 
		rm $(COMPNAME).tar

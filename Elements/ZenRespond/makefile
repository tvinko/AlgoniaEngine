TARGET	= ZenRespond

IDIR	= . ../../libs/logging ../../libs/zmq/macos/4.3.4/include ../../libs/czmq/macos/4.2.0/include ../../libs/ZenCoreCLR ../../ZenCommon  ../../libs/cJSON/src
CC	    = gcc
CFLAGS	= -fPIC -O2 $(foreach d, $(IDIR), -I$d)

ODIR	= .
LDIR	= . ../../libs/czmq/macos/4.2.0/lib ../../libs/zmq/macos/4.3.4/lib ../../ZenCommon ../../libs/ZenCoreCLR
LFLAGS	= $(foreach d, $(LDIR), -L$d)

LIBS	= -lczmq -lzmq -lZenCommon -lZenCoreCLR

_DEPS	= ZenCommon.h
DEPS	= $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ	= $(TARGET).o cJSON.o
OBJ	    = $(patsubst %,$(ODIR)/%,$(_OBJ))

SRC     = $(wildcard *.c) ../../libs/cJSON/src/cJSON.c 

$(ODIR)/%.o: %.c
	$(CC) -c $(SRC) $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -shared -o $@.so $^ $(LFLAGS) $(SRC_OBJ) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.so $(ODIR)/*.o *~ core $(INCDIR)/*~ 
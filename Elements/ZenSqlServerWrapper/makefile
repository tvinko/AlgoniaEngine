TARGET	= ZenSqlServerWrapper

IDIR	= . ../../ZenCommon ../../libs/ZenCoreCLR ../../libs/logging
CC	    = gcc
CFLAGS	= -fPIC -O2 $(foreach d, $(IDIR), -I$d)

ODIR	= .
LDIR	= . ../../ZenCommon ../../libs/ZenCoreCLR
LFLAGS	= $(foreach d, $(LDIR), -L$d)

LIBS	= -lZenCommon -lZenCoreCLR

_DEPS	= ZenCommon.h ZenCoreCLR.h
DEPS	= $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ	= $(TARGET).o
OBJ	    = $(patsubst %,$(ODIR)/%,$(_OBJ))

SRC     = $(wildcard *.c)

$(ODIR)/%.o: %.c
	$(CC) -c $(SRC) $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -shared -o $@.so $^ $(LFLAGS) $(SRC_OBJ) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.so $(ODIR)/*.o *~ core $(INCDIR)/*~ 
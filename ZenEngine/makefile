TARGET	= AlgoniaEngine

IDIR	= . ../ZenCommon  ../libs/cJSON/src ../libs/logging ../libs/os_call/src ../libs/ini/src ../libs/b64/src 
CC		= gcc
CFLAGS	= -fPIC -O2 $(foreach d, $(IDIR), -I$d)

ODIR	= .
LDIR	= . ../ZenCommon
LFLAGS	= $(foreach d, $(LDIR), -L$d)


LIBS	= -lZenCommon -ldl -lpthread

_DEPS	= ZenCommon.h
DEPS	= $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ	= $(TARGET).o cJSON.o ini.o
OBJ		= $(patsubst %,$(ODIR)/%,$(_OBJ))

SRC		= $(wildcard *.c) ../libs/cJSON/src/cJSON.c  ../libs/ini/src/ini.c ../libs/b64/src/decode.c ../libs/b64/src/encode.c


$(ODIR)/%.o: %.c
	$(CC) -c  $(SRC) $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS) $(SRC_OBJ) $(LIBS)  

.PHONY: clean

clean:
	rm -f $(ODIR)/*.so $(TARGET) $(ODIR)/*.o *~ core $(INCDIR)/*~ 
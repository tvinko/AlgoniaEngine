TARGET	= ZenGUI

IDIR	= . ../../ZenCommon ../../libs/ZenCoreCLR  ../../libs/ultralight/macos/include
CC	    = g++
CFLAGS	= -fPIC -O2 -Wno-unused-result -Wsign-compare -Wall -fstack-protector -Wformat -Werror=format-security  -fwrapv -Wall $(foreach d, $(IDIR), -I$d)

ODIR	= .
LDIR	= . ../../ZenCommon ../../libs/ZenCoreCLR ../../libs/ultralight/macos/bin
LFLAGS	= $(foreach d, $(LDIR), -L$d)

LIBS	= -lZenCommon -lZenCoreCLR -lpthread -ldl  -lutil -lm -lAppCore -lUltralight -lUltralightCore -lWebCore 

_DEPS	= $(TARGET).h ZenCommon.h ZenCoreCLR.h
DEPS	= $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ	= $(TARGET).o
OBJ	    = $(patsubst %,$(ODIR)/%,$(_OBJ))

SRC     = $(wildcard *.cpp) 

$(ODIR)/%.o: %.cpp
	  $(CC) -std=c++11  -c $(SRC)  $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -shared  -o $@.so $^ $(LFLAGS) $(SRC_OBJ) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.so $(ODIR)/*.o *~ core $(INCDIR)/*~ 
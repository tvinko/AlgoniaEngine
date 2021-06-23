TARGET	= ZenGUI

IDIR	= . ../../ZenCommon ../../libs/ZenCoreCLR  ../../libs/cJSON/src ../../libs/logging
CC	    = gcc
CFLAGS	= -fPIC -O2 -Wno-unused-result -Wsign-compare -Wall -fstack-protector -Wformat -Werror=format-security  -fwrapv -Wall $(foreach d, $(IDIR), -I$d)

ODIR	= .
LDIR	= . ../../ZenCommon ../../libs/ZenCoreCLR algonia-webview/target/debug
LFLAGS	= $(foreach d, $(LDIR), -L$d)

LIBS	= -lZenCommon -lZenCoreCLR -lpthread -ldl  -lutil -lm -lweb_view_core


_DEPS	= $(TARGET).h ZenCommon.h ZenCoreCLR.h
DEPS	= $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ	= $(TARGET).o
OBJ	    = $(patsubst %,$(ODIR)/%,$(_OBJ))

SRC     = ZenGUI.c

$(ODIR)/%.o: %.c
	  $(CC)   -c $(SRC)  $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -shared  -o $@.so $^ $(LFLAGS) $(SRC_OBJ) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.so $(ODIR)/*.o *~ core $(INCDIR)/*~ 
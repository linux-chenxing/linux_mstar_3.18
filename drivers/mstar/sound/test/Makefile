CFLAGS = -c -fPIC -Wall -funwind-tables
INC = include
OBJECTS =
LIB =
SLIB =
CROSS_COMPILE ?= arm-linux-gnueabihf-

all: $(LIB) tinypcm

tinypcm: $(SLIB) tinypcm.o
	$(CROSS_COMPILE)gcc tinypcm.o $(SLIB) -o tinypcm


$(LIB): $(OBJECTS)
	$(CROSS_COMPILE)gcc -shared $(OBJECTS) -o $(LIB)

$(SLIB): $(OBJECTS)
	$(CROSS_COMPILE)ar rcs $(SLIB) $(OBJECTS)

.c.o:
	$(CROSS_COMPILE)gcc $(CFLAGS) $< -I$(INC)

clean:
	-rm $(LIB) $(OBJECTS) tinypcm.o tinypcm \


install:
	mkdir -p ./lib
	cp -f $(LIB) ./lib
	mkdir -p ./bin
	cp -f tinypcm ./bin

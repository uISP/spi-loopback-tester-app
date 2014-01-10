CFLAGS  = $(shell pkg-config --cflags libusb-1.0)
LDFLAGS = $(shell pkg-config --libs libusb-1.0)

all: stool

OBJS=usb.c main.c
stool: $(OBJS)
	gcc $(CFLAGS)  -I"../include" -o $(@) $(^) $(LDFLAGS) -lusb

clean:
	-rm stool

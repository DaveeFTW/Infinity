PSPSDK = $(shell psp-config --pspsdk-path)
PSPDIR = $(shell psp-config --psp-prefix)

CFLAGS = -O2 -G0

OBJS = intraFont.o libccc.o
TARGET_LIB = libintrafont.a

include $(PSPSDK)/lib/build.mak

install: all
	mkdir -p $(PSPDIR)/include $(PSPDIR)/lib
	cp *.h $(PSPDIR)/include
	cp *.a $(PSPDIR)/lib

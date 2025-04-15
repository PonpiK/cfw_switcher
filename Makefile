TARGET = cfw_switcher


OBJS = stubk.o main.o


BUILD_PRX = 1


EXTRA_TARGETS = EBOOT.PBP


PSPSDK=$(shell psp-config -p)
include $(PSPSDK)/lib/build.mak

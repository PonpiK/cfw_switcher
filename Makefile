TARGET := cfw_switcher

OBJS   :=  main.o import.o
CFLAGS := -Os -G0 -fno-pic -Wall -Wextra -Wno-error

BUILD_PRX		 := 1
PSP_LARGE_MEMORY := 0
PSP_FW_VERSION	 := 660

EXTRA_TARGETS	 := EBOOT.PBP
PSP_EBOOT_TITLE  := CFW Switcher for ARK-4

PSPSDK := $(shell psp-config -p)
include $(PSPSDK)/lib/build.mak
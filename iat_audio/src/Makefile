#common makefile header
# makefile will make main.c and build a test in /bin
# learn it now

DIR_INC = ../include
DIR_BIN = ../bin
DIR_LIB = ../lib

TARGET	= iat_test
BIN_TARGET = $(DIR_BIN)/$(TARGET)

CROSS_COMPILE =
CFLAGS = -g -Wall -I$(DIR_INC)

#可根据实际需要修改，选择所需平台SDK，默认为x64或x86平台。
ifdef LINUX64
LDFLAGS := -L$(DIR_LIB)
else
LDFLAGS := -L$(DIR_LIB)
endif

LDFLAGS += -lmsc -lrt -ldl -lpthread

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

$(BIN_TARGET) : $(OBJECTS)
	$(CROSS_COMPILE)gcc $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CROSS_COMPILE)gcc -c $(CFLAGS) $< -o $@
clean:
	@rm -f *.o $(BIN_TARGET)

.PHONY:clean

#common makefile foot

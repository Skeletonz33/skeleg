CC=gcc
CFLAGS=-c -O2 -Wall -Werror -Wextra -Wstrict-prototypes -pedantic -Iinclude -std=c99
AR=ar
AFLAGS=rcs
LD=gcc
LFLAGS=-mwindows -lgdi32 -luser32

SOURCE_DIR=src
BUILD_DIR=build

SRC=
OBJ=$(SRC:.c=.o)

TEST=main.c
TESTOBJ=$(TEST:.c=.o)
TESTEXE=$(TEST:.c=.exe)

LIB=skeleg.a

RM=del
RFLAGS=/f /q

all: $(BUILD_DIR)/$(LIB)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(LIB): $(OBJ:%=$(BUILD_DIR)/%)
	$(AR) $(AFLAGS) $@ $^

test:
#	$(CC) $(CFLAGS) $(SOURCE_DIR)/$(TEST) -o $(BUILD_DIR)/$(TESTOBJ)
#	$(LD) $(LFLAGS) $(BUILD_DIR)/$(TESTOBJ) $(BUILD_DIR)/$(LIB) -o $(BUILD_DIR)/$(TESTEXE)
	$(CC) $(CFLAGS) $(SOURCE_DIR)/$(TEST) -o $(BUILD_DIR)/$(TESTOBJ)
	$(LD) $(LFLAGS) $(BUILD_DIR)/$(TESTOBJ) -o $(BUILD_DIR)/$(TESTEXE)

run:
	"$(BUILD_DIR)/$(TESTEXE)"

clean:
	$(RM) $(RFLAGS) $(BUILD_DIR)\*
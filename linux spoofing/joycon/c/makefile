
CC         = gcc
CFLAGS     = -c -Wall
LDFLAGS    = 
LDLIBS     = -lbluetooth
EXECUTABLE = joyconspoof
SRC        = main.c register.c
OBJ_DIR    = build
PRECOMPILED_OBJECTS = 

OBJECT_FILES = $(SRC:%.c=$(OBJ_DIR)/%.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES) $(PRECOMPILED_OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo "\nCompiled as $(EXECUTABLE)"

$(OBJECT_FILES): $(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $< 

.PHONY: clean

clean:
	@rm -f $(OBJECT_FILES) $(EXECUTABLE)
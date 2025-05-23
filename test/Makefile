SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))
EXE := main_nolib

all: dircheck $(EXE)
$(EXE): $(OBJ)
	cc -o $@ $^
$(OBJ): build/%.o: src/%.c
	cc -c -o $@ $^
dircheck:
	@mkdir -p build
	@echo two liner
clean:
	rm -f $(EXE) $(OBJ)
.PHONY: dircheck all clean
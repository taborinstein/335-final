SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, %.o, $(SRC))
EXE := main

$(EXE): $(OBJ)
	cc -o $@ $^
$(OBJ): %.o: %.c
	cc -c -o $@ $^
clean:
	rm -f $(EXE) $(OBJ)
.PHONY: all clean

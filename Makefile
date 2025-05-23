EXE := pmk
CC := $(shell which mpicc)

$(EXE): src/main.c
	$(CC) -o $@ $^
clean:
	@rm $(EXE)
.PHONY: clean
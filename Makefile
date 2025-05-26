EXE := pmk
CC := $(shell which mpicc)
all: dircheck $(EXE)
$(EXE): build/main.o build/pmk_reader.o build/process.o
	$(CC) -o $@ $^

build/main.o: src/main.c
	$(CC) -c -o $@ $^
build/pmk_reader.o: src/pmk_reader.c
	$(CC) -c -o $@ $^
build/process.o: src/process.c
	$(CC) -c -o $@ $^

clean:
	@rm -rf build $(EXE)
dircheck:
	@mkdir -p build
.PHONY: clean dircheck
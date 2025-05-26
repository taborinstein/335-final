# pmk - parallel make
`pmk` is a utility for building projects using multiple processors  
 
## Usage
In a project with a valid Makefile, run `python3 parse.py` to generate `build.pmk`. You only need to generate this once (unless you change your makefile/build files). After that, simply run `pmk` for a single processor or `mpirun -np 4 pmk` (change 4 to however many processors you want).  

## Examples
```sh
$ cd test_long

# generate test files
$ sh gen.sh 

# generate build.pmk
$ python3 ../parse.py

# run pmk with 4 processors
$ mpirun -np 4 ../pmk
[0] cc -c -o _100.o _100.c
[2] cc -c -o _57.o _57.c
[1] cc -c -o _33.o _33.c
[3] cc -c -o _7.o _7.c
[1] cc -c -o _34.o _34.c
[3] cc -c -o _80.o _80.c
[0] cc -c -o _10.o _10.c
[2] cc -c -o _58.o _58.c
...
[0] cc -c -o _30.o _30.c
[2] cc -c -o _79.o _79.c
[1] cc -c -o _55.o _55.c
[3] cc -c -o main.o main.c
[0] cc -c -o _31.o _31.c
[1] cc -c -o _56.o _56.c
[0] cc -c -o _32.o _32.c
[3] cc -o main _100.o _10.o _11.o _12.o _13.o _14.o _15.o _16.o ... _94.o _95.o _96.o _97.o _98.o _99.o _9.o main
```
```sh
# pmk can build itself
$ python3 parse.py
$ mpirun -np 4 ./pmk
[2] /opt/openmpi/bin/mpicc -c -o build/process.o src/process.c
[0] mkdir -p build
[1] /opt/openmpi/bin/mpicc -c -o build/pmk_reader.o src/pmk_reader.c
[0] /opt/openmpi/bin/mpicc -c -o build/main.o src/main.c
[3] /opt/openmpi/bin/mpicc -o pmk build/main.o build/pmk_reader.o build/process.o
```

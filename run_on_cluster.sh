route=borinsta@grendel.csse.rose-hulman.edu
scp -qr $(find -maxdepth 1 |  grep -vE -e '^\./\.' -e '^\.$') $route:pmk_run
ssh $route -X 'cd pmk_run && make clean && echo "make" && make && cd test_long && echo -e "GO" && mpirun -n 4 ../pmk'
route=borinsta@grendel.csse.rose-hulman.edu
scp -qr . $route:pmk_run
ssh $route -X 'cd pmk_run && make clean && echo "make" && make && cd test && echo -e "GO" && mpirun -n 4 ../pmk'
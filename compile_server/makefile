compile_server:compile_server.cc compile_run.pb.cc
	g++ -o $@  $^  --std=c++11 -ljsoncpp -lpthread -lprotobuf

.PHONY:clean
clean:
	rm -f compile_server
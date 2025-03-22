oj_server:oj_server.cpp server.pb.cc
	g++ -o $@ $^ -std=c++11 -lpthread -lctemplate -ljsoncpp -lmysqlclient -lprotobuf

.PHONY:clean
clean:
	rm -f oj_server
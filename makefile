server:
	g++ -o server webserver.cpp task.cpp server.cpp ThreadPool.h -lpthread -std=c++11

clean:
	rm -rf  server

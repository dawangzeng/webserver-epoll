server:
	g++ -o  server -g  webserver.cpp task.cpp server.cpp ThreadPool.h -lpthread 
clean:
	rm -rf  server

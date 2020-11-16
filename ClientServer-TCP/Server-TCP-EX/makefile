all: server.exe
  
clean:
	rm server.o server.exe
  
server.exe: server.o
	g++ -g -o server.exe server.o
  
server.o: server.c
	g++ -c -g server.c
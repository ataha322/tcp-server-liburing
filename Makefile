all:
	g++ main.cpp server.cpp -I liburing/src/include/ -L liburing/src/ -Wall -O2 -D_GNU_SOURCE -luring

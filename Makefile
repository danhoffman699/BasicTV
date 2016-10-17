CXXFLAGS+=-Wall -Wextra -std=c++11
LDFLAGS+=-lstdc++ -lcurl -lSDL2_net -lSDL2
CXX?=g++

all:
	$(CC) $(LDFLAGS) */*.cpp *.cpp $(CFLAGS) 

debug:
	$(CC) $(LDFLAGS) */*.cpp *.cpp $(CFLAGS) -O0 -g

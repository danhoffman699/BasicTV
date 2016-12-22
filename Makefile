CXXFLAGS+=-Wall -Wextra -std=c++11 -Wpedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-old-style-cast
LDFLAGS+=-lstdc++ -lcurl -lSDL2_net -lSDL2 -lz -ISDL -ISDL2
#changed for personal reasons, clang++ is faster at compiling
CXX:=clang++

default: debug

id:
	$(CXX) $(CXXFLAGS) -c id/id_unified.cpp

tv: id
	$(CXX) $(CXXFLAGS) -c tv/tv_unified.cpp

net: id
	$(CXX) $(CXXFLAGS) -c net/net_unified.cpp

input: id
	$(CXX) $(CXXFLAGS) -c input/input_unified.cpp

encrypt: id
	$(CXX) $(CXXFLAGS) -c encrypt/encrypt_unified.cpp

all: tv net input encrypt
	$(CXX) $(CXXFLAGS) net_unified.o tv_unified.o id_unified.o encrypt_unified.o *.cpp $(LDFLAGS) 

debug: tv net input encrypt
	$(CXX) $(CXXFLAGS) -DDEBUG -O0 -g tv_unified.o net_unified.o input_unified.o id_unified.o encrypt_unified.o *.cpp $(LDFLAGS) 

fast: tv net input encrypt
	$(CXX) $(CXXFLAGS) -Ofast -march=native tv_unified.o net_unified.o input_unified.o id_unified.o encrypt_unified.o *.cpp $(LDFLAGS)

clean:
	rm input_unified.o tv_unified.o net_unified.o id_unified.o encrypt_unified.o a.out id/*~ tv/*~ net/*~ net/proto/*~ encrypt/*~ *~

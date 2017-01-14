CXXFLAGS+=-Wall -Wextra -std=c++11 -Wpedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-old-style-cast
LDFLAGS+=-lstdc++ -lcurl -lSDL2_net -lSDL2_mixer -lSDL2 -lz -lcrypto
#changed for personal reasons, clang++ is faster at compiling
CXX:=clang++

all: debug

id_: input_
	$(CXX) $(CXXFLAGS) -DDEBUG -O0 -g -c id/id_unified.cpp

tv_: id_
	$(CXX) $(CXXFLAGS) -DDEBUG -O0 -g -c tv/tv_unified.cpp

net_: id_
	$(CXX) $(CXXFLAGS) -DDEBUG -O0 -g -c net/net_unified.cpp

input_: id_
	$(CXX) $(CXXFLAGS) -DDEBUG -O0 -g -c input/input_unified.cpp

encrypt_: id_
	$(CXX) $(CXXFLAGS) -DDEBUG -O0 -g -c encrypt/encrypt_unified.cpp

debug: tv_ net_ input_ encrypt_ id_
	$(CXX) $(CXXFLAGS) -DDEBUG -O0 -g  *.cpp tv_unified.o net_unified.o input_unified.o id_unified.o encrypt_unified.o $(LDFLAGS) 

fast: tv_ net_ input_ encrypt_
	$(CXX) $(CXXFLAGS) -Ofast -march=native tv_unified.o net_unified.o input_unified.o id_unified.o encrypt_unified.o *.cpp $(LDFLAGS)

clean:
	rm input_unified.o tv_unified.o net_unified.o id_unified.o encrypt_unified.o a.out id/*~ tv/*~ net/*~ net/proto/*~ encrypt/*~ *~

CXXFLAGS+=-Wall -Wextra -std=c++11 -Wpedantic -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-old-style-cast -O0 -g
LDFLAGS+=-lstdc++ -lcurl -lSDL2_net -lSDL2
#changed for personal reasons, clang++ is faster at compiling
CXX=clang++

default: debug

id:
	$(CXX) $(CXXFLAGS) -c id/id_unified.cpp

tv: id
	$(CXX) $(CXXFLAGS) -c tv/tv_unified.cpp

net: id
	$(CXX) $(CXXFLAGS) -c net/net_unified.cpp

all: tv net
	$(CXX) $(CXXFLAGS) net_unified.o tv_unified.o id_unified.o *.cpp $(LDFLAGS) 

debug: tv net
	$(CXX) $(CXXFLAGS) -O0 -g tv_unified.o net_unified.o id_unified.o *.cpp $(LDFLAGS) 

clean:
	rm tv_unified.o net_unified.o id_unified.o a.out

CXXFLAGS+=-Wall -Wextra -std=c++11
LDFLAGS+=-lstdc++ -lcurl -lSDL2_net -lSDL2
#changed for personal reasons, clang++ is faster at compiling
CXX=clang++

all:
	$(CXX) $(CXXFLAGS) */*.cpp *.cpp $(LDFLAGS) 

debug:
	$(CXX) $(CXXFLAGS) -O0 -g */*.cpp *.cpp $(LDFLAGS) 

CXXFLAGS += -Wall -Wextra -std=c++11 -Wno-unused-function
LDLIBS = -lcurl -lSDL2_net -lSDL2_mixer -lSDL2 -lz -lcrypto -lopus
CPPFLAGS = -DDEBUG

SRC = $(shell find . -name '*.cpp')
DEP = .depend

all: basictv

%.o: %.cpp
	@mkdir -p $(DEP)/$(@D)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MMD -MF $(DEP)/$*.d -c $< -o $@
	@echo -e "CXX\t$<"

basictv: $(SRC:.cpp=.o)
	@$(CXX) $^ -o $@ $(LDLIBS)
	@echo -e "LD\t$@"

clean:
	$(RM) $(SRC:.cpp=.o) basictv
	$(RM) -r $(DEP)

.PHONY: all clean

.PRECIOUS: $(DEP)/%.d

-include $(addprefix $(DEP)/, $(SRC:.cpp=.d))

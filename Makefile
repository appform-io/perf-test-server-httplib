RM=rm -f
CPPFLAGS=-Ivendors/cpp-httplib -Ivendors/spdlog/include
CXXFLAGS=-O4 -Wall -Werror
LDLIBS=-lpthread

SRC_DIR=src

EXE := test-server

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=%.o)

SRCS=src/service.cpp

OBJS=$(subst .cpp,.o,$(SRCS))

.PHONY: all clean


all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) -static $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) -rv $(EXE) $(OBJ)

-include $(OBJ:.o=.d)


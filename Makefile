CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O -g
LDFLAGS =
SRC_DIRS := .
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -print)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)
TARGET := assembler

.PHONY: all clean distclean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .cpp to .o and generate dependency .d
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS)

distclean: clean
	rm -f $(TARGET)

run: all
	./$(TARGET)

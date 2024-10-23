CXX = g++

CXXFLAGS = -I~/rf24libs/RF24 -I~/rf24libs/RF24Network -I~/rf24libs/RF24Mesh

ALDFLAGS = -L~/rf24libs/RF24/build -L~/rf24libs/RF24Network/build -L~/rf24libs/RF24Mesh/build

LDLIBS = -lrf24 -lrf24network -lrf24mesh

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = main

all:$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

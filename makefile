CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 $(shell sdl2-config --cflags)
LDFLAGS  = $(shell sdl2-config --libs)

TARGET = Game
SOURCE = Game.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
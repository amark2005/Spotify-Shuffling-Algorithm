CXX = g++
CXXFLAGS = -O2
LIBS = -lraylib -lraygui -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = engine
SRC = engine.cpp

all: run

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean

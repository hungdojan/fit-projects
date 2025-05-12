CXX=mpic++
CXXFLAGS=-std=c++11 -Wall -Wextra -O2
TARGET=life
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(TARGET)

run: $(TARGET)
	mpirun -np $$(($$(wc -l < ./test_boards/test1.txt) / 2)) $< ./test_boards/test1.txt 4

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf *.o $(TARGET)

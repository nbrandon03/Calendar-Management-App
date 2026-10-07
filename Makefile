# Compiler used to build the program
CXX = g++

# Compiler flags for wxWidgets and C++17 standard
CXXFLAGS = `wx-config --cxxflags` -std=c++17

# Linker flags for wxWidgets
LDFLAGS = `wx-config --libs`

# Source files and object files
SRC = main.cpp Calendar.cpp CalendarView.cpp Date.cpp Event.cpp Settings.cpp WeatherAPI.cpp CheckList.cpp Task.cpp
OBJ = $(SRC:.cpp=.o)

# Name of the executable program
TARGET = clearday

# Compiles and links all source files to create the executable
all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean up the build directory by removing object files and the executable
clean:
	rm -f *.o $(TARGET)
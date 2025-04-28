CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -Iinclude
LDFLAGS = -pthread

# Source files
SOURCES = src/main.cpp src/data_loader.cpp src/query_processor.cpp src/thread_pool.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable
TARGET = tpch_query5

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install
install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/local/bin
	cp $(TARGET) $(DESTDIR)/usr/local/bin/

# Uninstall
uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall
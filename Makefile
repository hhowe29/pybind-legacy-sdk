PYTHON_INCLUDES = $(shell python3 -m pybind11 --includes)
APP_SUFFIX = $(shell python3-config --extension-suffix)
CC = g++
CXXFLAGS = -Wall -std=c++14 -fPIC -g -D_GLIBCXX_USE_CXX11_ABI=0 $(PYTHON_INCLUDES)
LFLAGS = -shared
LIBS =
SRCS = ./example.cpp
OBJS = $(SRCS:.cpp=.o)
MAIN = example$(APP_SUFFIX)

.PHONY: depend clean

all:    $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	@echo "Compiling"
	@echo "$(PYTHON_INCLUDES)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.cpp.o:
	@echo "Compiling"
	@echo $(PYTHON_INCLUDES)
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o $(MAIN)
CC = g++
CFLAGS = -Wall
EXEC_NAME = Chat Server
INCLUDES =
LIBS = -pthread
OBJ_FILES = chat server
INSTALL_DIR = 
CXXFLAGS  = -Wall $(DEBUG) -std=c++11 -pthread

all : $(EXEC_NAME)

clean :
	rm $(EXEC_NAME) $(OBJ_FILES)

$(EXEC_NAME) : $(OBJ_FILES)

%.o: %.cpp
	$(CC) $(CFLAGS)  -o $@ -c $< $(INCLUDES)

install :
	cp $(EXEC_NAME) $(INSTALL_DIR)

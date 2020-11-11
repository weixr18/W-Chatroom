CC:= g++
INCLUDE:= -I./ 
LIBS:= -lstdc++ -lwsock32
CXXFLAGS:= -std=c++11 -g
DIR_SRC:= ./src
DIR_OBJ:= ./obj
DIR_OBJ_WIN:= .\obj
SERVER_TARGET:= Server.exe
CLIENT_TARGET:= Client.exe
SERVER_OBJECTS := Server.o ThreadPool.o ServerTask.o InfoQueue.o 
CLIENT_OBJECTS := Client.o

SERVER_OBJECTS := $(addprefix $(DIR_OBJ)/,$(SERVER_OBJECTS))
CLIENT_OBJECTS := $(addprefix $(DIR_OBJ)/,$(CLIENT_OBJECTS))


all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(shell mkdir obj)

$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	$(CC) -o $(CLIENT_TARGET) $(CLIENT_OBJECTS) $(LIBS)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(CC) -o $(SERVER_TARGET) $(SERVER_OBJECTS) $(LIBS)
  
# $@表示所有目标集  
$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp   
	$(CC) -c $(CXXFLAGS) $(INCLUDE) $< -o $@
  
.PHONY : clean
clean:   
	-del $(DIR_OBJ_WIN)\*.o
	-del $(SERVER_TARGET) 
	-del $(CLIENT_TARGET)
	rmdir obj


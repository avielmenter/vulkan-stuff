CXX=g++
CXXFLAGS=-std=gnu++14 -I$(VULKAN_SDK)/include

LIBS = -L$(VULKAN_SDK)/lib `pkg-config --static --libs glfw3` -lvulkan
DEFS = -DDEBUG

EXECUTABLE=vk

OBJ_FOLDER=obj
SRC_FOLDER=src

OBJECTS=$(OBJ_FOLDER)/main.o $(OBJ_FOLDER)/VulkanApplication.o

all: $(EXECUTABLE)

run: all
	./$(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FOLDER) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LIBS)

$(OBJ_FOLDER):
	mkdir $(OBJ_FOLDER)

$(OBJ_FOLDER)/main.o: $(SRC_FOLDER)/main.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_FOLDER)/main.cpp -o $(OBJ_FOLDER)/main.o

$(OBJ_FOLDER)/VulkanApplication.o: $(SRC_FOLDER)/VulkanApplication.cpp $(SRC_FOLDER)/VulkanApplication.h $(SRC_FOLDER)/vk/VkDeleter.h
	$(CXX) $(CXXFLAGS) $(DEFS) -c $(SRC_FOLDER)/VulkanApplication.cpp -o $(OBJ_FOLDER)/VulkanApplication.o

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)

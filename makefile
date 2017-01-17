CXX=g++
CXXFLAGS=-std=gnu++14 -I$(VULKAN_SDK)/include -ggdb

GLSL=glslangValidator
GLSLFLAGS=-V

LIBS = -L$(VULKAN_SDK)/lib `pkg-config --static --libs glfw3` -lvulkan
DEFS = -DDEBUG

EXECUTABLE=vk

OBJ_FOLDER=obj
SRC_FOLDER=src
SHADERS_FOLDER=$(OBJ_FOLDER)/shaders

OBJECTS=$(OBJ_FOLDER)/main.o $(OBJ_FOLDER)/VulkanApplication.o $(OBJ_FOLDER)/Device.o $(OBJ_FOLDER)/SwapChain.o
SHADERS=$(SHADERS_FOLDER)/triangle_vert.spv $(SHADERS_FOLDER)/triangle_frag.spv

all: $(EXECUTABLE)

run: all
	./$(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FOLDER) $(OBJECTS) $(SHADERS_FOLDER) $(SHADERS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LIBS)

$(OBJ_FOLDER):
	mkdir $(OBJ_FOLDER)

$(SHADERS_FOLDER):
	mkdir $(SHADERS_FOLDER)

$(SHADERS_FOLDER)/triangle_vert.spv: $(SRC_FOLDER)/shaders/triangle.vert
	$(GLSL) $(GLSLFLAGS) $(SRC_FOLDER)/shaders/triangle.vert -o $(SHADERS_FOLDER)/triangle_vert.spv

$(SHADERS_FOLDER)/triangle_frag.spv: $(SRC_FOLDER)/shaders/triangle.frag
	$(GLSL) $(GLSLFLAGS) $(SRC_FOLDER)/shaders/triangle.frag -o $(SHADERS_FOLDER)/triangle_frag.spv

$(OBJ_FOLDER)/main.o: $(SRC_FOLDER)/main.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_FOLDER)/main.cpp -o $(OBJ_FOLDER)/main.o

$(OBJ_FOLDER)/VulkanApplication.o: $(SRC_FOLDER)/VulkanApplication.cpp $(SRC_FOLDER)/VulkanApplication.h $(SRC_FOLDER)/vk/VkDeleter.h
	$(CXX) $(CXXFLAGS) $(DEFS) -c $(SRC_FOLDER)/VulkanApplication.cpp -o $(OBJ_FOLDER)/VulkanApplication.o

$(OBJ_FOLDER)/Device.o: $(SRC_FOLDER)/vk/Device.cpp $(SRC_FOLDER)/vk/Device.h $(SRC_FOLDER)/vk/VkDeleter.h
	$(CXX) $(CXXFLAGS) $(DEFS) -c $(SRC_FOLDER)/vk/Device.cpp -o $(OBJ_FOLDER)/Device.o

$(OBJ_FOLDER)/SwapChain.o: $(SRC_FOLDER)/vk/SwapChain.cpp $(SRC_FOLDER)/vk/SwapChain.h $(SRC_FOLDER)/vk/Device.h $(SRC_FOLDER)/vk/VkDeleter.h
	$(CXX) $(CXXFLAGS) $(DEFS) -c $(SRC_FOLDER)/vk/SwapChain.cpp -o $(OBJ_FOLDER)/SwapChain.o

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS) $(SHADERS)

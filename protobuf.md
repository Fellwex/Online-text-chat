### **The file contains tutorial how to build and add "Protobuf" to the project**

**To build "Protobuf", the following tools are needed:**  

* CMake
* Git

**How to build:**  

1. Run Git and enter the command:  
   "git clone -b v3.20.0 https://github.com/protocolbuffers/protobuf.git", where v3.20.0 - Protobuf version
   
2. Change your current directory to "protobuf" and execute git command:    
   "git submodule update --init --recursive"  
   
3. Create subdirectory in this folder using the command:    
   mkdir build-your-platform-release   
   
4. Change your current directory to "build-your-platform-release" and execute the following CMake command:     
   cmake -G "Your Generator" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX = path\to\protobuf\install-your-platform-release 
   path\to\protobuf\cmake   
   
5. Finally, execute CMake command:    
   cmake --build . --config Release --target install
   
**How to add to the project:**    

1. Create new folder with the name of your platform and add all the subfolders from folder "install-your-platform-release"       
   Your folders structure should look like in the following example (Example: Adding "protobuf" to the project "server"): 
   ``` 
   |   CMakeLists.txt         
   |                             
   +---libs       
   |   +---protobuf        
   |       \---win         
   |           +---bin
   |           |             
   |           +---include
   |           |                  
   |           \---lib                                       
   \---src
       +---client
       |       CMakeLists.txt
       |       main.cpp
       |       
       +---common
       |       CMakeLists.txt
       |       common.cpp
       |       
       \---server
               CMakeLists.txt
               test.proto
               main.cpp
   ```
2. Add next lines in your CMake file:    
``` cmake
set(PRT_PATH ${PROJECT_SOURCE_DIR}/path/to/protobuf)
set(Protobuf_PREFIX_PATH ${PRT_PATH}/platform/include ${PRT_PATH}/platform/lib ${PRT_PATH}/platform/bin)
list(APPEND CMAKE_PREFIX_PATH "${Protobuf_PREFIX_PATH}")
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB protobuf_SRC CONFIGURE_DEPENDS "*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${protobuf_SRC})

add_executable(Project ${project_SRC} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(Project ${Protobuf_LIBRARIES})
```
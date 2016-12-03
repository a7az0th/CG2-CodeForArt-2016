cmake_minimum_required (VERSION 3.7)

project (cg2_Demo)

set(CMAKE_VERBOSE_MAKEFILE on)   

#Used throughout the projects to search the sdk for the proper libs
set (PLATFORM x86)
string(FIND ${CMAKE_GENERATOR} Win64 position)
if (NOT (${position} EQUAL "-1"))
       set(PLATFORM x64)
endif()

if(CMAKE_CONFIGURATION_TYPES)      
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
endif()

set(SDK_PATH ${CMAKE_SOURCE_DIR}/sdk)
find_package(wxWidgets REQUIRED gl core base)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(CUDA)
if (CUDA_FOUND) 
	# Tell our C++ compiler to compile the CUDA stuff
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DALLOW_CUDA")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DALLOW_CUDA")	
else()
	message(WARNING "CUDA Toolkit was not found. Code will be compiled as standard C++ with CUDA disabled")
endif()

add_subdirectory (Filtering)
add_subdirectory (UserInterface)
add_dependencies(cg2_demo Filtering)
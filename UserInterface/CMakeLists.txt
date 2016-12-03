cmake_minimum_required (VERSION 3.7)
project (cg2_demo)

set(PROJECT_LINK_LIBS Filtering.lib FreeImage.lib glew32.lib ${wxWidgets_LIBRARIES} ${CUDA_LIBRARIES})

link_directories(
	${SDK_PATH}/Filtering/${PLATFORM}/lib
	${SDK_PATH}/FreeImage/${PLATFORM}/lib
	${SDK_PATH}/glew_2.0.0/${PLATFORM}/lib
	)

#Bring the headers into the project
include_directories(
	include
	${SDK_PATH}/glew_2.0.0/${PLATFORM}/include
	${SDK_PATH}/Filtering/${PLATFORM}/include
	${wxWidgets_INCLUDE_DIRS}
	)

file(GLOB SOURCES "src/*.cpp")

add_executable(cg2_demo ${SOURCES})
target_link_libraries(cg2_demo ${PROJECT_LINK_LIBS} )

IF(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
	set_target_properties(cg2_demo PROPERTIES LINK_FLAGS "/subsystem:windows")
  else()
    message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
  endif()
else()
  message(SEND_ERROR "You are on an unsupported platform! (Not Win32)")
ENDIF()

add_custom_command(TARGET cg2_demo POST_BUILD         # Adds a post-build event to Filtering
    COMMAND ${CMAKE_COMMAND} -E copy_if_different      # which executes "cmake - E copy_if_different..."
        "${SDK_PATH}/glew_2.0.0/${PLATFORM}/bin/glew32.dll"  # <--this is in-file
        $<TARGET_FILE_DIR:cg2_demo>) # <--this is out-file path

add_custom_command(TARGET cg2_demo POST_BUILD         # Adds a post-build event to Filtering
    COMMAND ${CMAKE_COMMAND} -E copy_if_different      # which executes "cmake - E copy_if_different..."
        "${SDK_PATH}/FreeImage/${PLATFORM}/bin/FreeImage.dll"  # <--this is in-file
        $<TARGET_FILE_DIR:cg2_demo>) # <--this is out-file path
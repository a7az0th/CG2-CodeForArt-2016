cmake_minimum_required (VERSION 3.7)
project (ImageFiltering)

if (CUDA_FOUND)
   set(CUDA_VERBOSE_BUILD on)
	# Pass options to NVCC
	set(CUDA_NVCC_FLAGS_DEBUG ${CUDA_NVCC_FLAGS_DEBUG} -O0 -G -lineinfo)
	set(CUDA_NVCC_FLAGS_RELEASE ${CUDA_NVCC_FLAGS_RELEASE} -O3)
	set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -DALLOW_CUDA")
	#You may need to explicitly specify the CC of your card here
	
	set(CUDA_INCLUDE_DIRS 
		${CUDA_INCLUDE_DIRS} 
		include
		cuda
		${SDK_PATH}/glew_2.0.0/${PLATFORM}/include
		${SDK_PATH}/FreeImage/${PLATFORM}/include
		)
	
	set(SOURCE_FILES
		src/Color.cpp
		src/cpuFilter.cpp
		src/cudaFilter.cpp
		src/filtering.cpp
		src/image.cpp
		cuda/kernel.cu
	)
	
	cuda_add_library(Filtering STATIC ${SOURCE_FILES})
	
else() # NVCC was not found, compile without CUDA support

	#Bring the headers into the project
	include_directories(
		include
		${SDK_PATH}/glew_2.0.0/${PLATFORM}/include
		${SDK_PATH}/FreeImage/${PLATFORM}/include
		)
	#The file(GLOB...) allows for wildcard additions:
	file(GLOB SOURCES "src/*.cpp")
	
	#Generate the static library from the sources
	add_library(Filtering STATIC ${SOURCES})

endif()

# Copy the necessary headers to use the library into the SDK
add_custom_command(TARGET Filtering POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${PROJECT_SOURCE_DIR}/include/image.h ${SDK_PATH}/Filtering/${PLATFORM}/include/image.h
)
add_custom_command(TARGET Filtering POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	${PROJECT_SOURCE_DIR}/include/Color.h ${SDK_PATH}/Filtering/${PLATFORM}/include/Color.h
)
add_custom_command(TARGET Filtering POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	${PROJECT_SOURCE_DIR}/include/demoFilters.h ${SDK_PATH}/Filtering/${PLATFORM}/include/demoFilters.h
)

# Copy the library to the SDK
add_custom_command(TARGET Filtering POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
	$<TARGET_FILE_DIR:Filtering>/Filtering.lib ${SDK_PATH}/Filtering/${PLATFORM}/lib/Filtering.lib
)

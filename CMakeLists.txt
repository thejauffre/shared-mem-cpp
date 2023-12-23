cmake_minimum_required(VERSION 3.16.3)

project(shared_memory_example)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(OpenCV 4.6.0 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

add_executable (main_1 src/main_1.cpp )
target_link_libraries(main_1 ${Boost_LIBRARIES} ${OpenCV_LIBS})

add_executable (main_2 src/main_2.cpp )
target_link_libraries(main_2 ${Boost_LIBRARIES} ${OpenCV_LIBS})

add_executable (main_3 src/main_3.cpp )
target_link_libraries(main_3 ${Boost_LIBRARIES} ${OpenCV_LIBS})
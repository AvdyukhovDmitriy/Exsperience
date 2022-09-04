cmake_minimum_required(VERSION 3.0)

project(VectorAndOptional)
set(CMAKE_CXX_STANDARD 17)

set(FILES_PROJECT src/main.cpp 
                  src/optional.h
				  src/test.h
				  src/vector.h)
				  
add_executable(vector_and_optional ${FILES_PROJECT})
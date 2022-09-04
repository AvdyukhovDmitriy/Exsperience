cmake_minimum_required(VERSION 3.0)

project(Patterns)
set(CMAKE_CXX_STANDARD 17)

set(FILES_PROJECT src/main.cpp 
                  src/structural/adapter_wrapper.h 				  
				  src/structural/fasade.h
				  src/structural/decorator.h

				  src/generative/factory_method.h
				  src/generative/object_pool.h
				  src/generative/builder.h
				  src/generative/prototype.h
				  src/generative/singleton.h src/generative/singleton.cpp
				  src/generative/abstract_factory.h

				  src/behavior/visitor.h
				  src/behavior/command.h
				  src/behavior/iterator.h src/behavior/iterator.cpp
				  src/behavior/mediator.h
				  src/behavior/obsever.h
				  src/behavior/state.h src/behavior/state.cpp
				  src/behavior/strategy.h
				  src/behavior/chain_of_responsibility.h)
				  
add_executable(patterns ${FILES_PROJECT})
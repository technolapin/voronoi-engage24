cmake_minimum_required(VERSION 3.9)

project(c2ga CXX)
set(CMAKE_BUILD_TYPE Release)

# include eigen
find_package(Eigen3 REQUIRED)
if(${EIGEN3_FOUND})
    message(STATUS "lib EIGEN3 found")
    message(STATUS "  version " ${EIGEN3_VERSION_STRING})
    message(STATUS "  include " ${EIGEN3_INCLUDE_DIR})
else()
    include_directories("/usr/include/eigen3") # manually specify the include location
endif()


# python binding
#option(BUILD_PYTHON "Build Python library" OFF)
if (BUILD_PYTHON)
    find_package(pybind11 REQUIRED)
endif()


# call the CMakeLists.txt to make the documentation (Doxygen)
# > 'make html' to generate the documentation
find_package(Doxygen OPTIONAL_COMPONENTS QUIET)
if(DOXYGEN_FOUND)
    add_subdirectory(doc)
else()
    message(WARNING "Doxygen not found, skip dococumentation")
endif()


# files to compile
set(source_files src/c2ga/Mvec.cpp)
file(GLOB_RECURSE header_files src/c2ga/*.hpp)

# display info
message(STATUS "  sources")
foreach(src_file ${source_files})
    message("          " ${src_file})
endforeach()
message(STATUS "  headers")
foreach(src_file ${header_files})
    message("          " ${src_file})
endforeach()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# generate a library
if (MSVC)
	add_library(c2ga STATIC ${source_files} ${header_files})
else()
	add_library(c2ga SHARED ${source_files} ${header_files})
endif()

if (BUILD_PYTHON)
    pybind11_add_module(c2ga_py 
        src/c2ga/PythonBindings.cpp
    )
    target_link_libraries(c2ga_py PRIVATE 
        c2ga)
endif()

# compilation flags
if (MSVC)   
    target_compile_features(c2ga PRIVATE cxx_std_14) 
    target_compile_options(c2ga PRIVATE -Wall -O2) 
else()
    target_compile_features(c2ga PRIVATE cxx_std_14) 
    target_compile_options(c2ga PRIVATE -Wall -O2 -Wno-return-local-addr)
endif()
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-report ") # comilation profiling:  -Q -ftime-report
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-elide-constructors") # force the compilor to use our move 'copy constructor'

# include directory path
# THIS HAS BEEN CHANGED INTO TARGET THINGY (keywords for me to find this with a text search: CUSTOM FIX MODERN WORK)
target_include_directories(c2ga PRIVATE src)
target_include_directories(c2ga PUBLIC src ${EIGEN3_INCLUDE_DIR})

# install lib
install(FILES ${header_files} ${source_files} DESTINATION /usr/local/include/c2ga)
install(TARGETS c2ga
        RUNTIME DESTINATION /usr/local/bin
        LIBRARY DESTINATION /usr/local/lib
        ARCHIVE DESTINATION /usr/local/lib)


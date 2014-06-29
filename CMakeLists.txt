cmake_minimum_required(VERSION 2.8)
find_package( Freetype )

set(CMAKE_BUILD_TYPE Debug)

add_executable( example1 example1.cpp font_to_svg.hpp )
add_executable( example2 example2.cpp font_to_svg.hpp )
add_executable( example3 example3.cpp font_to_svg.hpp )
add_executable( example4 example4.cpp font_to_svg.hpp )

include_directories( ${FREETYPE_INCLUDE_DIRS} )
target_link_libraries( example1 ${FREETYPE_LIBRARIES} )
target_link_libraries( example2 ${FREETYPE_LIBRARIES} )
target_link_libraries( example3 ${FREETYPE_LIBRARIES} )
target_link_libraries( example4 ${FREETYPE_LIBRARIES} )


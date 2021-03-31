cmake_minimum_required(VERSION 3.0.0)
project(set_gt VERSION 0.1.0)

include(CTest)
enable_testing()

#試著找到OpenCV檔
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(set_gt main.cpp)

# target_link_libraries(mosse_test_flow cvplot)
target_link_libraries(set_gt ${OpenCV_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

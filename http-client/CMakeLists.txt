cmake_minimum_required(VERSION 3.20)
project(Spider VERSION 0.1 LANGUAGES CXX)

set(SOURCE_F
    ${CMAKE_SOURCE_DIR}/includes/parser/parser.cpp
    ${CMAKE_SOURCE_DIR}/includes/db/database.cpp
)

set(HEADER_F
    ${CMAKE_SOURCE_DIR}/includes/parser/parser.hpp
    ${CMAKE_SOURCE_DIR}/includes/db/database.hpp
    ${CMAKE_SOURCE_DIR}/includes/helper/help.hpp
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp ${SOURCE_F} ${HEADER_F})

target_link_libraries(${PROJECT_NAME} pqxx)

target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
include_directories(include)

file(GLOB HEADERS ./include/*.h)
file(GLOB SOURCE_FILES ./*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

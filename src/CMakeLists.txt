file(GLOB SOURCES *.c)

add_library(bej STATIC ${SOURCES})
target_include_directories(bej
  PUBLIC ${CMAKE_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(bej-parser main.c)
target_link_libraries(bej-parser PRIVATE bej)

install(TARGETS bej bej-parser
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
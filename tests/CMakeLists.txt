file(COPY ${CMAKE_SOURCE_DIR}/tests/dummy_data/ DESTINATION ${CMAKE_BINARY_DIR}/tests/dummy_data)
file(COPY ${CMAKE_SOURCE_DIR}/tests/dummy_dictionaries/ DESTINATION ${CMAKE_BINARY_DIR}/tests/dummy_dictionaries)

add_executable(test_json_writer test_json_writer.c)
target_link_libraries(test_json_writer bej unity)
add_test(NAME TestJsonWriter COMMAND test_json_writer)

add_executable(test_decode test_decode.c)
target_link_libraries(test_decode bej unity)
add_test(NAME TestDecode COMMAND test_decode)

add_executable(test_dictionary test_dictionary.c)
target_link_libraries(test_dictionary bej unity)
add_test(NAME TestDictionary COMMAND test_dictionary)

add_executable(test_stream_utils test_stream_utils.c)
target_link_libraries(test_stream_utils bej unity)
add_test(NAME TestStreamUtils COMMAND test_stream_utils)
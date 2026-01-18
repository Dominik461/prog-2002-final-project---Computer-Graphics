add_executable(Exam
    main.cpp
)

# Link Lab3Test with the Lab3Application library
target_link_libraries(Exam
    GameLib
)

# Set output directory for the test executable
set_target_properties(Exam PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_command(
  TARGET Exam POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/cube_texture.jpg
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/cube_texture.jpg
)

add_custom_command(
  TARGET Exam POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/wall_texture.png
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/wall_texture.png
)
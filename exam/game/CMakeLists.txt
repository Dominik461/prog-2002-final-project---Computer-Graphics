add_library(GameLib
    Game.cpp Game.h
    Block.h 
    BlockData.h
    ComplexBlock.cpp ComplexBlock.h
)

# Include directories for the library
target_include_directories(GameLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/framework/GLFWApplication
    ${CMAKE_SOURCE_DIR}/framework/GeometricTools
    ${CMAKE_SOURCE_DIR}/framework/Rendering
    ${CMAKE_SOURCE_DIR}/framework/Projection
)

# Link Lab3Application with required dependencies
target_link_libraries(GameLib
    Renderer
    GLFWApplication
    GeometricTools
    Rendering
    Projection
    glad
    glfw
    stb
    glm
)

target_compile_definitions(GameLib PRIVATE
  TEXTURES_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/")

target_compile_definitions(GameLib PRIVATE STB_IMAGE_IMPLEMENTATION)


#add_custom_command(
#  TARGET Practice POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy
#  ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/cube_texture.jpg
#  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/cube_texture.jpg
#)
#
#add_custom_command(
#  TARGET Practice POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy
#  ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/floor_texture.jpg
#  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/floor_texture.jpg
#)
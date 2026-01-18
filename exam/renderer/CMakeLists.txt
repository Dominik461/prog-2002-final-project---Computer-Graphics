add_library(Renderer 
	ShaderDefenitionsTunnel.h
	ShaderDefenitionsBlocks.h
    
    Renderer.h Renderer.cpp
)

# Include directories for the library
target_include_directories(Renderer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/framework/GLFWApplication
    ${CMAKE_SOURCE_DIR}/framework/GeometricTools
    ${CMAKE_SOURCE_DIR}/framework/Rendering
    ${CMAKE_SOURCE_DIR}/framework/Projection
)

# Link Lab3Application with required dependencies
target_link_libraries(Renderer
    GLFWApplication
    GeometricTools
    Rendering
    Projection
    glad
    glfw
    stb
    glm
    GameLib
)
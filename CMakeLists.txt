cmake_minimum_required(VERSION 3.16)
project(OpenGLGraphicsModules LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OGM_DIR ${CMAKE_CURRENT_LIST_DIR})

function(ogm_copy_folder target src_dir dest_name)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${src_dir}"
                "$<TARGET_FILE_DIR:${target}>/${dest_name}"
        COMMENT "Copying ${dest_name} to build output"
    )
endfunction()

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(${OGM_DIR}/vendors/GLFW)

add_subdirectory(${OGM_DIR}/vendors/glm)

file(GLOB GLAD_SOURCES CONFIGURE_DEPENDS ${OGM_DIR}/vendors/glad/src/*.c)
add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC ${OGM_DIR}/vendors/glad/include)

file(GLOB OGM_SOURCES CONFIGURE_DEPENDS
    ${OGM_DIR}/Camera/*.cpp
    ${OGM_DIR}/Input/*.cpp
    ${OGM_DIR}/Mesh/*.cpp
    ${OGM_DIR}/Shader/*.cpp
    ${OGM_DIR}/Window/*.cpp
    # Update here when adding more folders
)

add_library(OpenGLGraphicsModules STATIC ${OGM_SOURCES})
add_library(OGM::OGM ALIAS OpenGLGraphicsModules)

target_include_directories(OpenGLGraphicsModules PUBLIC ${OGM_DIR})

if(TARGET glm::glm)
    set(OGM_GLM_TARGET glm::glm)
else()
    set(OGM_GLM_TARGET glm)
endif()

target_link_libraries(OpenGLGraphicsModules PUBLIC
    glfw
    glad
    ${OGM_GLM_TARGET}
    ${CMAKE_DL_LIBS}
)

if(MSVC)
    target_compile_options(OpenGLGraphicsModules PRIVATE /W4)
else()
    target_compile_options(OpenGLGraphicsModules PRIVATE -Wall -Wextra)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_executable(ogm_test ${OGM_DIR}/test.cpp)
    target_link_libraries(ogm_test PRIVATE OGM::OGM)

    ogm_copy_folder(ogm_test "${OGM_DIR}/test_shaders" "shaders")

    set_target_properties(ogm_test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ogm_test>")
endif()

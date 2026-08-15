cmake_minimum_required(VERSION 3.22)

include(CMakePrintHelpers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

#
# GAME
# Cambiar en cada proyecto.
#
set(GAME_NAME GameMin)
project(${GAME_NAME})

add_subdirectory(Resources)

#
# Librería que debemos generar para Android nativo.
#
if(OSK_PLATFORM STREQUAL "OSK_ANDROID")
    include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
    add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    target_link_libraries(native_app_glue log android)
    add_compile_definitions(VK_USE_PLATFORM_ANDROID_KHR)
endif()

#
# Todos los archivos de código.
#
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS "*.cpp")
file(GLOB HEADER_FILES CONFIGURE_DEPENDS "*.h*")

add_compile_definitions(OSK_DLL_IMPORT)


if (OSK_PLATFORM STREQUAL "OSK_ANDROID")
    add_library(${GAME_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES} "MinComputePass.h" "MinComputePass.cpp" "Render2dPass.h" "Render2dPass.cpp"  engine_config.json)
else()
    add_executable(${GAME_NAME} ${SOURCE_FILES} ${HEADER_FILES} "MinComputePass.h" "MinComputePass.cpp" "Render2dPass.h" "Render2dPass.cpp"    engine_config.json)
    target_sources(${GAME_NAME} PRIVATE ${SOURCE_FILES})
endif()


#
# #define de C++.
#
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_compile_definitions(${OSK_PLATFORM})
add_compile_definitions(${EXTRA_DEFINES})
add_compile_definitions(${OSK_EXTRA})
if(OSK_DEBUG)
    add_compile_definitions(OSK_DEBUG)
endif()


#
# Búsqueda de dependencias.
#
if(OSK_PLATFORM STREQUAL "OSK_WINDOWS")
    find_package(glfw3          CONFIG REQUIRED)
    find_package(Vulkan REQUIRED)
    target_include_directories(${GAME_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
endif()
find_package(glm            CONFIG REQUIRED)
find_package(Freetype       CONFIG REQUIRED)
find_package(OpenAL         CONFIG REQUIRED)
find_package(unofficial-spirv-reflect  CONFIG REQUIRED)

find_package(OSKengine      CONFIG REQUIRED)
get_target_property(AVAILABLE_CONFIGS ${GAME_NAME} IMPORTED_CONFIGURATIONS)

# Dependencias transitivas.
# Se ponen explícitamente para pdoer copiar sus .dll.
find_package(unofficial-brotli CONFIG REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)


#
# Dependencias.
#.
if(OSK_PLATFORM STREQUAL "OSK_ANDROID")
    set(ALL_DEPENDENCIES OSKengine::OSKengine unofficial::brotli::brotlidec BZip2::BZip2 ZLIB::ZLIB PNG::PNG vulkan android glm::glm-header-only unofficial::spirv-reflect OpenAL::OpenAL Freetype::Freetype) #spirv-cross-c
else()
    if(OSK_PLATFORM STREQUAL "OSK_WINDOWS")
        set(ALL_DEPENDENCIES OSKengine::OSKengine unofficial::brotli::brotlidec BZip2::BZip2 ZLIB::ZLIB PNG::PNG glm::glm-header-only unofficial::spirv-reflect OpenAL::OpenAL Freetype::Freetype glfw ${VULKAN_SDK}/Lib/vulkan-1.lib) #spirv-cross-c
    else()
        set(ALL_DEPENDENCIES OSKengine::OSKengine unofficial::brotli::brotlidec BZip2::BZip2 ZLIB::ZLIB PNG::PNG glm::glm-header-only unofficial::spirv-reflect OpenAL::OpenAL Freetype::Freetype glfw ${VULKAN_SDK}/x86_64/lib/libvulkan.so.1) #spirv-cross-c
    endif()
endif()

cmake_print_variables(ALL_DEPENDENCIES)
if(OSK_PLATFORM STREQUAL "OSK_ANDROID")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate -v")
    target_link_libraries(${GAME_NAME} PUBLIC ${ALL_DEPENDENCIES} native_app_glue)
else()
    target_link_libraries(${GAME_NAME} PUBLIC ${ALL_DEPENDENCIES})
endif()

cmake_print_variables(TARGET_RUNTIME_DLLS)
if(WIN32)
    add_custom_command(TARGET ${GAME_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${GAME_NAME}>
            $<TARGET_FILE_DIR:${GAME_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()

file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/engine_config.json" "${CMAKE_CURRENT_BINARY_DIR}/engine_config.json" SYMBOLIC)
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Assets" "${CMAKE_CURRENT_BINARY_DIR}/Resources/Assets" SYMBOLIC)
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons" "${CMAKE_CURRENT_BINARY_DIR}/Resources/Icons" SYMBOLIC)
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Materials" "${CMAKE_CURRENT_BINARY_DIR}/Resources/Materials" SYMBOLIC)
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Scenes" "${CMAKE_CURRENT_BINARY_DIR}/Resources/Scenes" SYMBOLIC)
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders" "${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders" SYMBOLIC)


# Instalación del ejecutable
install(TARGETS ${GAME_NAME}                      DESTINATION ".")
install(FILES $<TARGET_RUNTIME_DLLS:${GAME_NAME}> DESTINATION ".")

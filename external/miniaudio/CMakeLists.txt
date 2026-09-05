set(CMAKE_C_COMPILER /usr/bin/clang)

add_library(miniaudio STATIC miniaudio.c)
target_link_libraries(miniaudio PRIVATE
    "-framework CoreAudio"
    "-framework AudioToolbox"
    "-framework CoreFoundation"
)
target_include_directories(miniaudio PUBLIC .)
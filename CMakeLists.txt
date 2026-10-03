cmake_minimum_required(VERSION 3.20)

project(CharacterCounter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(CharacterCounter
    main.cpp
    application/ConsoleApplication.cpp
    factory/CountingStrategyFactory.cpp
    model/CharacterData.cpp
    service/FileService.cpp
    strategy/UnorderedMapCountingStrategy.cpp
    utils/Utf8Converter.cpp
)

target_compile_features(CharacterCounter PRIVATE cxx_std_20)

if (MSVC)
    target_compile_options(CharacterCounter PRIVATE /utf-8)
endif()

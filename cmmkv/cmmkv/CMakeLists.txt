# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("cmmkv")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        src/main/cpp/MMKVLog.h
        src/main/cpp/native-lib.cpp
        src/main/cpp/cpplang.hpp
        src/main/cpp/ThreadLock.h
        src/main/cpp/ThreadLock.cpp
        src/main/cpp/ScopedLock.hpp
        src/main/cpp/InterProcessLock.cpp
        src/main/cpp/InterProcessLock.h
        src/main/cpp/PBUtility.h
        src/main/cpp/PBUtility.cpp
        src/main/cpp/CodedInputData.h
        src/main/cpp/CodedInputData.cpp
        src/main/cpp/CodedOutputData.h
        src/main/cpp/CodedOutputData.cpp
        src/main/cpp/MMBuffer.h
        src/main/cpp/MMBuffer.cpp
        src/main/cpp/MmapedFile.h
        src/main/cpp/MmapedFile.cpp
        src/main/cpp/CMMKV.h
        src/main/cpp/CMMKV.cpp
        src/main/cpp/MiniPBCoder.h
        src/main/cpp/MiniPBCoder.cpp
        src/main/cpp/PBEncodeItem.hpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})
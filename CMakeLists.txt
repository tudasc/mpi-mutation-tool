cmake_minimum_required(VERSION 3.0)

project(mpi_mutation_tool)

find_package(Clang REQUIRED)
message(STATUS "Found LLVM/Clang version " ${LLVM_VERSION})

add_executable(MPIMutationTool src/MPIMutationTool.cpp src/Mutations.cpp)

target_link_libraries(MPIMutationTool clang clang-cpp clangTooling)
target_include_directories(MPIMutationTool PUBLIC ${CLANG_INCLUDE_DIRS})
target_include_directories(MPIMutationTool PUBLIC src)

#LLVM is typically build with no RTTI, we need to match that
target_compile_options(MPIMutationTool PUBLIC -fno-rtti -Wl,--copy-dt-needed-entries)

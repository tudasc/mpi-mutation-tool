cmake_minimum_required(VERSION 3.0)

project(mpi_mutation_tool)

find_package(Clang REQUIRED)
message(STATUS "Found LLVM/Clang version " ${LLVM_VERSION})

add_executable(MPIMutationTool MPIMutationTool.cpp Mutations.cpp)

target_link_libraries(MPIMutationTool clang clang-cpp clangTooling)
target_include_directories(MPIMutationTool PUBLIC ${CLANG_INCLUDE_DIRS})

#wir wollen das die ausführbare datei keine run time type information generiert
#das ist notwendig weil die bibliothek (clangTooling) auch keine rtti hat, 
#und wir die gleiche configuration haben müssen
target_compile_options(MPIMutationTool PUBLIC -g -fno-rtti -Wl,--copy-dt-needed-entries)

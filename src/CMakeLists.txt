add_executable(main main.cpp)
target_link_options(main PRIVATE -Wall)
target_compile_options(main PRIVATE -Wall)
# target_link_options(main PRIVATE -Wall -fsanitize=address -fno-omit-frame-pointer)
# target_compile_options(main PRIVATE -Wall -fsanitize=address -fno-omit-frame-pointer)

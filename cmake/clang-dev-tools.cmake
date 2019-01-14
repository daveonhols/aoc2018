# Additional target to perform clang-format/clang-tidy run
# Requires clang-format and clang-tidy

# Get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp)

add_custom_target(
        clang-format
        COMMAND /usr/bin/clang-format
        -style=llvm
        -i
        ${ALL_SOURCE_FILES}
)

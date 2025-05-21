# Source file organization
set(BUILTINS_SOURCES
    builtins/cd.c
    builtins/echo.c
    builtins/exit.c
    builtins/pwd.c
    builtins/type.c
)

set(COMMAND_SOURCES
    command/parser.c
    command/executor.c
    command/redirection.c
)

set(COMPLETION_SOURCES
    completion/completion.c
    completion/matcher.c
)

set(TERMINAL_SOURCES
    terminal/raw_mode.c
    terminal/input.c
)

set(UTILS_SOURCES
    utils/memory.c
    utils/path.c
    utils/string.c
    utils/error.c
)

# Collect all sources
set(SOURCES
    ${BUILTINS_SOURCES}
    ${COMMAND_SOURCES}
    ${COMPLETION_SOURCES}
    ${TERMINAL_SOURCES}
    ${UTILS_SOURCES}
    main.c
)

# Define the shell executable target
add_executable(shell ${SOURCES})

# Link with required libraries
target_link_libraries(shell m)  # Math library

# Platform-specific configurations
if(UNIX AND NOT APPLE)
    # Linux-specific flags
    target_compile_definitions(shell PRIVATE _POSIX_C_SOURCE=200809L)
endif()

# OS-specific libraries
if(APPLE)
    find_library(CORE_FOUNDATION CoreFoundation)
    target_link_libraries(shell ${CORE_FOUNDATION})
endif()
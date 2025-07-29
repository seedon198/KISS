# Import FreeRTOS
include(FetchContent)

FetchContent_Declare(
    freertos_kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG        V10.5.1
)

FetchContent_MakeAvailable(freertos_kernel)

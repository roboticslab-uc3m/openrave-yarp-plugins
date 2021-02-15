option(ENABLE_examples "Enable/disable C++ examples" OFF)

if(ENABLE_examples)
    add_subdirectory(robotDraw)
endif()

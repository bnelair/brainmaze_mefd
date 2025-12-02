cmake_minimum_required(VERSION 3.15)

# Python module
pybind11_add_module(_brainmaze_mefd bindings.cpp)
target_link_libraries(_brainmaze_mefd PRIVATE brainmaze_mefd)

# Set output name
set_target_properties(_brainmaze_mefd PROPERTIES
    OUTPUT_NAME "_brainmaze_mefd"
)

# Install Python module
install(TARGETS _brainmaze_mefd
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages/brainmaze_mefd
)

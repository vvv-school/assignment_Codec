# requires minimum cmake version
cmake_minimum_required(VERSION 3.5)

# produce the cmake var PROJECT_NAME
project(assignment_Codec)

# mandatory use of these packages
find_package(YARP 3.0.0 REQUIRED)
find_package(ICUBcontrib REQUIRED)

# extend the current search path used by cmake to load helpers
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})

# helpers defining certain macros
include(ICUBcontribHelpers)
include(ICUBcontribOptions)

# specify to install in $ICUBcontrib_DIR/bin
icubcontrib_set_default_prefix()

# generate thrift idl file
yarp_add_idl(IDL_GEN_FILES ${CMAKE_SOURCE_DIR}/src/idl/${PROJECT_NAME}.thrift)

# building module
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}
               ${CMAKE_SOURCE_DIR}/src/Codec.cpp
               ${CMAKE_SOURCE_DIR}/src/main.cpp
               ${idl}
               ${IDL_GEN_FILES})

target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# generate ad-hoc project to perform "make uninstall"
icubcontrib_add_uninstall_target()

file(GLOB scripts ${CMAKE_SOURCE_DIR}/app/scripts/*.xml)
yarp_install(FILES ${scripts} DESTINATION ${ICUBCONTRIB_APPLICATIONS_INSTALL_DIR})
yarp_install(FILES assignment_Codec.xml DESTINATION ${ICUBCONTRIB_MODULES_INSTALL_DIR})

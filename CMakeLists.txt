cmake_minimum_required(VERSION 3.13)
project(env_vars_commander)

set(CMAKE_CXX_STANDARD 14)

add_library(env_vars_commander SHARED library.cpp library.h plugin_utils.h dialogs.h dialogs.cpp descriptions.h descriptions.cpp)
set_target_properties(env_vars_commander PROPERTIES PREFIX "" SUFFIX ".wfx")
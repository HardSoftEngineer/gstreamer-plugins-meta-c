cmake_minimum_required(VERSION 3.10)
project(gst_myplugin C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-base-1.0)

add_library(gstmymetawriter SHARED gstmywriter.c)

target_include_directories(gstmymetawriter PRIVATE ${GST_INCLUDE_DIRS})
target_link_libraries(gstmymetawriter ${GST_LIBRARIES})

target_compile_options(gstmymetawriter PRIVATE ${GST_CFLAGS_OTHER})

target_compile_definitions(gstmymetawriter PRIVATE
    PACKAGE="mymetawriter"
    PACKAGE_NAME="mymetawriter"
    VERSION="1.0"
)

set_target_properties(gstmymetawriter PROPERTIES
    PREFIX ""
)

add_library(gstmymetareader SHARED gstmyreader.c)

target_include_directories(gstmymetareader PRIVATE ${GST_INCLUDE_DIRS})
target_link_libraries(gstmymetareader ${GST_LIBRARIES})
target_compile_options(gstmymetareader PRIVATE ${GST_CFLAGS_OTHER})

target_compile_definitions(gstmymetareader PRIVATE
    PACKAGE="mymetareader"
    PACKAGE_NAME="mymetareader"
    VERSION="1.0"
)

set_target_properties(gstmymetareader PROPERTIES 
    PREFIX ""
)

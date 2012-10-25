ADD_LIBRARY(r2d2 SHARED r2d2.cpp)
ADD_LIBRARY(r2d2static STATIC r2d2.cpp)

set_target_properties(r2d2static PROPERTIES
         ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out)
set_target_properties(r2d2static PROPERTIES
         OUTPUT_NAME r2d2)

set_target_properties(r2d2 PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out)



find_library(

        CUNIT_LIBRARY
        cunit
        libcunit
        CUnit

        HINTS
        ~/homebrew/Cellar/cunit/2.1-3/lib
        /lib64
        /usr/lib64

)

get_filename_component(CUNIT_PATH ${CUNIT_LIBRARY} DIRECTORY)
include_directories("${CUNIT_PATH}/../include")

include_directories(../ginosa)

set(SOURCE_FILES queue.c)
add_executable(queue queue.c)
target_link_libraries(queue ginosa)
target_link_libraries(queue ${CUNIT_LIBRARY})
target_link_libraries(queue pthread)

set(SOURCE_FILES n1.c)
add_executable(n1 n1.c)
target_link_libraries(n1 ginosa)
target_link_libraries(n1 ${CUNIT_LIBRARY})
target_link_libraries(n1 pthread)

set(SOURCE_FILES n2.c)
add_executable(n2 n2.c)
target_link_libraries(n2 ginosa)
target_link_libraries(n2 ${CUNIT_LIBRARY})
target_link_libraries(n2 pthread)

set(SOURCE_FILES n3.c)
add_executable(n3 n3.c)
target_link_libraries(n3 ginosa)
target_link_libraries(n3 ${CUNIT_LIBRARY})
target_link_libraries(n3 pthread)

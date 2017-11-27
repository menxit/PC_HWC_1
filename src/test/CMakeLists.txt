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

set(SOURCE_FILES test.c)
add_executable(

    HWC1_TEST test.c

    all_tests/all_tests.h
    all_tests/all_tests.c

    utilities/Factory/Factory.h
    utilities/Factory/Factory.c

    utilities/ConsumatoreBloccanteDiUnMessaggio/ConsumatoreBloccanteDiUnMessaggio.h
    utilities/ConsumatoreBloccanteDiUnMessaggio/ConsumatoreBloccanteDiUnMessaggio.c

    utilities/ProduttoreBloccanteDiUnMessaggio/ProduttoreBloccanteDiUnMessaggio.h
    utilities/ProduttoreBloccanteDiUnMessaggio/ProduttoreBloccanteDiUnMessaggio.c

)

target_link_libraries(HWC1_TEST ${CUNIT_LIBRARY})
target_link_libraries(HWC1_TEST pthread)
target_link_libraries(HWC1_TEST ginosa)

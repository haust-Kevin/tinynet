set(BASE_SRCS
    Thread.cc
    ThreadPool.cc
    Timestamp.cc
)

add_library(tinynet_base ${BASE_SRCS})

target_link_libraries(tinynet_base pthread)

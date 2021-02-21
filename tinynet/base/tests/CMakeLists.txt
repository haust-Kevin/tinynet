

add_executable(thread_test Thread_test.cc)
target_link_libraries(thread_test tinynet_base)
add_test(NAME thread_test COMMAND thread_test)

add_executable(threadpool_test ThreadPool_test.cc)
target_link_libraries(threadpool_test tinynet_base)
add_test(NAME threadpool_test COMMAND threadpool_test)

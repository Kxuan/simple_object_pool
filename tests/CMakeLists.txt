add_executable(mpool_test pool_test.c)
target_link_libraries(mpool_test PUBLIC sop)
target_compile_options(mpool_test PUBLIC -Os -pg )
target_link_libraries(mpool_test PUBLIC "-pg")

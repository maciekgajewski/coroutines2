find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})


add_executable(test

	execution_context.cpp
	coroutine.cpp
)

target_link_libraries(test

	coroutines

	${GTEST_BOTH_LIBRARIES}
	pthread
)


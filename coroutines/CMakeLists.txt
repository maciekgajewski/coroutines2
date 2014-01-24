find_package( Boost 1.54.0 COMPONENTS context system REQUIRED)

add_library(coroutines

	coroutine.cpp coroutine.hpp
	execution_context.cpp execution_context.hpp
)

target_link_libraries(coroutines

	${Boost_LIBRARIES}
)

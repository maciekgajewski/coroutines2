find_package( Boost 1.55.0 COMPONENTS context system coroutine REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


add_executable(http_asio_stackful

	http_asio_stackful.cpp
	socket_streambuf.hpp
)

target_link_libraries(http_asio_stackful

	${Boost_LIBRARIES}
	PocoNet
)

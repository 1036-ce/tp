find_package(benchmark REQUIRED)

if (!benchmark_FOUNDED) 
	message(FATAL "benchmark not found")
endif()

add_executable(bench bench.cpp)

target_link_libraries(bench
	PRIVATE
		benchmark::benchmark)

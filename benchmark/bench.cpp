#include <benchmark/benchmark.h>
#include <map.hpp>
#include <map>

static void BM_tp_map(benchmark::State &state) {
	tp::map<int, int> mp{};
	int index = 0;
	for (auto _ : state)
		mp.insert({index, index});
}
BENCHMARK(BM_tp_map);

static void BM_std_map(benchmark::State &state) {
	std::map<int, int> mp{};
	int index = 0;
	for (auto _ : state)
		mp.insert({index, index});
}
BENCHMARK(BM_std_map);


BENCHMARK_MAIN();

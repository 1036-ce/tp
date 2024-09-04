# disable cpu frequency scaling
sudo cpupower frequency-set --governor performance >> /dev/null
./build/benchmark/bench

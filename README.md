# SSZ++
A fast SSZ library focused on minimal memory allocations and fast hashing. 

## Requirements
- A compiler supporting C++23, tested only with GCC 13.1 (clang will fail due to lack of C++20 concepts)
- [Hashtree](https://github.com/prysmaticlabs/hashtree) is used for hashing.
- Cmake for building.
- Optional for the spectest suite
    - [Yaml-cpp](https://github.com/jbeder/yaml-cpp)
    - [Snappy](https://github.com/google/snappy)

## Usage

1. Install all requirements. 
2. Clone the repo: `git clone https://github.com/potuz/sszpp.git`
3. Build the project. This will download the spectests tarball, will take a while.
```bash
$ cd sszpp
$ mkdir build
$ cd build
$ cmake ../
$ make
```
4. Run spectests `./spectests` 
5. Run some simple benchmarks. In the same directory `build` place a file named `state.ssz` with the ssz binary representation of a Capella Beacon State. Then run the benchmark with `./bench_beacon_state`.

## Benchmarks

These are results on a Dell XPS 9320
```
$ ./bench_beacon_state
Deserializaton: 28ms
Hashing: 377ms
```

Compared with Prysm (using ADX on BLST and GoHashtree for hashing)
```
Deserialize Duration: 196.692907ms, Hashing Duration: 483.900149ms 
```

## License
Licenced under GPLv3 except for [uint256_t](https://github.com/calccrypto/uint256_t) and [acutest](https://github.com/mity/acutest) (included) that are  licenced MIT. 

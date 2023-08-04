# SSZ++
A fast SSZ library focused on minimal memory allocations and fast hashing. 

It is focused on templated polymorphism, with no dynamic dispatching and performing the minimal number of memory copies. 

**WARNING** This project is in very early implementation status, it is surely filled with bugs and security issue, it should not be used in production code. 
## Requirements
- A compiler supporting C++23, tested only with GCC 13.1 (clang will fail due to lack of C++20 concepts)
- [Hashtree](https://github.com/prysmaticlabs/hashtree) is used for hashing.
- Cmake for building.
- Optional for the spectest suite
    - [Yaml-cpp](https://github.com/jbeder/yaml-cpp)
    - [Snappy](https://github.com/google/snappy)

## Tests

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
Deserialization: 28ms
Hashing: 62ms
Root: 0x5422c2fe46f4fa5719f91b353371428125f04ba684264c4bd2b25967258e66d9
```
Compared with Prysm using ADX on BLST and fully parallelized GoHashtree for hashing
```
Deserialize Duration: 100.248102ms, Hashing Duration: 139.618937ms HTR: 0x5422c2fe46f4fa5719f91b353371428125f04ba684264c4bd2b25967258e66d9
```
These are on a Ryzen 9 7950X
```
Deserialization: 27ms
 Hashing: 23ms
 Root: 0x5422c2fe46f4fa5719f91b353371428125f04ba684264c4bd2b25967258e66d9
```

These on a Ryzen 9 5900HX without any paralellization (courtesy of E. Del Fante)
```
Deserialization: 35ms
Hashing: 349ms
```
Compared to native Teku on the same computer deserializing at `130ms` and cold hashing at `1 500ms`. 

## Usage

You can use basic types like `bool`, `std::byte`, `std::uintX_t` with `X=8,16,32,64,128` and `256`. 

To model bitvectors use `std::bitset`, to model arbitrary vectors use `std::array` or `std::vector`. The library provides a wrapper `ssz::list<T, N>` to model the SSZ type `List[T,N]`. That list uses internally a `std::vector<T>` as container. 

To define a custom container unfortunately until `C++26` with static introspection we need to use a macro. You can define them as follows
```c++
struct indexed_attestation_t : ssz::ssz_variable_size_container {
    ssz::list<std::uint64_t, MAX_VALIDATORS_PER_COMMITEE> attesting_indices;
    attestation_data_t data;
    signature_t signature;

    constexpr auto operator<=>(const indexed_attestation_t& rhs) const noexcept = default;
    constexpr bool operator==(const indexed_attestation_t& rhs) const noexcept = default;

    SSZ_CONT(attesting_indices, data, signature);
};
```
You only need to inherit from `ssz::ssz_container` or `ssz::ssz_variable_size_container` depending on whether this is a container where all members are fixed sizes or not. These bases classes are empty classes just there for the type traits until reflection comes to C++. 

To deserialize an object of type `T` you would use
```c++
std::vector<std::byte> vec{bytes};
auto object = ssz::deserialize<T>(vec);
```

To deserialize in place you can use
```c++
T object{};
ssz::deserialize(vec, object);
```

To deserialize an object allocated in the heap you can use
```c++
std::unique_ptr object_ptr = ssz::deserialize<T*>(vec);
```

To serialize your object is simply
```c++
std::vector<std::byte> vec = ssz::serialize(object);
```

To get the hash tree root you would try
```c++
std::array<std::byte, 32> htr = ssz::hash_tree_root(object, cpu_count);
```
where `cpu_count` is the number of threads that you want to use. Using `0` (the default) will use all available cores. 

The library comes with all the consensus layer structures used in the `Capella`  fork, you can copy those as templates, or simply wrap your structures around them.

## License
Licenced under the [Apache License version 2.0](https://apache.org/licenses/LICENSE-2.0.txt) except for [uint256_t](https://github.com/calccrypto/uint256_t) and [acutest](https://github.com/mity/acutest) (included) that are  licenced MIT. 

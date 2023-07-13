Licenced under XXXX except for [uint256_t](https://github.com/calccrypto/uint256_t) and [acutest](https://github.com/mity/acutest) (included) that are  licenced MIT. 

- You need a pretty good new compiler as in GCC 13.1
- Run spectests
```
mkdir build
cd build
cmake ../
make && ./spectests
```
- Grab a full beacon state and name it `state.ssz` in the `build` directory. 
```
./bench_beacon_state
Deserializaton: 0.0315011s
Hashing: 0.384708s
```

This is to compare to Prysm hashing the state at 800ms in the same laptop. 

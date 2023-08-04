/*  bench_beacon_state.cpp
 *
 *  This file is part of ssz++.
 *  ssz++ is a C++ library implementing simple serialize
 *  https://github.com/ethereum/consensus-specs/blob/dev/ssz/simple-serialize.md
 *
 *  Copyright (c) 2023 - Offchain Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *  http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <filesystem>
#include <fstream>
#include <memory>
#include <iostream>
#include <chrono>

#include "bytelists.hpp"
#include "container.hpp"
#include "ssz++.hpp"
#include "beacon_state.hpp"

constexpr auto file_path = "state.ssz";

int main() {
    std::ifstream ssz_contents(file_path, std::ios::in | std::ios::binary);
    if (not ssz_contents.is_open())
        throw std::filesystem::filesystem_error("could not open file state.ssz", std::error_code());

    const auto ssz_contents_size = std::filesystem::file_size(file_path);
    std::vector<std::byte> ssz_bytes(ssz_contents_size);
    ssz_contents.read(reinterpret_cast<char *>(ssz_bytes.data()), ssz_contents_size);
    ssz_contents.close();

    const auto start_deserialize = std::chrono::high_resolution_clock::now();
    std::unique_ptr<ssz::beacon_state_t> state{ssz::deserialize<ssz::beacon_state_t*>(ssz_bytes)};
    const auto end_deserialize = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> elapsed_deserialize = end_deserialize - start_deserialize;

    const auto start_hashing = std::chrono::high_resolution_clock::now();
    auto htr = hash_tree_root(*state);
    const auto end_hashing = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> elapsed_hashing = end_hashing - start_hashing;

    std::cout << "Deserialization: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_deserialize)
              << "\nHashing: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_hashing)
              << "\nRoot: " << ssz::to_string(htr) << std::endl;

    return 0;
}

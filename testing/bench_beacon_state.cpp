/*  bench_beacon_state.cpp
 *
 *  This file is part of ssz++.
 *  ssz++ is a C++ library implementing simple serialize
 *  https://github.com/ethereum/consensus-specs/blob/dev/ssz/simple-serialize.md
 *
 *  Copyright (c) 2023 - Potuz potuz@potuz.net
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <filesystem>
#include <fstream>
#include <memory>
#include <iostream>
#include <chrono>

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

    std::cout << "Deserializaton: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_deserialize)
              << "\nHashing: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_hashing) << std::endl;

    return 0;
}

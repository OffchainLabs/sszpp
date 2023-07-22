/*  math.hpp
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
#pragma once
#include <bit>
#include <climits>
#include <concepts>

namespace helpers {
/**
 * returns the logarithm of the smallest power of 2 not smaller than the given input
 *
 * (0,0), (1,0), (2,1), (3,2), (4,2), (5,3),...
 */
constexpr auto log2ceil(std::unsigned_integral auto n) {
    return n ? CHAR_BIT * sizeof(n) - std::countl_zero(n - 1) : 0;
}
}  // namespace helpers

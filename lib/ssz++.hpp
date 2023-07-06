/*  ssz++.hpp
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

#pragma once

#include <stdexcept>
#include "merkleize.hpp"
#include "bitlists.hpp"
#include "attestation.hpp"

namespace ssz {
template <ssz_object T>
    requires(!std::is_pointer_v<T>)
T deserialize(const serialized_range auto &bytes) {
    T ret{};
    deserialize(bytes, ret);
    return ret;
}

template <typename T>
    requires(std::is_pointer_v<T> && ssz_object<std::remove_pointer_t<T>>)
T deserialize(const serialized_range auto &bytes) {
    auto ret = new (std::remove_pointer_t<T>);
    deserialize(bytes, *ret);
    return ret;
}
} // namespace ssz

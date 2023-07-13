/*  fork.hpp
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
namespace ssz {
struct fork_t : ssz_container {
    using version_t = std::array<std::byte, 4>;
    version_t  previous_version, current_version;
    Epoch epoch;

    constexpr auto operator<=>(const fork_t& rhs) const noexcept = default;
    constexpr bool operator==(const fork_t& rhs) const noexcept = default;

    SSZ_CONT(previous_version, current_version, epoch);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, version_t&>("previous_version", previous_version),
              std::pair<const char*, version_t&>("current_version", current_version),
              std::pair<const char*, Epoch&>("epoch", epoch));
#endif
};
}

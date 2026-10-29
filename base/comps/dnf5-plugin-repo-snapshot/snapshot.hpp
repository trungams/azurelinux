// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include <string>

namespace snapshot {

std::uint64_t parse_time(const std::string & value);
std::set<std::string> parse_exemptions(const std::string & value);

bool filter_package(
    std::optional<unsigned long long> file_time,
    std::optional<std::uint64_t> cutoff,
    bool installed,
    bool exempt,
    const std::string & repo_id,
    const std::string & nevra);

}

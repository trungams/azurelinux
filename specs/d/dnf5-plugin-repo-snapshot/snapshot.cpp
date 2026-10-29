// SPDX-License-Identifier: MIT
#include "snapshot.hpp"

#include <charconv>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace snapshot {

std::uint64_t parse_time(const std::string & value) {
    std::uint64_t result = 0;
    const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), result);
    if (error != std::errc{} || end != value.data() + value.size() ||
        result > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
        throw std::invalid_argument("snapshot time must be a nonnegative decimal POSIX second in signed 64-bit range");
    }
    return result;
}

std::set<std::string> parse_exemptions(const std::string & value) {
    std::set<std::string> ids;
    if (value.empty()) {
        throw std::invalid_argument("snapshot: --snapshot-exclude-repos requires repository IDs");
    }
    std::istringstream input(value);
    std::string id;
    while (std::getline(input, id, ',')) {
        if (id.empty() || id.find_first_of(" \t\r\n") != std::string::npos || !ids.insert(id).second) {
            throw std::invalid_argument("snapshot: empty, duplicate or invalid repository exemption: " + id);
        }
    }
    if (value.back() == ',') {
        throw std::invalid_argument("snapshot: empty trailing repository exemption");
    }
    return ids;
}

bool filter_package(
    std::optional<unsigned long long> file_time,
    std::optional<std::uint64_t> cutoff,
    bool installed,
    bool exempt,
    const std::string & repo_id,
    const std::string & nevra) {
    if (!cutoff || installed || exempt) {
        return false;
    }
    if (!file_time || *file_time > static_cast<unsigned long long>(std::numeric_limits<std::int64_t>::max())) {
        throw std::runtime_error("snapshot: missing or invalid file time in " + repo_id + ": " + nevra);
    }
    return *file_time > *cutoff;
}

}

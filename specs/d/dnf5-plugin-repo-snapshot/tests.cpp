// SPDX-License-Identifier: MIT
#include "snapshot.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <functional>
#include <stdexcept>

static void rejects(const std::function<void()> & action) {
    bool rejected = false;
    try {
        action();
    } catch (const std::exception &) {
        rejected = true;
    }
    assert(rejected);
}

int main() {
    assert(snapshot::parse_time("1761696000") == 1761696000);
    for (const auto & bad : {"", "-1", "1761696000x", "9223372036854775808"}) {
        rejects([&] { (void)snapshot::parse_time(bad); });
    }
    assert(snapshot::parse_exemptions("one,two") == (std::set<std::string>{"one", "two"}));
    for (const auto & bad : {"", "one,", "one,one", "one, two"}) {
        rejects([&] { (void)snapshot::parse_exemptions(bad); });
    }
    const auto filter = [](std::optional<unsigned long long> time, std::optional<std::uint64_t> cutoff,
                           bool installed = false, bool exempt = false) {
        return snapshot::filter_package(time, cutoff, installed, exempt, "fixture", "alpha-1-1.x86_64");
    };
    assert(!filter(1761695999, 1761696000));
    assert(!filter(1761696000, 1761696000));
    assert(filter(1761696001, 1761696000));
    assert(!filter(std::nullopt, std::nullopt));
    assert(!filter(std::nullopt, 1761696000, true));
    assert(!filter(std::nullopt, 1761696000, false, true));
    rejects([&] { (void)filter(std::nullopt, 1761696000); });
}

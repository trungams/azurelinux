// SPDX-License-Identifier: MIT
#include "snapshot.hpp"

#include <libdnf5/base/base.hpp>
#include <libdnf5/conf/vars.hpp>
#include <libdnf5/plugin/iplugin.hpp>
#include <libdnf5/repo/repo_query.hpp>
#include <libdnf5/rpm/package_query.hpp>
#include <libdnf5/rpm/package_sack.hpp>
#include <libdnf5/rpm/package_set.hpp>

#include <cstring>
#include <filesystem>
#include <iostream>
#include <set>
#include <stdexcept>

namespace {

class SnapshotHook final : public libdnf5::plugin::IPlugin {
public:
    SnapshotHook(libdnf5::plugin::IPluginData & data, libdnf5::ConfigParser &) : IPlugin(data) {}
    libdnf5::PluginAPIVersion get_api_version() const noexcept override { return libdnf5::PLUGIN_API_VERSION; }
    const char * get_name() const noexcept override { return "repo_snapshot"; }
    libdnf5::plugin::Version get_version() const noexcept override { return {2, 0, 0}; }
    const char * const * get_attributes() const noexcept override {
        static const char * const attributes[]{"description", nullptr};
        return attributes;
    }
    const char * get_attribute(const char * name) const noexcept override {
        return std::strcmp(name, "description") == 0 ? "Repository publication-time cutoff" : nullptr;
    }

    void repos_loaded() override {
        auto & base = get_base();
        const auto vars = base.get_vars();
        const bool active = vars->contains("azl_snapshot_time");
        const bool has_exemptions = vars->contains("azl_snapshot_exempt_repos");
        if ((active && vars->get("azl_snapshot_time").priority != libdnf5::Vars::Priority::RUNTIME) ||
            (has_exemptions && vars->get("azl_snapshot_exempt_repos").priority != libdnf5::Vars::Priority::RUNTIME)) {
            throw std::runtime_error(
                "snapshot: variables must be set by the DNF5 CLI, not an environment or varsdir fallback");
        }
        if (!active) {
            if (has_exemptions) {
                throw std::runtime_error("snapshot: repository exemptions require --snapshot-time");
            }
            return;
        }
        if (!vars->contains("azl_snapshot_requires_available")) {
            if (has_exemptions) {
                throw std::runtime_error("snapshot: repository exemptions require available repository loading");
            }
            return;
        }
        const auto cutoff = snapshot::parse_time(vars->get_value("azl_snapshot_time"));
        const auto exempt = has_exemptions ? snapshot::parse_exemptions(vars->get_value("azl_snapshot_exempt_repos"))
                                           : std::set<std::string>{};
        libdnf5::repo::RepoQuery repos(base);
        std::set<std::string> enabled;
        for (auto repo : repos) {
            if (repo->get_type() == libdnf5::repo::Repo::Type::AVAILABLE && repo->is_enabled()) {
                if (!std::filesystem::is_regular_file(repo->get_metadata_path("primary"))) {
                    throw std::runtime_error(
                        "snapshot: required repository primary metadata unavailable: " + repo->get_id());
                }
                enabled.insert(repo->get_id());
            }
        }
        if (enabled.empty()) {
            throw std::runtime_error("snapshot: no enabled repository available for constrained candidates");
        }
        for (const auto & id : exempt) {
            if (!enabled.contains(id)) {
                throw std::runtime_error("snapshot: exempt repository not enabled or not found: " + id);
            }
            std::cerr << "snapshot: bypassing publication cutoff for entire repository " << id << '\n';
        }
        // Check excluded packages too: missing publication data must not be hidden by existing excludes.
        libdnf5::rpm::PackageQuery packages(base, libdnf5::sack::ExcludeFlags::IGNORE_REGULAR_EXCLUDES);
        packages.filter_available();
        libdnf5::rpm::PackageSet excluded(base);
        for (const auto & package : packages) {
            const auto id = package.get_repo_id();
            if (!enabled.contains(id) || exempt.contains(id) || package.is_installed()) {
                continue;
            }
            if (snapshot::filter_package(package.get_file_time(), cutoff, false, false, id, package.get_nevra())) {
                excluded.add(package);
            }
        }
        base.get_rpm_package_sack()->add_user_excludes(excluded);
    }

    void pre_add_cmdline_packages(const std::vector<std::string> & paths) override {
        if (get_base().get_vars()->contains("azl_snapshot_time")) {
            for (const auto & spec : paths) {
                if (spec.ends_with(".rpm") || spec.find("://") != std::string::npos) {
                    throw std::runtime_error("snapshot: local and URL RPM inputs are unsupported with --snapshot-time");
                }
            }
        }
    }
};

}  // namespace

extern "C" libdnf5::PluginAPIVersion libdnf_plugin_get_api_version() { return libdnf5::PLUGIN_API_VERSION; }
extern "C" const char * libdnf_plugin_get_name() { return "repo_snapshot"; }
extern "C" libdnf5::plugin::Version libdnf_plugin_get_version() { return {2, 0, 0}; }
extern "C" libdnf5::plugin::IPlugin * libdnf_plugin_new_instance(
    libdnf5::LibraryVersion, libdnf5::plugin::IPluginData & data, libdnf5::ConfigParser & parser) {
    return new SnapshotHook(data, parser);
}
extern "C" void libdnf_plugin_delete_instance(libdnf5::plugin::IPlugin * instance) { delete instance; }

# Target distro bootstrap fixture

`cases.toml` exercises the immutable target distro/version evaluation input
defined by [Resolution](../../resolution.md). The target input alone selects
the distro-version default component provider before `RM-EFFECTIVE`.
`spec.upstream-distro` and `project.default-distro` are source references and
cannot select or reselect that provider.

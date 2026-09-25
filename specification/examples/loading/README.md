# Loading-only fixture

This fixture exercises loading behavior using only document-control keys. It is
metasyntactic and is not a complete conforming object-model document.

Start at `azldev.toml`. The expected depth-first document order is:

1. `azldev.toml`
2. `configuration/base.toml`
3. `configuration/nested/defaults.toml`
4. `configuration/components/alpha.toml`
5. `configuration/components/zeta.toml`

`configuration/optional/*.toml` has an existing, readable literal directory
prefix but intentionally has no matching TOML entries, so its complete
contained enumeration contributes no document. Every TOML file contains only
document-control keys or comments.

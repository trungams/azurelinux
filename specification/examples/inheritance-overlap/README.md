# Inheritance and component-group fixture

`cases.toml` exercises the fixed low-to-high inheritance order in
[Resolution](../../resolution.md#component-inheritance-order): distro,
project, component-group, then direct component configuration.

It also covers the component-group aggregate. Append-composed arrays are
concatenated in group-name UTF-8 order without choosing a winning group;
disjoint map/table leaves compose; and repeated non-append leaves are
destructive overlap errors even when their values agree. Cases contain nested
partial `ComponentConfig` tables rather than pre-flattened leaves. Validation
uses an independently checked field-composition map and derives leaf paths,
append order, destructive overlap, and supplier provenance.

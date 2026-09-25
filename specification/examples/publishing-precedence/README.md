# Publishing precedence and identity fixture

`cases.toml` exercises the total publishing order and exact resolver inputs in
[Packages and publishing](../../packages.md#package-inheritance-and-groups).
Every applicable layer uses a distinct value so an ordering error changes the
expected channel. The cases cover ordinary, debuginfo, debugsource, and source
classes; their lookup identities; and missing or ambiguous ownership. Final
debugsource routing is asserted both with its exact-package layer and with that
layer absent.

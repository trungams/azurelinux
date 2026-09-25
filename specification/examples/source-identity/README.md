# Source-identity fixture

`cases.toml` is a data fixture for the exact upstream-commit and selector
precedence rules in [Source identity and acquisition](../../sources.md).

The `valid` cases must resolve to `expected-commit`. The `invalid` values must
be rejected before acquisition. The fixture models ordinary later-scalar
composition; it does not assign a special file role to generated pins.

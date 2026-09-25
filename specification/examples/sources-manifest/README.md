# Upstream `sources` manifest fixture

`cases.toml` exercises the exact byte, line, record, digest, filename,
duplicate, missing, and empty behavior in
[Source identity and acquisition](../../sources.md#upstream-sources-manifest).

Every valid case maps to the listed ordered artifact records. Every invalid
case must fail the complete manifest before acquisition.

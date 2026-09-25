# Conformance manifest fixture

`manifest.toml` demonstrates fixture format version `1` from
[Conformance methodology](../../conformance.md). It includes positive,
negative, resolved-leaf, and output expectations with closed
class-discriminated inputs while explicitly marking the current classes and
blocked materialized case non-claimable. `suite-registry.toml` digest-binds
that package and completely enumerates its required case IDs by class, role,
and profile without enabling any class. Every class transport and
environmental context is parsed by version and `document-kind`; the network
transcript executes a cross-origin redirect, credential-scope removal,
ordered repeated response headers, TLS fields, and exact response bytes.

The fixture contains no credentials. `credential-reference` values are opaque
non-secret identifiers only.

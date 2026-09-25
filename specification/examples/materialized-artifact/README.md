# Materialized artifact fixture

`cases.toml` and the readable `.manifest` files exercise the namespace,
basename-safe active-spec stem validation, deterministic upstream active-spec
selection and rename with differing exact `E`/`U` spellings, configured source
filename representability and generated-record reparsing, portable
symlink-target grammar, placement, entry-type, mode, exclusion,
behavioral tree identity, derived `sources` aggregate provenance, and atomic
publication rules in
[Materialized dist-git artifacts](../../artifacts.md).

The readable manifests are fixture notation, not files emitted into the
materialized tree:

```text
D <path> <mode>
F <path> <mode> <hex bytes>
L <path> - <UTF-8 target>
```

The notation enumerates the exact path set, entry types, modes, regular-file
bytes, and symbolic-link targets without defining a canonical serialization or
comparison algorithm.

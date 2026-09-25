# Determinism fixture

`cases.toml` is a focused falsifier for
[locale, timezone, and wall-clock independence](../../determinism.md#locale-timezone-and-wall-clock).
Each run supplies deliberately conflicting ambient values. The normative
projection uses only explicit UTF-8 byte ordering and the declared offset
date-time, so every run has the same expected result and ignores
`ambient-wall-clock`.

# Custom generator profile fixture

`cases.toml` exercises the selected-operation contract in
[Custom-source generation](../../sources.md#custom-source-generation-profile)
and [custom generator isolation](../../security.md#custom-generator-isolation).
It covers declared input filenames, declared mock packages,
implementation-specific isolation availability, default-deny network access,
semantic output validation, and configured output-hash verification. It does
not define execution-root bytes, package payload closure, kernel-observation
interfaces, or a reproducible sandbox proof.

The root contains exactly one `cases` array with the seven retained closed
records in `cases.toml`. Every record has the same exact key set. `id` uses the
lowercase fixture-token grammar. `architecture` is a non-empty ASCII token
matching `[A-Za-z0-9][A-Za-z0-9._+-]*`. `evaluation-instant` is a quoted
RFC 3339 UTC string using `Z`. Package arrays contain unique non-empty strings;
input arrays contain unique strings satisfying the configured source filename
grammar. `isolation-supported`, `attempted-network`, and
`semantic-output-valid` are TOML booleans.

`network` is exactly `deny`; `script-read` is empty or a configured source
filename; `output-kind` is exactly `archive-tree`; and `output-entry-count` is
a non-negative TOML integer, never a boolean. `output-filename` satisfies the
configured source filename grammar and uses a supported custom archive suffix.
`output-hash-type` is exactly `SHA256` or `SHA512`, and both digest strings are
lowercase hexadecimal text of the matching length. `expected` is exactly one
of `accept`, `integrity`, `security-policy`, `unsupported-operation`, or
`validation-error`.

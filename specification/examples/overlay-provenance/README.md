# Overlay secondary-document provenance fixture

The root source document reaches `config/components/example.toml` through the
include graph. That component declares `../../overlays/operations.toml` as an
overlay file.

The overlay file is a contained secondary semantic document with identity
`overlays/operations.toml` and defining path base `overlays`. Its operation's
`source = "payload.txt"` therefore resolves to the project-relative path
`overlays/payload.txt`. The overlay file is not an include reach and cannot
declare its own `includes` or `spec-version`.

`project/overlays/invalid-operations.toml` is the negative oracle: its
`includes` key makes it invalid as a secondary semantic document.

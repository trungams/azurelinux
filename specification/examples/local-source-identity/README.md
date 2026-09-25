# Local source identity fixture

`cases.toml` represents one local source tree without relying on host-specific
filesystem metadata. The expected identity is computed with the version-1
binary manifest grammar in
[Source identity and acquisition](../../sources.md#local-component-source).

The fixture covers an empty regular file, an executable regular file, a
symlink target, a spec file, and non-ASCII UTF-8 path bytes. Two expected
identity cases also show that adding empty directories does not change the
version-1 identity because directories are traversal nodes without records.

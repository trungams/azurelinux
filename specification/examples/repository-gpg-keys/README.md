# Repository GPG-key bindings

`repository-input.manifest.toml` binds remote and local repository GPG-key
locators to immutable bytes before repository use. `cases.toml` digest-binds
that selected-operation manifest. Remote cases name request/outcome scenarios in
the shared [HTTPS fixture](../lookaside-outcomes/README.md); local cases retain
defining-document and portable-path provenance while hashing the snapshotted
bytes. Positive cases stop at immutable binding and do not claim that fixture
bytes are accepted OpenPGP material. The selected package-manager
implementation owns key-format and signature acceptance. Diagnostic cases
execute the exact class, requirement identity, and phase for missing,
duplicate, unselected, locator-mismatched, rejected-format, and
digest-mismatched key inputs before repository access.

The owning contract is
[Repository GPG key bindings](../../resources.md#repository-gpg-key-bindings).

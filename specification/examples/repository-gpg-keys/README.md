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
duplicate, unselected, locator-mismatched, selected-implementation rejection,
and
digest-mismatched key inputs before repository access.

The owning contract is
[Repository GPG key bindings](../../resources.md#repository-gpg-key-bindings).

`repository-input.manifest.toml` has exactly `format-version`, `manifest-id`,
and `key-bindings`. Each binding has the common keys `repository`, `source`,
`locator`, and `sha256`. `source = "remote"` adds exactly `canonical-uri`;
`source = "local"` adds exactly `defining-document` and `portable-path`.

`cases.toml` is also closed. Its positive records have the common keys `name`,
`operation`, `repository`, `gpg-key`, `source`, `sha256`, `expected`, and
`repository-accessed`. Remote records add `canonical-uri` and
`shared-fetch-case`; successful records also add `key-acceptance-owner`.
Local successful records instead add `defining-document`, `portable-path`,
the two snapshot booleans, and `key-acceptance-owner`. Diagnostic records use
one exact field set selected by `defect`. Unknown, missing, wrong-type,
cross-source, cross-outcome, and cross-defect fields are errors.

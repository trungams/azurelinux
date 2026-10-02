[Previous: Source material](./source-material.md) ·
[Overview](./index.md) ·
[Next: Materialized dist-git](./materialized-dist-git.md)

# Transformations

This page explains the overlay pipeline within one private materialization
attempt. [Overlay transformations](./overlays.md) and the
[operation matrix](./overlay-operation-matrix.tsv) contain the binding rules.

After the attempt assembles a complete candidate from acquired source
material, overlays run once in private staging. They can edit the active RPM
spec, manage patches, change loose files, or transform files inside an archive.
The processor validates all declared inputs before editing, applies archive
work first, and publishes nothing unless the whole component attempt succeeds.

## Declare an ordered overlay sequence

This example sets a spec tag and adds a project-side file to the materialized
tree. In the selected package preamble, `spec-set-tag` adds the tag when no
match exists, updates its one match, and fails on multiple matches.

```toml
[[components.hello.overlays]]
type = "spec-set-tag"
tag = "Vendor"
value = "Azure Linux"

[[components.hello.overlays]]
type = "file-add"
file = "azurelinux.conf"
source = "files/azurelinux.conf"
```

`spec-set-tag` works only on the active spec. `file-add` snapshots the project
file named by `source`, then creates the materialized-tree path named by
`file`. The `source` path is relative to the document that defines the
operation. The `file` path is relative to the materialized-tree root.

The `overlays` array is append-composed. Entries retain their order within
each document, then append in document reach order and inheritance order.
When multiple groups contribute inline overlays, their arrays append in
unsigned UTF-8 group-name order while preserving each group's element order.

Unlike `overlays`, `overlay-files` is a replacing array. Later documents or
later inheritance layers replace earlier values, while contributions from
multiple groups conflict. Each entry names a project-contained TOML file whose
only content is an `overlays` array. The processor loads those files in array
order and appends their operations after all effective inline overlays. An
overlay file is a secondary semantic document, not an include fragment.

See [Overlay sequences and files](./overlays.md#overlay-sequences-and-files)
and [Common overlay field contracts](./overlays.md#common-overlay-field-contracts).

## Validate and snapshot before changing anything

Before editing, the processor validates the operations, snapshots their
project-side inputs, and creates a private staging tree. Each operation sees
the staged changes that the ordering rules below place before it. Immediately
before editing, it resolves and sorts all of its targets, then keeps that set
fixed while it runs. If any target fails, the processor discards the whole
component attempt and publishes nothing.

Every changed or created entry retains its operation and source provenance.
The remaining preflight, target-ordering, staging, and rollback rules are in
[Validation, staging, and provenance](./overlays.md#validation-staging-and-provenance).

## Apply archive groups before loose-tree operations

Archive-scoped `file-remove` and `file-search-replace` operations name an
`archive`. The processor groups them by exact archive filename. The first
occurrence of each filename sets the group order, and operations within a
group retain their declared order.

Every archive group finishes before any non-archive operation begins.
Non-archive operations then run in their original declared order, skipping
the entries assigned to archive groups. This gives each archive one extract,
modify, and repack cycle. A loose-file operation that could also touch a
grouped top-level archive is a conflict and fails before publication.
See [Ordering and conflicts](./overlays.md#ordering-and-conflicts).

For example, suppose a loose-file edit appears before operations for archive
A, archive B, and archive A again. The complete A group runs first, then B,
and then the loose-file edit. If either final archive hash fails, the complete
component attempt rolls back. The
[overlay operation fixture](./examples/overlay-operations/README.md) checks
grouping, post-overlay hashes, and all-or-nothing publication.

## Work within the bounded archive model

Archive transformations accept the defined tar filename families with
uncompressed, gzip, xz, or zstd content. The processor detects compression
from the bytes and preserves that family; the filename suffix cannot override
it.

The processor extracts each archive into a safe, deterministic tree, edits
that tree, and repacks it with the detected compression family. Unsafe paths
or links, duplicate paths, unsupported entries or metadata, malformed framing,
and trailing data fail when the archive group is interpreted, before any
archive entry is exposed to an operation. Only `file-search-replace` and
`file-remove` operate inside archives. Search/replace changes eligible
regular-file text and preserves executable classification. Removal may delete
an eligible regular file or symbolic link, never targets a directory, and
removes a link itself rather than following or retargeting it. Explicit empty
directories remain; only synthesized directories made unnecessary by removal
disappear.

Every transformed upstream archive has exactly one matching
`source-files` entry with `origin.type = "overlay"`,
`replace-upstream = true`, a replacement reason, and the required post-overlay
digest. At least one archive-scoped operation must reference it. The processor
rewrites the final `sources` record only after the digest passes. Revision
`0.1` does not choose a tar/compressor implementation or require different
tools to emit identical bytes. An implementation may use any encoder that
preserves the semantic archive result and detected compression family, but
the emitted archive must match its configured post-overlay hash.

See [Archive extraction and batching](./overlays.md#archive-extraction-and-batching)
and
[Post-overlay hash and `sources` association](./overlays.md#post-overlay-hash-and-sources-association).

## Choose the operation family

The 17 operations are grouped here for lookup:

- **Spec tags:** `spec-add-tag`, `spec-insert-tag`, `spec-set-tag`,
  `spec-update-tag`, and `spec-remove-tag`.
- **Spec lines and structure:** `spec-prepend-lines`, `spec-append-lines`,
  `spec-search-replace`, `spec-remove-section`, and
  `spec-remove-subpackage`.
- **Patches:** `patch-add` and `patch-remove`.
- **Loose files only:** `file-prepend-lines`, `file-add`, and `file-rename`.
- **Loose files or files inside an archive:** `file-search-replace` and
  `file-remove`.

The matrix marks every common field as required, optional, or forbidden for
each operation. A forbidden field is an error even when its value is empty or
false. The operation sections in
[Overlay transformations](./overlays.md#spec-tag-operations) define target
selection, effects, fixed match cardinality, and postconditions.

## Follow fixed matching and change rules

Tag operations don't have a configurable match mode. `spec-add-tag` requires
zero matches in the selected package preamble. In that preamble,
`spec-set-tag` adds on zero, updates one, and fails on multiple matches.
`spec-update-tag` requires exactly one. `spec-insert-tag` follows its family
insertion rule, and `spec-remove-tag` removes every selected match. Required
tag values cannot be empty or whitespace-only.

Spec text is normalized from LF or CRLF to LF before matching. After every
spec operation and after `patch-add` or `patch-remove`, the processor reparses
the spec. The next spec operation sees only that reparsed state.
Search/replace uses RE2 and treats replacement text literally. An operation
that requires a change fails when the requested edit changes nothing.

`patch-add` updates the spec reference and creates the patch file as one atomic
operation. `patch-remove` requires agreement between selected spec references
and loose patch files.

File operations never select a `.spec` path through the file namespace.
`file-add` and `file-rename` also reject `.spec` destinations. Pattern
matching never traverses a symbolic link as a directory, and all matched paths
use sorted normalized order.

## Publish only a complete valid result

After the last operation, the same materialization attempt checks all
postconditions, transformed-archive digests, the final `sources` record, tree
invariants, and provenance. Any failure leaves the previously published
artifact unchanged. On success, the transformed candidate remains in private
staging for the materialized-artifact publication boundary.

## Further reading

- [Overlay metadata](./overlays.md#overlay-metadata)
- [Overlay provenance fixture](./examples/overlay-provenance/README.md)

After overlays and their final checks succeed inside the attempt, the next
page shows the resulting tree and its single publication boundary.
[Continue: Materialized dist-git](./materialized-dist-git.md)

[Previous: Source material](./source-material.md) ·
[Overview](./index.md) ·
[Next: Materialized dist-git](./materialized-dist-git.md)

# Transformations

> **Non-normative reading guide.** This page explains the normal overlay
> workflow. The primary normative owners are
> [Overlay transformations](./overlays.md) and the
> [operation matrix](./overlay-operation-matrix.tsv); cross-cutting owners are
> linked where used.

Overlays make declared downstream changes after source identity and artifact
acquisition have produced a complete private candidate tree. They can edit the
active RPM spec, add or remove patches, change loose files, or transform files
inside a source archive. The complete component attempt is atomic.

**First-pass takeaways:**

- validate paths, patterns, and project-side sources before editing; for each
  operation, freeze its target set immediately before that operation changes
  the staging result;
- execute archive groups before loose-tree operations; and
- publish nothing unless every operation, digest, tree, and provenance check
  succeeds.

## Declare an ordered overlay sequence

This example sets one spec tag and adds one project-side file to the
materialized tree. Setting adds the tag when it is absent, updates it when
exactly one match exists, and fails on multiple matches:

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

`spec-set-tag` works on the active spec. `file-add` snapshots the project file
named by `source` and creates the materialized-tree path named by `file`.
`source` is relative to the document that defines the operation; `file` is
relative to the materialized-tree root.

`overlays` is append-composed. Entries keep their array order within a
document, then append in document reach order and inheritance order.
`overlay-files` is different: it is a replacing array of project-contained
TOML files. Those files contain only an `overlays` array, are loaded in array
order, and append their operations after all inline effective overlays. An
overlay file is a secondary semantic document, not an include fragment.

See [Overlay sequences and files](./overlays.md#overlay-sequences-and-files)
and [Common overlay field contracts](./overlays.md#common-overlay-field-contracts).

## Validate and snapshot before changing anything

Before the first edit, a processor validates every operation against the
17-row operation matrix, resolves paths, compiles regular expressions,
snapshots every project-side source, checks archive associations and
cross-scope conflicts, and creates a private staging copy.

Each operation reads the result of earlier operations in its phase. When one
operation has several targets, its complete sorted target set is frozen before
the first target changes. A missing target, wrong cardinality, invalid
intermediate spec, path escape, hash mismatch, or write failure rolls back the
whole component attempt. No partial tree or transformed archive is published.

Every changed or created entry retains operation and source provenance. Exact
validation, staging, target ordering, and rollback rules are in
[Validation, staging, and provenance](./overlays.md#validation-staging-and-provenance).

## Apply archive groups before loose-tree operations

Archive-scoped `file-remove` and `file-search-replace` operations name an
`archive`. They are grouped by exact archive filename. Groups are ordered by
the first occurrence of each archive, and operations within one group retain
their declared relative order. Every archive group completes before the first
non-archive operation. Non-archive operations then run in their original
declared order with the grouped archive entries skipped.

This batching gives one extract, modify, and repack cycle per archive while
preserving deterministic operation order. Conflicts between an archive group
and a loose-file operation that can touch the same top-level archive are
errors before publication. Exact ordering and conflict detection are in
[Ordering and conflicts](./overlays.md#ordering-and-conflicts).

Ordering/rollback sketch: if a declared loose-file edit is followed by
operations for archive A, archive B, and archive A again, the complete A group
runs first, then B, then the loose edit. A failed final hash for either archive
rolls back the complete component attempt. The focused
[overlay operation fixture](./examples/overlay-operations/README.md) checks
grouping, post-overlay hashes, and all-or-nothing publication.

## Work within the bounded archive model

Archive transformations accept the defined tar families with uncompressed,
gzip, xz, or zstd content. Compression is detected from bytes and preserved;
the filename suffix does not override the detected format. Archive parsing
rejects path traversal, duplicate normalized paths, unsupported entry types,
escaping links, malformed framing, unsupported metadata, and trailing data.

Operations work on a deterministic semantic extracted tree: normalized paths,
entry types, regular-file bytes and executable state, explicit or synthesized
directories, and symbolic-link targets. Repacking may use any encoder that
preserves that semantic result and the detected compression family, but the
exact emitted bytes must match the configured post-overlay hash.

Every transformed upstream archive has exactly one matching
`source-files` entry with `origin.type = "overlay"`,
`replace-upstream = true`, a replacement reason, and the required post-overlay
digest. The final `sources` record is rewritten only after that digest passes.
There is no canonical tar/compressor encoder or cross-tool byte-reproducibility
claim.

See [Archive extraction and batching](./overlays.md#archive-extraction-and-batching)
and
[Post-overlay hash and `sources` association](./overlays.md#post-overlay-hash-and-sources-association).

## Choose the operation family

The 17 operations are grouped for lookup rather than repeated here:

- **Spec tags:** `spec-add-tag`, `spec-insert-tag`, `spec-set-tag`,
  `spec-update-tag`, and `spec-remove-tag`.
- **Spec lines and structure:** `spec-prepend-lines`, `spec-append-lines`,
  `spec-search-replace`, `spec-remove-section`, and
  `spec-remove-subpackage`.
- **Patches:** `patch-add` and `patch-remove`.
- **Loose or archive files:** `file-prepend-lines`,
  `file-search-replace`, `file-add`, `file-remove`, and `file-rename`.

The matrix says which common fields are required, optional, or forbidden for
each operation. A forbidden field is an error even when its value is empty or
false. The individual sections in
[Overlay transformations](./overlays.md#spec-tag-operations) define target
selection, exact effects, match cardinality, and postconditions.

## Follow fixed matching and change rules

Tag operations do not have a configurable match mode. `spec-add-tag` requires
zero matches; `spec-set-tag` adds on zero, updates one, and errors on multiple;
`spec-update-tag` requires exactly one; `spec-insert-tag` uses its defined
family insertion rule; and `spec-remove-tag` removes every selected match.
Required tag values cannot be empty or whitespace-only.

Spec text is normalized from LF or CRLF to LF before matching and is reparsed
after every spec or hybrid patch operation. A later spec operation sees only
that reparsed state. Text search/replace uses RE2 with literal replacement
text. Operations that require a change fail when no requested edit occurs.

`patch-add` updates the spec reference and creates the patch file as one atomic
operation. `patch-remove` requires agreement between selected spec references
and loose patch files. File operations never select any `.spec` path through
the file namespace; `file-add` and `file-rename` also reject `.spec`
destinations. Patterns never traverse a symbolic link as a directory and use
sorted normalized paths.

## Publish only a complete valid result

After all operations, the processor checks every postcondition, transformed
archive digest, final `sources` record, tree invariant, and provenance
association. Any failure leaves the prior published artifact unchanged.
Success hands one complete candidate to the materialized-artifact publication
boundary.

## Further reading

- [Overlay metadata](./overlays.md#overlay-metadata)
- [Overlay provenance fixture](./examples/overlay-provenance/README.md)

[Continue: Materialized dist-git](./materialized-dist-git.md)

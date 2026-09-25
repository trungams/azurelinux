[Previous: Transformations](./transformations.md) ·
[Overview](./index.md) ·
[Next: Optional workflows](./optional-workflows.md)

# Materialized dist-git

> **Non-normative reading guide.** This page explains the final core artifact.
> The primary normative owner is
> [Materialized dist-git artifacts](./artifacts.md); cross-cutting owners are
> linked where used.

Core processing produces one build-ready dist-git tree for one resolved
component. It starts from verified local or upstream source, inserts verified
source artifacts, applies the complete overlay pipeline in private staging,
validates the result, and publishes it as one observable transition.

## Assemble the candidate tree

For an upstream component, the exact verified commit supplies the source tree.
The exact top-level `<upstream-name>.spec` is the only active-spec source; the
processor does not scan for another candidate. For a local component,
`spec.path` identifies the active spec inside the local source root.

In both cases the selected spec becomes the single top-level
`<effective-component-name>.spec` with mode `0644`. Other source-tree sidecars
retain normalized source-relative paths. Effective source artifacts become
top-level regular files at their exact filenames. Remaining collisions are
errors rather than implicit overwrites.

A typical result can contain:

```text
hello.spec
sources
hello-1.0.tar.gz
README.md
packaging/macros
```

The exact assembly order and active-spec stem restrictions are in
[Materialization input and candidate assembly](./artifacts.md#materialization-input-and-candidate-assembly).

## Keep one portable tree namespace

Artifact paths are strict UTF-8 relative paths with `/` separators. They
contain no empty, `.`, `..`, or `.git` segment and are compared without case
folding or Unicode normalization. The permitted entry types are directories,
regular files, and symbolic links with contained portable target text.

Directories have mode `0755` and exist only when required by a surviving
descendant. Regular files have exact bytes and mode `0644` or `0755`.
Hardlinks, devices, sockets, FIFOs, submodules, host-only reparse objects, and
other entry types are errors. Materialization never follows a symbolic link as
a directory or writes through one.

Processor-generated caches, locks, logs, temporary files, `.git` state,
failure markers, build outputs, and provenance sidecars are excluded. See
[Tree namespace and permitted entries](./artifacts.md#tree-namespace-and-permitted-entries)
and [Entry placement and exclusions](./artifacts.md#entry-placement-and-exclusions).

## Build the final `sources` manifest

The top-level `sources` path records effective source artifacts. For upstream
components, unchanged comments, blank lines, and unmodified artifact records
retain their exact original bytes and line terminators. Configured
replacements rewrite their existing record positions in canonical modern
form. New configured artifacts append in effective `source-files` order.

Archive-overlay replacements use the verified post-overlay digest. Every
record must agree with the actual top-level artifact bytes. A stale record,
missing file, unrecorded effective artifact, duplicate filename, malformed
emitted record, or wrong digest fails the complete attempt.

The exact byte algorithm is in
[`sources` manifest bytes](./artifacts.md#sources-manifest-bytes).

## Record provenance without adding a tree file

Every successful materialization has a semantic provenance ledger. It is
model state, not a filesystem entry and not a standardized byte format.

For each final non-directory entry other than `sources`, the ledger identifies
its base origin and every ordered transformation that created, renamed,
changed, or retained it. The active spec records its exact selected source and
final path. The `sources` file instead uses a derived aggregate origin
containing its starting manifest and ordered preserved, replacement, and
append contributors.

Different origins or operation histories are not collapsed merely because
they produce equal final bytes. Exact requirements are in
[Provenance association](./artifacts.md#provenance-association).

## Compare observable behavior, not a canonical encoding

Two successful materialized results are behaviorally identical only when
their component identity, complete normalized path set, entry types, regular
file bytes, executable classifications, symbolic-link target text, and
required exclusions are equal.

Revision `0.1` defines no canonical tree serialization, traversal order,
digest framing, comparison algorithm, or conformance tool. Implementations may
index or compare trees however they choose only when every observable property
above is preserved. A transformed archive additionally has exact emitted
bytes constrained by its configured hash, without a universal archive encoder.

See [Behavioral tree identity](./artifacts.md#behavioral-tree-identity) and
[Transformed archive byte boundary](./artifacts.md#transformed-archive-byte-boundary).

## Publish atomically

Acquisition, assembly, transformations, manifest rewriting, hash checks,
artifact validation, and provenance validation all happen in private state.
On success, observers see the new complete tree. On failure, no new local
artifact is published and a previous artifact remains byte-for-byte unchanged.

This local guarantee does not extend to optional remote package or image
publication. The exact local rule is in
[Atomic publication](./artifacts.md#atomic-publication).

## Further reading

- [Materialized artifact fixture](./examples/materialized-artifact/README.md)
- [Overlay provenance fixture](./examples/overlay-provenance/README.md)

[Continue: Optional workflows](./optional-workflows.md)

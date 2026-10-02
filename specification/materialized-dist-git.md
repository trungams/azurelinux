[Previous: Transformations](./transformations.md) ·
[Overview](./index.md) ·
[Next: Optional workflows](./optional-workflows.md)

# Materialized dist-git

This page shows how verified source material and overlay results become one
build-ready dist-git tree. [Materialized dist-git artifacts](./artifacts.md)
contains the binding rules.

Core produces one materialized artifact for each resolved component. In one
private component attempt, the
[`MT-MATERIALIZE` operation](./resolution.md#processing-and-validation-order)
assembles the source tree and verified artifacts, runs the overlay pipeline
described on the previous page once, and validates the final result. It
publishes the complete tree in one observable transition.

## Assemble the candidate tree

For an upstream component, the verified commit supplies the source tree. Its
exact top-level `<upstream-name>.spec` is the only active-spec source. The
processor doesn't scan for or choose another spec. For a local component,
`spec.path` identifies the active spec inside the local source root.

In both cases, the selected spec becomes the single top-level
`<effective-component-name>.spec` with mode `0644`. Other source-tree sidecars
retain normalized source-relative paths. Effective source artifacts become
top-level regular files at their exact filenames. Remaining collisions are
errors; the processor never overwrites them implicitly.

A typical result can contain:

```text
hello.spec
sources
hello-1.0.tar.gz
README.md
packaging/macros
```

The assembly order and active-spec stem restrictions are in
[Materialization input and candidate assembly](./artifacts.md#materialization-input-and-candidate-assembly).

## Keep one portable tree namespace

The materialized tree uses strict UTF-8 relative paths with `/` separators. It
may contain directories, regular files, and contained symbolic links. Regular
files keep their exact bytes and executable state. Directories use mode `0755`
and exist only when a surviving descendant needs them.

The processor rejects escaping paths or links, links used as directories, and
unsupported entry types. Paths contain no empty, `.`, `..`, or `.git` segment
and are compared without case folding or Unicode normalization. Processor
state such as caches, locks, logs, temporary files, `.git`, failure markers,
build outputs, and provenance sidecars stays out of the tree. See
[Tree namespace and permitted entries](./artifacts.md#tree-namespace-and-permitted-entries)
and [Entry placement and exclusions](./artifacts.md#entry-placement-and-exclusions).

## Build the final `sources` manifest

The top-level `sources` file records the effective source artifacts. For an
upstream component, unchanged comments, blank lines, and unmodified artifact
records keep their original bytes and line terminators. A configured
replacement rewrites the record in its existing position using canonical
modern form. New configured artifacts append in effective `source-files`
order.

An archive-overlay replacement uses its verified post-overlay digest. Every
emitted manifest record must agree with the actual top-level artifacts and
their digests. Any mismatch fails the complete component attempt.

[`sources` manifest bytes](./artifacts.md#sources-manifest-bytes) defines the
byte algorithm and individual failures.

## Record provenance without adding a tree file

Materialization records provenance for each final entry. This provenance is
model data, not a file in the tree. The active spec records its selected
source and final path. Apart from `sources`, every final non-directory entry
records its base origin and every ordered transformation that created,
renamed, changed, or retained it. A derived directory identifies the surviving
descendant entries that require it.

`sources` is different because it combines preserved and rewritten manifest
records. Its provenance records the starting manifest and, in order, which
records were preserved, replaced, or appended.

Different origins or operation histories remain distinct even when they
produce equal final bytes. The complete requirements are in
[Provenance association](./artifacts.md#provenance-association).

## Publish atomically

Acquisition, assembly, transformations, manifest rewriting, hash checks,
artifact validation, and provenance validation all happen in private state.
On success, observers see the complete new tree in one transition. On failure,
no new local artifact is published, and the previous artifact remains
byte-for-byte unchanged.

This local guarantee does not extend to optional remote package or image
publication. [Atomic publication](./artifacts.md#atomic-publication) defines
the local rule.

## Compare observable behavior, not a canonical encoding

Two successful results are behaviorally identical when they have the same
component identity, complete normalized path set, and entry type at every
path. Regular-file bytes and executable state, symbolic-link targets, and
required exclusions must also match. The specification does not require one
tree encoding or comparison method.

Transformed archives have one additional byte gate: their exact emitted bytes
must match the configured hash. That result-specific hash is an acceptance
gate, not a canonical encoding rule.

See [Behavioral tree identity](./artifacts.md#behavioral-tree-identity) and
[Transformed archive byte boundary](./artifacts.md#transformed-archive-byte-boundary).

## Further reading

- [Materialized artifact fixture](./examples/materialized-artifact/README.md)
- [Overlay provenance fixture](./examples/overlay-provenance/README.md)

Core processing now has a complete local artifact. Optional workflows can use
it for builds, tests, images, or publication.
[Continue: Optional workflows](./optional-workflows.md)

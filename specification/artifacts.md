[Return to index](./index.md)

# Materialized dist-git artifacts

This chapter defines the observable build-ready dist-git tree produced after
source acquisition and overlay transformation. It defines the artifact
namespace and byte comparison contract without standardizing a command, work
directory, cache, transport, or future semantic transformation model.

## Materialization input and candidate assembly

`MT-MATERIALIZE` consumes one reference-valid resolved component, its verified
immutable source identity, the complete acquired artifact set, every resolved
overlay source snapshot, and the explicit operation/profile inputs required by
the selected materialization operation. It also consumes the immutable
[environmental-input record](./determinism.md#environmental-input-record),
applicable [resource limits](./security.md#resource-limits-and-denial-of-service),
and security context. No undeclared ambient file, process setting, cache
entry, prior output, locale, timezone, wall clock, architecture, macro, or
tool default may affect the candidate tree.

Let `E` be the exact effective component name. For an upstream component, let
`U` be the exact effective `spec.upstream-name` after its default to `E`. `U`
is both the dist-git repository name and the stem of the active spec source
name.

An **active-spec stem** is a non-empty Unicode scalar sequence with an exact
valid UTF-8 representation. It contains no `/`, `\`, `:`, NUL, C0 control, or
U+007F; is not `.` or `..`; does not begin or end with Unicode whitespace or
end with `.`; and is not ASCII-case-insensitively equal to `.git`. The colon,
slash, and backslash exclusions also prohibit URI-scheme, drive-relative,
drive-absolute, UNC, device-root, and alternate-data-stream interpretations.
Because separators are forbidden, no `.git` path segment can be formed; the
explicit `.git` spelling rejection prevents it from being treated as an
administrative basename by a platform adapter. Every other accepted scalar,
including spelling and case, is retained exactly.

During `RM-SOURCE-ID`, before repository or artifact acquisition and before
`MT-MATERIALIZE`, `E` MUST satisfy the active-spec-stem grammar. For an
upstream component, `U` MUST independently satisfy the same grammar after
defaulting. Failure produces no resolved source identity. This is a
materialization cross-invariant, not a global restriction on object names or
references elsewhere in the model, and it does not alter the source URI
placeholder encoding or final-URI validation rules.

The only active-spec candidate is therefore the exact one-segment top-level
Git-tree path `U.spec` in the verified `upstream-commit`; processors MUST NOT
scan for, rank, sanitize, basename, or infer another `*.spec` path. That
candidate MUST be one regular-file Git blob. A missing candidate, a
non-regular candidate, or a source adapter that reports more than one entry or
aliases for that exact path is an error. Other `*.spec` entries neither create
ambiguity nor become active.

The initial private candidate is assembled as follows:

1. For an upstream component, read the exact Git tree at the verified
   `upstream-commit`. Git submodules are unsupported and are an error. Exclude
   the repository's top-level `.git` administrative entry.
2. For a local component, read the entries represented by the local source
   identity. Empty directories are absent because version-1 local identity
   makes them nonsemantic.
3. For an upstream component, select the exact one-segment `U.spec` source path
   above. For a local component, select the exact effective `spec.path`
   relative to its local source root. Copy the selected regular file to the
   single top-level one-segment path `E.spec` with mode `0644`. No basename
   extraction or sanitization occurs. If its source path differs, the source
   spelling is not also retained. A different entry already at `E.spec`,
   including another non-active spec, is a collision and is an error.
4. Copy every other source-tree regular file, directory ancestor, and symbolic
   link at its normalized path, subject to the namespace and entry rules below.
   This retains every non-active `*.spec` at its original path unless step 3's
   canonical-destination collision makes the attempt fail.
5. Insert every effective source artifact as a top-level regular file at its
   exact `filename`, mode `0644`. Replacement intent and collisions have
   already been resolved by [Sources](./sources.md#acquisition-and-collision-order);
   any remaining collision is an error.
6. Construct or update the top-level `sources` manifest under
   [Manifest bytes](#sources-manifest-bytes).
7. Apply the complete [overlay pipeline](./overlays.md#common-transformation-pipeline)
   in the private candidate.
8. Validate the final tree, comparison manifest, and provenance ledger before
   atomic publication.

The candidate assembly and overlay pipeline are one component attempt. A
processor MUST NOT publish the initial tree, an archive-group intermediate,
or a partially overlaid tree.

## Tree namespace and permitted entries

Every artifact path is a strict UTF-8 Unicode scalar sequence using `/` as the
only separator. A path is non-empty and relative, contains no NUL, backslash,
empty, `.`, or `..` segment, and has no `.git` segment. Comparison uses exact
Unicode scalar values without normalization, case folding, or locale
collation. Two paths that the host considers equivalent but whose scalar
sequences differ cannot coexist; inability to represent both exactly is an
error rather than permission to coalesce them.

The root is not an entry. The only permitted entry types are:

| Entry type | Required materialized state |
| --- | --- |
| Directory | Mode `0755`. It MUST be an ancestor of at least one other entry; empty directories are excluded. |
| Regular file | Exact bytes; mode `0644` or `0755` only. |
| Symbolic link | Exact target text satisfying the shared [portable symbolic-link target grammar](./overlays.md#portable-symbolic-link-targets) relative to the link's parent and this tree root. Mode is not semantic. |

Hardlinks, submodules, devices, sockets, FIFOs, mount points, opaque host
reparse objects, ACL-only aliases, and every other entry type are errors. No
path may have a regular file or symbolic link as an ancestor. A symbolic link
may be dangling after lexical containment validation, but materialization and
overlay traversal never follow it as a directory and never write through it.

Directory records are derived from surviving descendants. Removing the final
descendant removes the now-empty directory from the artifact. This agrees with
the version-1 local source identity while allowing an extracted archive to
retain an empty directory inside the archive's own semantic namespace.

The final tree MUST contain exactly one top-level active spec named
`E.spec`, where `E` is the validated exact effective-component active-spec
stem. Another top-level or nested `*.spec` entry is allowed only when it came
from declared source input and no overlay treats it as the active spec; file
overlays still exclude every `.spec` target.

## Entry placement and exclusions

The following placement rules are normative:

- the active spec is the exact one-segment top-level `E.spec`;
- the generated or preserved source manifest is the top-level `sources`;
- every acquired upstream or configured source artifact is a top-level regular
  file at its exact artifact filename;
- transformed archives remain at that same top-level filename;
- source-tree sidecars retain normalized paths relative to the source root;
- `file-add` destinations use their declared normalized tree path; and
- `patch-add` destinations are top-level patch basenames.

An implementation MUST NOT add a synthetic `.git` tree, rendered-tree alias
symlink, cache file, lock file, log, temporary file, editor metadata,
`RENDER_FAILED` marker, build output, or implementation-specific provenance
sidecar. This prohibition concerns processor-synthesized entries. A
source-controlled ordinary file whose name happens to resemble a tool marker
remains an ordinary source entry unless another explicit rule excludes it.

The excluded `render.skip-file-filter` switch has no effect. Entries survive
or are removed only through the source/collision rules and declared
transformations.

## `sources` manifest bytes

The top-level `sources` path is reserved for the effective source-artifact
manifest. For an upstream component, its starting bytes are the verified
upstream `sources` file parsed under
[the manifest grammar](./sources.md#upstream-sources-manifest). For a local
component, a pre-existing top-level `sources` entry is a reserved-path
collision and is an error.

Each parsed upstream line retains its exact content bytes and its terminator:
LF, CRLF, or no terminator for the final line. The final bytes are derived in
this order:

1. Preserve every blank, comment, and unmodified artifact-record line
   byte-for-byte.
2. For each configured replacement, replace the matching artifact-record
   content in its original line position with the canonical modern content
   `HASHTYPE (filename) = lowercase-digest`, retaining that line's original
   terminator.
3. Archive-overlay replacement uses the configured and verified post-overlay
   hash under
   [Post-overlay hash association](./overlays.md#post-overlay-hash-and-sources-association).
4. Append every configured non-replacement artifact in effective
   `source-files` array order, excluding `origin.type = "overlay"`, using one
   canonical modern record followed by LF.

When appending to non-empty starting bytes that lack a final line terminator,
insert one LF before the first appended record. When there are no starting
bytes, the first appended record begins at byte zero. No extra blank line is
inserted. The canonical hash type is the configured `SHA256` or `SHA512`, the
digest is lowercase, and the filename is the exact configured filename.
Every configured filename has already passed the modern manifest filename
grammar before acquisition. A processor MUST construct each replacement or
appended record and reparse it under that same modern record grammar before
publication; a record that does not reproduce the exact configured filename,
hash type, and digest is an error rather than an escaping opportunity.

The path is omitted only when the upstream path was absent and the effective
artifact sequence is empty. An existing empty or comment-only upstream
manifest is preserved. When present, `sources` is a regular file with mode
`0644`.

The bytes recorded in the final manifest and the actual top-level artifact
bytes MUST agree for every record. A missing file, unrecorded effective
artifact, duplicate filename, wrong digest, or stale upstream record is an
error.

## Behavioral tree identity

The materialized-tree contract is defined directly by observable filesystem
state. Two successful materialized results are behaviorally identical only
when all of the following are equal:

- the exact effective component identity;
- the complete set of normalized relative entry paths;
- each entry's directory, regular-file, or symbolic-link type;
- each regular file's complete bytes and executable classification (`0644` or
  `0755`);
- each symbolic link's exact accepted target text; and
- every required empty-directory exclusion and generated-entry exclusion.

No canonical serialization, traversal order, digest framing, semantic
comparison algorithm, or conformance tool is defined for this identity.
Implementations MAY compare or index trees by any method that preserves every
observable property above. A digest is sufficient evidence only when the
digest's non-normative encoding is explicitly agreed outside this
specification.

The tracked
[materialized-artifact fixture](./examples/materialized-artifact/README.md)
provides readable entry manifests for representative success and failure
cases. The manifest notation is fixture metadata, not a standardized artifact
or comparison encoding.

## Provenance association

A successful materialization has an accompanying semantic provenance ledger.
The ledger is not a filesystem entry, and this revision defines its required
semantic records rather than a canonical byte serialization.

Every final non-directory entry other than `sources` MUST identify one base
origin:

- upstream Git commit plus Git-tree path and mode;
- local source identity plus local-source path;
- upstream `sources` record plus acquired digest;
- configured `source-files` entry plus origin and verified digest; or
- a project-side overlay `source` snapshot.

It MUST then record every ordered transformation that created, renamed,
changed, or retained the final entry, including archive group membership and
post-overlay hash association. A derived directory identifies the set of
surviving descendant entries that requires it. The active spec ledger records
the validated exact `E` and, for an upstream component, validated exact `U`,
the exact designated source path, the exact final one-segment path `E.spec`,
and every spec or patch operation. For an upstream component the source path
is the exact one-segment Git-tree path `U.spec` paired with the verified
commit; for a local component it is the exact normalized path of `spec.path`
relative to the local source root.

The final `sources` file instead has one **derived sources aggregate** origin.
It is not forced into one ordinary base-origin choice. That origin contains:

1. the optional starting manifest origin and SHA-256 of its exact bytes, or an
   explicit `absent` marker for a local/generated manifest;
2. one contributor for every starting line in original order, identifying the
   exact preserved line bytes or the original record replaced at that
   position;
3. for each replacement, the configured `source-files` entry, verified
   artifact digest, canonical emitted record, and, for an archive overlay, the
   ordered archive operations and post-overlay hash association; and
4. for each append, in effective `source-files` order, the configured entry,
   verified artifact digest, and canonical emitted record.

The contributor sequence is sufficient to reproduce the final manifest-byte
algorithm above. Comments, blank lines, preserved records, replacements, and
appends are distinct contributor kinds. The final ledger records the aggregate
origin plus the manifest-rewrite transformation; it MUST NOT select one
artifact contributor as an arbitrary base origin.

Two path aliases, two different acquired bytes, or two different operation
sequences MUST NOT be collapsed into one provenance event merely because the
final regular-file bytes agree. Provenance is diagnostic and semantic model
state; it does not add bytes to the materialized tree.

## Atomic publication

All acquisition, candidate assembly, archive extraction/repacking, overlay
application, manifest rewriting, behavioral-identity validation, hash
verification, and final validation occur in private staging state. This section
owns only the local materialized-artifact commit. Remote package and image
publication is governed by the selected-operation boundary in
[Profiles](./profiles.md#publishing-boundary) and does not inherit a rollback
guarantee from this local boundary.

On success, the complete validated tree becomes the component's published
materialized artifact as one observable transition. On any failure:

- no new artifact is published;
- a previously published artifact at the destination remains byte-for-byte
  unchanged;
- no failure marker or partial tree is substituted; and
- temporary staging state is not a conforming output.

An implementation may use atomic directory rename, content-addressed
publication, or another mechanism, but readers MUST observe either the prior
complete artifact or the new complete artifact, never a mixture. If the host
cannot provide that observable guarantee, publication is unsupported.

## Transformed archive byte boundary

Regular-file identity includes the complete emitted transformed-archive
bytes, and those bytes MUST match the configured post-overlay hash. The
normative archive transformation output is nevertheless the semantic
extracted-tree result defined by
[Overlay transformations](./overlays.md#archive-extraction-and-batching), with
content-detected compression preserved.

Revision `0.1` defines no canonical tar/compressor encoder, encoder version
registry, or cross-tool archive-byte reproducibility guarantee. Different
tools may be unable to produce the configured bytes from the same semantic
tree; such a producer reports unsupported transformed-archive encoding or
hash mismatch instead of publishing different bytes.

The Materialized-tree class is not enabled or claimed. That status does not
weaken the artifact namespace, exact behavioral
tree identity, placement, exclusions, provenance association, configured-hash
gate, or atomic success/failure rules in this chapter.

[Return to index](./index.md)

# Decision status and open decisions

> **Non-normative review material.** This chapter records alternatives,
> resolutions, observable consequences, evidence, and recommendations. It does not define
> `MUST`, `SHOULD`, or `MAY` requirements, even when those words appear while
> describing another chapter. Normative chapters link here to make an
> unresolved product decision or the history of a resolved decision visible.

## OD-1: Repeated canonical-document evaluation

**Status: resolved on 2026-09-24.**

Each canonical document is evaluated once at its first deterministic
depth-first traversal position. Later non-cycle reaches reuse the cached
validated source model, do not add another composition contribution or traverse
the document's outgoing includes again, and add only incoming-reach
provenance.

This prevents duplicate append-composed operations while retaining every graph
edge for diagnostics. The normative rule and fixture are in
[Loading](./loading.md#cycles-and-repeated-reaches).

## OD-2: Inheritance layer precedence

**Status: resolved on 2026-09-24.**

The fixed low-to-high order is selected distro-version defaults, top-level
project defaults, the component-group aggregate, then direct component
configuration. Each boundary uses typed composition and the explicit
field-level exceptions defined by the owning chapter.

This preserves reusable partial defaults while making direct configuration the
highest-precedence source. The normative rule is in
[Resolution](./resolution.md#component-inheritance-order).

## OD-3: Multiple component groups

**Status: resolved on 2026-09-25.**

Revision `0.1` defines neither lexical-name precedence nor a priority field.
Several groups may contribute append-composed arrays or demonstrably disjoint
table/map leaves. Repeated non-append leaves are destructive overlap errors,
including equal values.

Group names are sorted by UTF-8 bytes only to construct the sequence of
append-composed array elements; that ordering cannot select a winning scalar,
replacing array, or map leaf. Explicit group priorities are deferred until a
later revision has concrete use cases.

## OD-4: Canonical resolved-model serialization

**Status: resolved on 2026-09-25.**

Revision `0.1` defines behavioral typed-object and materialized-tree identity,
not canonical JSON, canonical TOML, another byte serialization, a semantic
comparison algorithm, or a conformance tool. Resolved behavior covers typed
values, object and reference identities, array order, immutable source
identities, and semantic provenance. Materialized-tree behavior covers exact
paths, entry types, file bytes, executable state, and symbolic-link targets.

Fixture metadata may select typed leaves or enumerate tree entries for review
without becoming a standardized encoding.

## OD-5: Final core and profile assignment

**Status: resolved on 2026-09-24.**

Core covers document loading and composition, resolution, components, source
acquisition, overlays, and materialized dist-git production. RPM build, images,
tests, publishing, and executable custom-source generation are optional
profiles. Repository resources are shared optional data for RPM-build and
image operations. Their presence requires validation and preservation but does
not select an operation. Tool configuration is tool-specific and outside the
portable source-document vocabulary.

## OD-6: Overlay match cardinality

**Question:** What should happen when operations intended to identify one RPM
tag encounter duplicate matching tag lines?

**Affected operations:** `spec-add-tag`, `spec-set-tag`, and
`spec-update-tag`.

**Alternative A - singleton-safe errors**

- `spec-add-tag` succeeds only with zero matches.
- `spec-set-tag` adds on zero, updates on one, and errors on several.
- `spec-update-tag` requires exactly one.
- Prevents silently treating malformed or conditionally duplicated singleton
  tags as one arbitrary target.

**Alternative B - all-match behavior**

- `spec-add-tag` may append another instance.
- Set/update may change every matching line.
- Supports legitimate repeatable tags but can unexpectedly rewrite duplicate
  singleton tags.

**Alternative C - first-match behavior**

- Set/update changes the first parsed match only.
- Depends on source order and leaves other duplicates untouched.

**Evidence:** Current documentation says `spec-add-tag` fails when the tag
exists, while current runtime `AddTag` appends indiscriminately. Set/update
comments describe the first instance, while the visitor currently continues
and appears to update every matching instance. Repeatable tag families also
make one global policy risky.

**Recommendation and current containment:** Prefer explicit per-operation or
per-tag multiplicity in a future semantic model. For the current low-level API,
retain the normative provisional singleton-safe errors in
[Overlay transformations](./overlays.md#spec-tag-operations). They make the
present result deterministic without pretending the product decision is
closed. `spec-insert-tag` and `spec-remove-tag` already define deliberate
multi-instance behavior and are not gated by this question.

## OD-7: Canonical transformed-archive bytes

**Question:** Which exact tar headers, metadata, padding, compression
parameters, and encoder versions produce portable byte-identical transformed
archives?

**Alternative A - one fully specified portable encoder**

- Defines tar header format, field encodings, metadata, entry order, padding,
  and compressor parameters for every supported compression.
- Makes transformed archives independently reproducible.
- Requires compatibility commitments for xz and zstd bitstreams or embedded
  reference vectors.

**Alternative B - semantic archive tree plus configured byte hash**

- Defines one extracted-tree result and accepts an encoding only when its exact
  bytes match the declared post-overlay hash.
- Prevents incompatible successful output for one declaration.
- Does not let a new implementation derive the expected bytes without a
  compatible encoder.

**Alternative C - restrict transformed archives**

- Prohibits archive transformation until one canonical encoder is published,
  or supports only a smaller format such as uncompressed tar.
- Maximizes portability but drops current gzip/xz/zstd use cases.

**Evidence:** Current azldev sorts entries, pins timestamps and ownership,
normalizes gzip headers, and preserves content-detected compression. Exact xz
and zstd bytes still depend on library behavior, and source archives can
contain metadata or format variations without a published portable
normalization contract.

**Recommendation and current containment:** Retain Alternative B for this
revision: the overlay chapter defines the semantic archive result and requires
the configured post-overlay hash as the byte-level success gate. Portable
transformed-archive byte claims and the Materialized-tree class remain
prohibited. Before enabling them, publish one canonical encoder contract and
cross-implementation vectors, or deliberately narrow supported formats.

## Decisions assigned to later work

The following approved questions remain open and are intentionally assigned to
later work:

| Decision | Owning slice |
| --- | --- |
| Overlay match-cardinality closure (OD-6) | Human/product review |
| Canonical transformed-archive bytes (OD-7) | Human/product review |
| Long-term version compatibility and deprecation | Later revision |
| Future semantic component-transformation model | Future specification revision |

Environmental-input classification and defaults are no longer an open product
decision. [Determinism](./determinism.md) and the tracked environmental-input
matrix classify architecture, locale, timezone, wall clock, process
environment, RPM macros, network context, toolchains, credentials, and
resource limits.

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

**Status: resolved on 2026-09-25.**

Revision `0.1` uses fixed operation-specific behavior and adds no configurable
match field:

- `spec-add-tag` requires zero matching tag names;
- `spec-set-tag` adds on zero, updates one, and errors on multiple;
- `spec-update-tag` requires exactly one;
- `spec-insert-tag` retains repeatable-family insertion semantics; and
- `spec-remove-tag` retains all-match removal, optionally filtered by one
  exact non-empty parsed value.

This decision avoids arbitrary first-match selection and prevents a hidden
global multiplicity policy from changing operation meaning. The normative
rules and executable cardinality fixtures are in
[Overlay transformations](./overlays.md#spec-tag-operations).

Configurable modes such as first, last, all, or exactly N may be considered in
a later revision only with concrete use cases and a schema extension. They are
non-normative future work and do not alter revision `0.1`.

## OD-7: Canonical transformed-archive bytes

**Status: resolved on 2026-09-25.**

Revision `0.1` standardizes the semantic extracted-tree transformation result,
preserves the compression family detected from the input content, and requires
the exact emitted archive bytes to match the configured post-overlay hash.
An implementation that cannot emit matching bytes fails rather than publishing
a different archive.

Revision `0.1` does not define a canonical tar/compressor encoder, encoder
version registry, or cross-tool archive-byte reproducibility guarantee. The
semantic result and configured-hash gate are the complete normative boundary;
lack of a universal encoder is not an unresolved product decision and does not
weaken the required byte hash.

A future specification may define a canonical encoder and cross-implementation
vectors or deliberately narrow supported formats. That work is non-normative
for revision `0.1`.

## Non-normative future work

The following topics are intentionally deferred and do not change revision
`0.1`:

| Topic | Owning revision |
| --- | --- |
| Configurable tag match modes or match fields | Later schema revision |
| Canonical archive encoder and cross-tool byte vectors | Later archive/conformance revision |
| Long-term version compatibility and deprecation | Later revision |
| Future semantic component-transformation model | Future specification revision |

Environmental-input classification and defaults are no longer an open product
decision. [Determinism](./determinism.md) and the tracked environmental-input
matrix classify architecture, locale, timezone, wall clock, process
environment, RPM macros, network context, toolchains, credentials, and
resource limits.

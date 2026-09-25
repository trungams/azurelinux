[Previous: Configuration files](./configuration-files.md) ·
[Overview](./index.md) ·
[Next: Source material](./source-material.md)

# Project model

> **Non-normative reading guide.** This page explains how the main objects fit
> together. The primary normative owners are
> [Top-level object model](./objects.md), [Project](./project.md),
> [Distros and versions](./distros.md), [Components](./components.md),
> [Component groups and discovery](./component_groups.md), and
> [Resolution](./resolution.md); cross-cutting owners are linked where used.

A project names the distributions, components, defaults, groups, and optional
workflow data that a processor can use. The common case declares a distro,
then declares components directly or groups them for shared configuration.

**First-pass takeaways:**

- the resolution request supplies one immutable target distro/version pair;
- a provider below is one already-composed partial configuration from an
  inheritance layer; and
- groups share configuration without forming a precedence ladder, while
  optional profile data never executes itself.

## Declare a distro and a component

This separate upstream variant defines one upstream component. Because its
complete `upstream-distro` table is absent, `project.default-distro` supplies
that source-repository reference:

```toml
spec-version = "0.1"

[project]
description = "Example distribution project"
default-distro = { name = "azurelinux", version = "4.0" }

[distros.azurelinux]
dist-git-base-uri = "https://src.example.invalid/rpms/$name.git"
lookaside-base-uri = "https://src.example.invalid/lookaside/$name/$filename/$hashtype/$hash/$filename"

[distros.azurelinux.versions."4.0"]
release-ver = "4.0"
dist-git-branch = "4.0"

[component-groups.core]
components = ["hello"]

[components.hello.spec]
type = "upstream"
upstream-commit = "0123456789abcdef0123456789abcdef01234567"
```

The exact commit is the portable source identity. `lookaside-base-uri` names a
distro-hosted, hash-addressed source-artifact store. The branch can help a
producer refresh a pin, but it does not replace the effective
`upstream-commit`. The resolution request must separately supply the target
distro/version input used for resolution. `project.default-distro` selects a
source repository context only; it never chooses or changes that target.

Exact field contracts are in [Project](./project.md),
[Distro fields](./distros.md#distro-fields), and
[Upstream component source](./sources.md#upstream-component-source).

## Understand the top-level objects

The root table is closed. Its portable object families are:

- `project`, for descriptive values and a default source-distro reference;
- `distros`, whose versions provide release values and optional component
  defaults;
- `components`, the named components that become materialized dist-git trees;
- `component-groups`, which provide membership, discovery, and shared partial
  component configuration;
- `default-component-config`, the project-wide partial component default;
- `resources`, `images`, `default-package-config`, `package-groups`, `tests`,
  and `test-groups`, which are optional profile data;
  and
- document controls `spec-version` and `includes`.

Unknown top-level keys and alternate spellings are errors. Name-keyed maps use
exact Unicode names without case folding or normalization. A missing,
ambiguous, or wrong-kind reference fails instead of choosing a near match.
The canonical list and common name rules are in
[Canonical top-level vocabulary](./objects.md#canonical-top-level-vocabulary)
and [Names and references](./objects.md#names-and-references).

## Resolve the target before inheritance

Every resolution evaluation receives an exact
`(target-distro, target-version)` pair from its resolution request. That pair
is not a TOML field, host default, or source selector. It must identify one
declared distro version and remains fixed for the complete evaluation.

The selected distro version supplies the first component-default layer. Source
fields such as `spec.upstream-distro`, and the project fallback used in the
example, identify where upstream source comes from. They cannot reselect the
target provider. See
[Target distro/version evaluation input](./resolution.md#target-distroversion-evaluation-input).

## Apply defaults and inheritance in one fixed order

Each provider has already combined all source-document contributions to that
inheritance layer before inheritance begins.
Resolution then combines partial `ComponentConfig` values from low to high
precedence:

1. the selected distro-version `default-component-config`;
2. the top-level `default-component-config`;
3. the aggregate of every applicable component group; and
4. the direct `components.<name>` configuration.

Tables and maps merge recursively, later scalars replace earlier scalars, and
replacing arrays replace the earlier array. The `overlays` sequence is the
important append exception: entries retain provider and element order. A field
default is applied only after all four layers and only when its owner defines
one. Absence, `false`, zero, an empty string, an empty array, and an empty
table are not interchangeable.

The full rule, including provenance, is in
[Component inheritance order](./resolution.md#component-inheritance-order).
Individual field chapters define whether a value is required, its default,
and any composition exception.

## Use groups for shared, non-conflicting contributions

A component group can list explicit component names, discover components from
project-contained `.spec` path patterns, and provide a partial
`default-component-config`. Discovery is deterministic: patterns run in array
order, matches are sorted by normalized project-relative path, exclusions run
after positive matches, and repeated reaches of one canonical spec path are
coalesced while retaining all provenance occurrences. Only distinct paths
that derive the same component name collide and cause an error.

Several groups do not form a precedence ladder. They may append
append-composed arrays or contribute disjoint table/map leaves. If two groups
contribute the same non-append leaf, resolution fails even when the values are
equal. Group-name UTF-8 order is used only to order appended elements; it
cannot select a winning scalar or replacing array. Revision `0.1` has no group
priority field.

See [Discovery pattern dialect](./component_groups.md#discovery-pattern-dialect),
[Discovered component construction](./component_groups.md#discovered-component-construction),
and the group-layer rules in
[Component inheritance order](./resolution.md#component-inheritance-order).

Result/failure sketch: one group may contribute
`build.defines = { hardening = "1" }` while another contributes a disjoint
`build.defines = { tracing = "1" }`; both leaves survive. If both groups set
`release.calculation`, resolution fails even when the two strings are equal.
The focused
[inheritance and component-group fixture](./examples/inheritance-overlap/README.md)
checks both outcomes.

## Keep core and optional profile data distinct

Component construction, source acquisition, overlays, and materialized
dist-git are core. Build, release, package publishing, image, test, RPM
repository-resource, and executable custom-generation fields are canonical
optional profile data. Their presence still requires validation and
preservation, but it does not run an operation.

An optional operation begins only through an explicit request using one of the
defined operation identifiers. A processor that cannot support the selected
operation returns `unsupported-operation` before side effects; it does not
discard valid unselected data. The complete split is in
[Optional profile data and operations](./profiles.md).

## Produce one resolved model or an error

Resolution reconciles discovered and explicit components, applies inheritance
and defaults, resolves references, checks effective invariants, and converts
upstream selectors to exact commits. A later phase cannot repair an earlier
error. Success produces one typed resolved object graph with relocatable
provenance; failure produces no resolved model for that evaluation.

The exact phase order is in
[Processing and validation order](./resolution.md#processing-and-validation-order).
The behavioral boundary deliberately does not define canonical JSON, TOML, a
comparison stream, or a conformance tool.

## Further reading

- [Field disposition inventory](./field-disposition.tsv)
- [Field/profile matrix](./profile-matrix.tsv)

[Continue: Source material](./source-material.md)

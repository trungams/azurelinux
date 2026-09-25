[Return to top-level objects](./objects.md)

# Component groups and discovery

`component-groups` is a name-keyed map of closed group tables. A group can name
explicit components, discover components from spec paths, and supply partial
component defaults.

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `component-groups.<group>.description` | string | Optional; no default. | Descriptive UTF-8; path base N/A. | Scalar replace. | N/A. | No processing semantics. | Core |
| `component-groups.<group>.metadata` | Closed `OverlayMetadata` table | Optional; no default. | URL and enum constraints are defined in [Overlay metadata](./overlays.md#overlay-metadata). | Recursive table. | N/A. | Metadata does not affect group matching. | Core |
| `component-groups.<group>.components` | Array of component-name references | Optional; default `[]`. | Exact names; path base N/A. | Array replace. | Establishes this group as a candidate provider for each member. | Duplicate names or references to no explicit/discovered component are errors. | Core |
| `component-groups.<group>.specs` | Array of discovery path patterns | Optional; default `[]`. | Relative to the defining document using the [portable pattern syntax](./loading.md#portable-relative-path-pattern-syntax) and [project-tree traversal](./loading.md#project-tree-pattern-traversal). | Array replace. | Discovered components receive this group as a candidate provider. | Each successful expansion is sorted by normalized semantic path. A distinct-path same-name collision is an error. | Core |
| `component-groups.<group>.excluded-paths` | Array of discovery path patterns | Optional; default `[]`. | Relative to the defining document using the [portable pattern syntax](./loading.md#portable-relative-path-pattern-syntax) and [project-tree traversal](./loading.md#project-tree-pattern-traversal). | Array replace. | N/A. | Applied after `specs`; an excluded match contributes no component. A successful zero-match pattern is allowed. | Core |
| `component-groups.<group>.default-component-config` | Closed partial `ComponentConfig` | Optional; no default. | Reuses all component field contracts and provenance. | Recursive component-config composition. | Candidate contribution to the component-group layer for every member/discovered component. | Partial config allowed; field profile assignments are unchanged; destructive overlap with another group is an `RM-EFFECTIVE` error. | Core |

## Discovery pattern dialect

Discovery patterns import both the
[portable relative-path pattern syntax](./loading.md#portable-relative-path-pattern-syntax)
and [project-tree pattern traversal](./loading.md#project-tree-pattern-traversal),
including recursive `**`. Thus `SPECS/**/*.spec` is valid discovery syntax
while `ab**cd.spec` is not.

In addition to those reusable lexical and traversal contracts, component
discovery requires
the final result to be a contained regular file whose final segment ends in
the literal suffix `.spec`; directories and symlinks are not matches. A
successful zero-match expansion contributes nothing. The `.spec` terminal
filter is specific to component discovery, not part of either reusable
pattern contract.

Patterns in each array are evaluated in array order, with each pattern's
matches sorted by normalized project-relative path. A canonical spec path
reached by more than one pattern is coalesced, not rediscovered; all matching
pattern occurrences remain in provenance. Exclusion patterns use the same
dialect and remove exact canonical matches after all positive patterns have
expanded.

## Discovered component construction

The component name discovered from a spec path is the basename with exactly one
terminal `.spec` suffix removed. The result must satisfy the common name rule.
Discovery does not read RPM `Name:` syntax during object construction.

One distinct canonical matched path synthesizes this partial
`ComponentConfig`:

```toml
[components.example.spec]
type = "local"
path = "<normalized-project-relative-matched-path>"
```

The synthesized `spec.path` string is the normalized project-relative match
and its defining path base is the project root. The synthesized component,
`spec.type`, and `spec.path` provenance records the specification discovery
rule, group name, group-defining source-document identity, `specs` array
index, original pattern, and normalized matched path. When several patterns or
groups reach the same canonical file, their ordered discovery occurrences are
all retained.

A discovered component is a member of every group that discovered its
canonical path. Within one group, the ordered membership output is the
declared `components` sequence followed by discovered members in normalized
project-relative path order, omitting a discovered name already explicitly
listed. This ordering is an observable discovery result only; it does not
select inheritance precedence. Across groups, discovery occurrences are
ordered by group-name UTF-8 bytes, then `specs` array index, then matched
project-relative path. Processors enumerate the resulting component identities
by component-name UTF-8 bytes.

After source-document composition and before `RM-EFFECTIVE`, discovery and
explicit components are reconciled by this matrix. Several occurrences of the
same canonical spec file count as one distinct discovery.

| Distinct discovered spec paths for a name | No explicit component | Explicit component with no `spec` | Explicit identical local `spec` | Explicit different local `spec` | Explicit upstream `spec` |
| --- | --- | --- | --- | --- | --- |
| zero | No component is synthesized. | Keep the explicit partial component; ordinary effective requiredness still applies. | Keep the explicit component. | Keep the explicit component. | Keep the explicit component. |
| one | Create the synthesized local component. | Add the synthesized `spec` and retain every explicit non-`spec` field. | Keep the explicit `spec` and its provenance; add discovery membership/provenance. | Error. | Error. |
| two or more | Error. | Error. | Error. | Error. | Error. |

An explicit local `spec` is identical only when it resolves to the same
canonical regular spec file as the discovered path and contains no
variant-forbidden field. A present but incomplete `spec`, a different source
type, or a different canonical path is not identical. Collision errors produce
no partial component set. The
[discovery fixture](./examples/component-discovery/README.md) covers recursive
matching and every one-path collision outcome.

The complete provider order and group-layer composition are defined in
[Component inheritance order](./resolution.md#component-inheritance-order).
Revision `0.1` has no lexical-name precedence and no group-priority field.
Several groups may contribute append-composed arrays or demonstrably disjoint
table/map leaves. Any same non-append leaf contributed by several groups is a
destructive overlap error, even when the values are equal.

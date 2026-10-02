[Previous: Configuration files](./configuration-files.md) ·
[Overview](./index.md) ·
[Next: Source material](./source-material.md)

# Project model

This guide shows how a project declares distros and components, shares
settings through defaults and groups, and produces the resolved model used by
later stages. The common case declares one distro and one or more components;
the following sections explain each object or step and link its binding rules
where they are used.

## Declare a distro and a component

This standalone example declares one upstream component. It omits
`upstream-distro`, so `project.default-distro` supplies the source-repository
context:

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

The full `upstream-commit` is the portable source identity.
`lookaside-base-uri` names a distro-hosted, hash-addressed source-artifact
store. The branch can help a producer refresh a pin, but it does not replace
the effective `upstream-commit`.

The resolution request must separately supply the target distro/version input
used for resolution. `project.default-distro` selects only the source
repository context. It never chooses or changes the target.

[Project](./project.md) and [Distro fields](./distros.md#distro-fields) define
the project and distro values.

[Upstream component source](./sources.md#upstream-component-source) defines
the source reference and commit fields.

## Understand the top-level objects

The root table is closed. It recognizes these portable object families:

- The [project](./project.md) table holds descriptive values and a default
  source-distro reference.
- Each [distro](./distros.md) entry provides versions, release values, and
  optional component defaults.
- Named [components](./components.md) become materialized dist-git trees.
- A [component group](./component_groups.md) provides membership, discovery,
  and shared partial component configuration.
- The top-level `default-component-config` is the project-wide partial
  component default.
- Optional profile data lives under `resources`, `images`,
  `default-package-config`, `package-groups`, `tests`, and `test-groups`.
- Document controls are `spec-version` and `includes`.

Unknown top-level keys and alternate spellings are errors. Name-keyed maps use
exact Unicode names without case folding or normalization. A missing,
ambiguous, or wrong-kind reference fails instead of choosing a near match.

The [Top-level object model](./objects.md) defines these root objects.
Details: [Canonical top-level vocabulary](./objects.md#canonical-top-level-vocabulary)
and [Names and references](./objects.md#names-and-references).

## Resolve the target before inheritance

Every resolution request must supply an exact
`(target-distro, target-version)` pair. The pair comes from the request, not a
TOML field, host default, or source selector. It must identify one declared
distro version and remains fixed throughout evaluation.

The selected distro version supplies the first component-default layer.
`spec.upstream-distro` and the project fallback used in the example identify
where upstream source comes from. They cannot reselect or change the target
distro/version.

Details:
[Resolution](./resolution.md) defines the complete model; the
[Target distro/version evaluation input](./resolution.md#target-distroversion-evaluation-input)
section defines this required pair.

## Apply defaults and inheritance in one fixed order

Each inheritance layer may receive values from several source documents. Those
values are composed first, producing one partial `ComponentConfig` for that
layer; the reference calls this partial configuration a provider. Resolution
then combines the four providers from low to high precedence:

1. the selected distro-version `default-component-config`;
2. the top-level `default-component-config`;
3. the aggregate of every applicable component group; and
4. the direct `components.<name>` configuration.

Tables and maps merge recursively. Later scalars replace earlier scalars, and
replacing arrays replace the earlier array. The `overlays` sequence is the
important append exception: entries retain provider and element order.

A field default is applied only after all four layers, and only when its owner
defines one. Absence, `false`, zero, an empty string, an empty array, and an
empty table are not interchangeable. Individual field chapters define whether
a value is required, its default, and any composition exception.

Details:
[Component inheritance order](./resolution.md#component-inheritance-order).

## Use groups for shared, non-conflicting contributions

Component groups share partial configuration across named or discovered
components. Groups do not have priority over one another. They may append
append-composed arrays or contribute disjoint table or map leaves. Resolution
fails if two groups set the same non-append leaf, even to the same value.

A group can list component names or discover `.spec` files with
project-contained patterns. Discovery is deterministic, and repeated matches
for the same file are coalesced. Distinct files that derive the same component
name collide and fail. The reference defines pattern order, exclusions,
provenance, and append ordering.

For example, one group may contribute
`build.defines = { hardening = "1" }` while another contributes a disjoint
`build.defines = { tracing = "1" }`; both leaves survive. If both groups set
`release.calculation`, resolution fails even when the two strings are equal.
The [inheritance and component-group example](./examples/inheritance-overlap/README.md)
checks both outcomes.

The [Discovery pattern dialect](./component_groups.md#discovery-pattern-dialect)
defines matching, and
[Discovered component construction](./component_groups.md#discovered-component-construction)
defines the synthesized component.

[Component inheritance order](./resolution.md#component-inheritance-order)
defines how group contributions enter resolution.

## Keep core and optional profile data distinct

Declaring optional profile data does not run anything. Component construction,
source acquisition, overlays, and materialized dist-git are core. Settings for
builds, releases, package publishing, images, tests, RPM repository resources,
and custom generators are optional profile data. They are validated and
preserved, but an operation begins only when it is explicitly requested. If
the processor cannot support that operation, it returns
`unsupported-operation` before side effects.

Details: [Optional profile data and operations](./profiles.md).

## Produce one resolved model or an error

Resolution combines the component settings, applies defaults, resolves
references, validates the result, and pins upstream sources to full commits. A
later phase cannot repair an earlier error. If resolution succeeds, source
acquisition receives the resolved component and its provenance. If it fails,
no resolved model is produced. The reference does not define a serialization
format or conformance tool.

Details:
[Processing and validation order](./resolution.md#processing-and-validation-order).

## Further reading

- [Field disposition inventory](./field-disposition.tsv)
- [Field/profile matrix](./profile-matrix.tsv)

[Continue: Source material](./source-material.md)

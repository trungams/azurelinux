# Distro TOML specification

Distro TOML describes how source configuration and declared inputs become one
build-ready materialized dist-git tree. It standardizes portable data and
observable behavior, not a command, implementation language, cache, work, or
storage layout.

```toml
spec-version = "0.1"

[distros.azurelinux.versions."4.0"]
release-ver = "4.0"

[components.hello.spec]
type = "local"
path = "pkg/hello.spec"

[[components.hello.overlays]]
type = "file-add"
file = "azurelinux.conf"
source = "files/azurelinux.conf"
```

This root document describes one local component. Resolve it with the
target distro/version input `azurelinux` / `4.0`. `spec.path` and overlay
`source` resolve from their defining document; overlay `file` is relative to
the materialized-tree root. Here, `file-add` reads the project source path
`files/azurelinux.conf` and creates the materialized-tree destination path
`azurelinux.conf`. The result also contains the local dist-git content and one
active `hello.spec`.

## From TOML to materialized dist-git

A processor loads the root file and its includes, combines their values,
applies defaults and inheritance, resolves references and immutable source
identities, acquires declared inputs, runs the ordered overlays, validates the
candidate, and atomically publishes one materialized dist-git tree.

The reference chapters call the loaded files
[source documents](./document.md), the combined configuration the
[composed model](./resolution.md#composed-model), and the effective
configuration the
[resolved model](./resolution.md#resolved-model). They define the exact
[processing order](./resolution.md#processing-and-validation-order), how
[archives are transformed and hashed](./overlays.md#archive-extraction-and-batching),
how final entries retain a
[materialization provenance ledger](./artifacts.md#provenance-association),
how optional work is explicitly
[selected](./profiles.md#selected-operation-contracts), and how failures are
reported through [structured diagnostics](./validation.md#diagnostic-record).

## Read next

1. [Configuration files](./document.md): syntax, strict keys, and document
   identity; then [loading, includes, and composition](./loading.md).
2. [Project model](./objects.md): [project](./project.md),
   [distros](./distros.md), [components](./components.md), and
   [component groups](./component_groups.md); then
   [Resolution](./resolution.md) for defaults, inheritance, references, and
   effective values.
3. [Source material](./source-material.md): the common source workflow, with
   links to the unchanged exhaustive source reference.
4. [Transformations](./overlays.md): overlay order, atomicity, archives, and
   all 17 operations.
5. [Materialized dist-git](./artifacts.md): final paths, bytes, provenance, and
   atomic publication.
6. [Optional workflows](./profiles.md): [RPM build](./build.md),
   [repositories](./resources.md), [publishing](./packages.md),
   [images](./images.md), and [tests](./tests.md).
7. [Determinism](./determinism.md), [validation](./validation.md), and
   [security](./security.md).
8. [Conformance examples](./conformance.md) and
   [excluded or deferred fields](./compatibility.md).

## Requirement language

**MUST**, **MUST NOT**, and **REQUIRED** state conformance requirements.
**SHOULD** and **SHOULD NOT** permit a deviation only when its consequences are
understood and the implementation documents why it is necessary. **MAY** states
optional behavior; an implementation that omits it remains conforming.
Uncapitalized forms are descriptive. Text labeled **Open decision**, **Review
note**, **Example**, or **Non-normative** creates no requirement.

An **error** means that the affected document, resolved object, component, or
materialization attempt is non-conforming and MUST NOT produce a successful
result. Required diagnostic information is defined where it affects
interoperability, but exact diagnostic wording is not standardized.

The root document selects this revision with `spec-version = "0.1"`; the full
identity and vocabulary contract is in [Document format](./document.md).

## Conformance status

| Conformance class | Status |
| --- | --- |
| **Source-document** | Not enabled or claimed |
| **Loading/composed-model** | Not enabled or claimed |
| **Resolved-model** | Not enabled or claimed |
| **Materialized-tree** | Not enabled or claimed |

No class is enabled or claimed. Revision `0.1` defines no class lifecycle,
claim protocol, required suite, or transition rule. Exact input and observable
boundaries, phase identifiers, whole-chapter owners, and identity/equality
requirements are in the
[detailed conformance boundaries](./conformance.md#conformance-boundaries).

The retained ordinary positive/negative examples and traceability are defined
in [Conformance](./conformance.md). Those examples do
not enable or establish a claim.

## Boundaries and deferred work

Revision `0.1` does not standardize implementation libraries or
implementation-specific fingerprints. Existing tool behavior is evidence, not
authority; behavior is normative only when this specification states it.

Exact non-goals and deferrals are retained in
[Conformance](./conformance.md#explicit-deferrals-and-non-goals),
[profile output exclusions](./profiles.md#status-and-output-exclusions), and
[Compatibility](./compatibility.md). Resolved decisions and remaining future
work are recorded, non-normatively, in
[Decision status and open decisions](./open-decisions.md).

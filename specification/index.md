# Distro TOML specification

Distro TOML tells a processor how to turn project configuration and declared
inputs into one build-ready materialized dist-git tree for each component
after resolution. It defines portable data and observable results. It does not
prescribe a command-line interface, implementation language, cache, workspace,
or storage layout.

The seven reading guides explain that workflow. They are non-normative; the
reference chapters they link contain the binding rules.

The following standalone example assumes that `pkg/hello.spec` and
`files/azurelinux.conf` already exist and are valid.

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
`azurelinux.conf`. On successful processing with those inputs, the result also
contains the local dist-git content and one active `hello.spec`.

## From TOML to materialized dist-git

A processor first loads the root file and its includes, then combines their
values. It applies defaults and inheritance, resolves references and immutable
source identities, and acquires the declared inputs. It runs the overlays in
order, validates the candidate, and atomically publishes one materialized
dist-git tree for each resolved component.

The specification calls the loaded files
[source documents](./document.md) and the combined configuration the
[composed model](./resolution.md#composed-model). After defaults and
inheritance are applied, references are resolved, the resulting values pass
validation, and source identities are resolved, the result is the
[resolved model](./resolution.md#resolved-model). The
[processing order](./resolution.md#processing-and-validation-order) defines
the binding sequence.

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
claim protocol, required suite, or transition rule. The
[detailed conformance boundaries](./conformance.md#conformance-boundaries)
list each class's inputs and observable results, phase identifiers, defining
chapters, and identity/equality rules.

[Conformance](./conformance.md) contains the positive and negative examples
and their traceability records. These examples do not enable or establish a
conformance claim.

## Boundaries and deferred work

Revision `0.1` does not standardize implementation libraries or
implementation-specific fingerprints. Existing tool behavior is evidence, not
authority; behavior is normative only when this specification states it.

Non-goals and deferred work are listed in
[Conformance](./conformance.md#explicit-deferrals-and-non-goals).
Profile-specific exclusions are listed in
[profile output exclusions](./profiles.md#status-and-output-exclusions).
[Compatibility](./compatibility.md) defines the remaining compatibility
boundary.

Resolved decisions and selected future-version topics are recorded,
non-normatively, in
[Decision status and open decisions](./open-decisions.md).

## Read next

Read the following pages in order. Each is a short non-normative guide with
direct links to the exhaustive owners. Examples in these guides are standalone
excerpts unless explicitly marked as continuing.

1. [Configuration files](./configuration-files.md): root files, fragments,
   includes, containment, deterministic order, and composition.
2. [Project model](./project-model.md): project, distros, components, groups,
   defaults, inheritance, discovery, references, and profiles.
3. [Source material](./source-material.md): the common source workflow, with
   links to the exhaustive source reference.
4. [Transformations](./transformations.md): overlay purpose, order, atomicity,
   archive scope, and the 17 operations.
5. [Materialized dist-git](./materialized-dist-git.md): active spec, tree,
   source manifest, provenance, identity, and atomic publication.
6. [Optional workflows](./optional-workflows.md): RPM build, repositories,
   images, tests, publishing, and custom generation.
7. [Errors, determinism, and security](./errors-determinism-security.md):
   failure boundaries, explicit inputs, diagnostics, and safety rules.

During implementation, use [Reference](./reference.md) to find exhaustive
owners, including [archive transformation and hashing](./overlays.md#archive-extraction-and-batching),
the final [materialization provenance ledger](./artifacts.md#provenance-association),
[selected operation contracts](./profiles.md#selected-operation-contracts),
and [diagnostic records](./validation.md#diagnostic-record), plus matrices,
fixtures, exclusions, and deferred work.

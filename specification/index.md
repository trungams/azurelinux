# Distro TOML specification

This specification defines the observable process that converts source TOML
documents and declared upstream or local inputs into a build-ready materialized
dist-git tree. It defines portable data and behavior, not a particular command,
implementation language, cache, work directory, or storage layout.

## Requirement language

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHOULD**, **SHOULD NOT**,
and **MAY** are normative:

- **MUST**, **MUST NOT**, and **REQUIRED** state requirements for conformance.
- **SHOULD** and **SHOULD NOT** permit a deviation only when its consequences
  are understood and the implementation documents why the deviation is
  necessary.
- **MAY** states an optional behavior. An implementation that does not provide
  the behavior remains conforming.

Uncapitalized forms of these words are descriptive. Text explicitly labeled
**Open decision**, **Review note**, **Example**, or **Non-normative** does not
create a conformance requirement.

An **error** means that the affected document, resolved object, component, or
materialization attempt is non-conforming and MUST NOT produce a successful
result. This specification defines required diagnostic information where it
affects interoperability, but it does not standardize exact diagnostic wording.

## Document identity

The root document selects this provisional specification revision with:

```toml
spec-version = "0.1"
```

The complete document identity, encoding, key vocabulary, and fragment rules
are defined in [Document format](./document.md).

## Scope

The normative scope is the complete configuration-to-materialized-tree
pipeline:

1. Discover and parse the root source document.
2. Load its included fragments while retaining source and path provenance.
3. Compose the parsed documents into one composed model.
4. Apply defaults and inheritance and resolve references.
5. Resolve every upstream source selector to an immutable source identity.
6. Acquire declared upstream, local, and source-artifact inputs.
7. Apply declared transformations atomically and deterministically.
8. Emit the materialized, build-ready dist-git tree.

Later chapters define the inputs, outputs, validation requirements, and failure
conditions at each boundary. An implementation MUST NOT reorder stages when
that changes an observable result.

Final RPM and image reproducibility is outside this specification's core scope.
The build-ready dist-git tree is the final core artifact.

## Terminology

**Root document**
: The source TOML file selected as the entry point. It declares
  `spec-version` and establishes the project root.

**Included fragment**
: A TOML file loaded from an `includes` entry. It contributes configuration to
  the root document but does not select a specification version.

**Source document**
: The root document or an included fragment before cross-document composition.
  Comments, formatting, and physical key order are not semantic.

**Composed model**
: The typed result after all source documents have been loaded and combined by
  the composition rules, before defaults, inheritance, and references are
  resolved.

**Resolved model**
: The typed result after defaults, inheritance, references, and source
  selectors required by the applicable resolution stage have been resolved.
  The resolved model retains path provenance needed by later stages.

**Source selector**
: A value such as a branch, snapshot, or explicit commit that participates in
  choosing upstream content.

**Source identity**
: The immutable, exact identity selected for upstream content. A branch or
  snapshot alone is not a source identity.

**Materialized dist-git tree**
: The final build-ready filesystem tree after acquisition and transformation.
  Its paths, file contents, file types, executable modes, symlink targets, and
  transformed source hashes participate in conformance where specified.

**Declared operation sequence**
: The effective inline overlays followed by operations from resolved secondary
  overlay documents, in their specified order.

**Semantic archive result**
: The validated extracted archive entry tree after archive-scoped operations,
  independent of tar headers, compression encoding, and other nonsemantic
  archive metadata.

**Materialization provenance ledger**
: The semantic companion mapping each final tree entry to its source origin
  and ordered transformations. It is not a file inside the materialized tree.

**Profile**
: A named, independently optional group of requirements. A profile can add
  behavior but cannot weaken core requirements.

**Selected operation**
: One explicit implementation-interface request such as `rpm-build`,
  `image-build`, `test`, `publish-packages`, `publish-image`, or
  `custom-source-generate`. Profile data presence does not select an
  operation.

**Environmental-input record**
: The immutable operation input that binds every permitted architecture,
  evaluation-time, network, resource-limit, toolchain, package, and macro
  influence. Ambient host state is not an implicit record value.

**Diagnostic record**
: Structured error information containing a portable class, validation and
  processing phase, requirement identity, semantic subject, and related
  semantic locations. Exact human wording is not standardized.

## Conformance

A conformance class names an input boundary, required processing, and
observable output. Revision `0.1` uses the names only to describe boundaries.
The ordinary fixture contains Source-document and Loading/composed-model
examples; it does not contain executable Resolved-model or Materialized-tree
success cases. Revision `0.1` defines no class lifecycle, role protocol, suite
registry, or claim issuance mechanism.

| Conformance class | Specification revision | Status | Input boundary | Observable boundary | Required phase identifiers and chapters |
| --- | --- | --- | --- | --- | --- |
| **Source-document** | `0.1` | Not enabled or claimed | One UTF-8 root document or included fragment, its root/fragment role, canonical project root, canonical document target, and applicable field contracts | A validated source model or structured diagnostics | [`SD-BYTES`, `SD-CONTROLS`, and `SD-MODEL`](./resolution.md#processing-and-validation-order); [Document format](./document.md), [Path provenance](./loading.md#path-provenance), and [Validation](./validation.md) |
| **Loading/composed-model** | `0.1` | Not enabled or claimed | A project root and root document whose reached documents satisfy the source-document requirements | Ordered unique document evaluations, repeated-reach provenance, and one atomic composed model, or structured diagnostics | [`LD-ROOT`, `SD-BYTES`, `SD-CONTROLS`, `SD-MODEL`, `LD-INCLUDES`, `CM-COMPOSE`, and `CM-VALIDATE`](./resolution.md#processing-and-validation-order); [Document loading and composition](./loading.md), [Determinism](./determinism.md), and [Validation](./validation.md) |
| **Resolved-model** | `0.1` | Not enabled or claimed | A valid composed model, exact target distro/version input, environmental-input record, and declared resolution inputs | One behaviorally defined typed resolved model with relocatable provenance, or structured diagnostics | [`RM-TARGET`, `RM-EFFECTIVE`, `RM-REFERENCES`, `RM-VALIDATE`, and `RM-SOURCE-ID`](./resolution.md#processing-and-validation-order); [Resolution](./resolution.md), [Determinism](./determinism.md), and [Validation](./validation.md) |
| **Materialized-tree** | `0.1` | Not enabled or claimed | A valid resolved model, declared acquisition/transformation inputs, environmental-input record, security context, and resource limits | A build-ready materialized dist-git tree with exact behavioral identity and semantic provenance, or structured diagnostics | [`MT-MATERIALIZE`](./resolution.md#processing-and-validation-order); [Sources](./sources.md), [Overlay transformations](./overlays.md), [Materialized artifacts](./artifacts.md), [Determinism](./determinism.md), [Validation](./validation.md), and [Security](./security.md) |

The authoritative status is simple: no class is enabled or claimed. The table
does not reserve lifecycle states, roles, required suites, or transition
rules.

The Source-document location input derives a normalized UTF-8
project-relative source-document identity with `/` separators and no empty,
`.` or `..` segments plus its defining path base under
[Path provenance](./loading.md#path-provenance). These are operational
location inputs, not semantic output identity. Relocating the root and target
together without changing that relative identity does not change
source-model semantic equality.

Source documents are compared using
[TOML semantic equality](./document.md#semantic-equality). Composed models
compare typed values recursively using the same equality rules and compare
provenance using normalized project-relative identities, never absolute
checkout roots. Resolved models use the behavioral typed-object contract in
[Resolution](./resolution.md#resolved-model-behavioral-contract).
Materialized-tree identity uses exact paths, entry types, file bytes, modes,
and symlink targets under
[Materialized artifacts](./artifacts.md#behavioral-tree-identity). This
revision defines no canonical serialization, comparison algorithm, or
conformance tool for either boundary.

The strict ordinary example format, deterministic repetition, typed outcomes,
closed unions, and traceability rules are defined in
[Conformance examples and fixture format](./conformance.md). Those examples do
not enable or establish a claim.

Core processing includes component construction and hash-verifiable source
artifacts. Images, tests, publishing, executable custom-source generation, and
RPM build are named optional profiles. Profile data presence, validation,
preservation, operation selection, and unsupported-operation behavior are
defined in [Optional profile data and operations](./profiles.md).
Repository resources are shared optional RPM-build/image profile data; their
presence does not select either operation.

The class labels confer no claim. Deferred questions remain non-normative in
[Open decisions](./open-decisions.md).

## Non-goals

This specification does not standardize:

- command names or command-line interfaces;
- implementation language, libraries, cache layout, work layout, or temporary
  storage;
- lock-file formats or implementation-specific fingerprints;
- exact diagnostic wording;
- a future semantic component-transformation API;
- final RPM or image reproducibility; or
- a long-term version compatibility and deprecation policy;
- external-operation replay or publication simulation;
- portable custom-generator closure;
- package-manager internals or OpenPGP packet grammar; or
- a conformance suite, role, lifecycle, or claim protocol.

Existing tool behavior is evidence, not authority. A behavior is normative only
when this specification states it as a requirement.

## Processing model and chapters

- [Document format](./document.md) defines document identity, TOML syntax,
  encoding, and the strict key vocabulary.
- [Document loading and composition](./loading.md) defines root discovery,
  includes, path provenance, and composition.
- [Resolution](./resolution.md) defines the source, composed, and resolved
  models and their validation boundaries.
- [Source identity and acquisition](./sources.md) defines exact upstream
  commit identity, selector precedence, and source-artifact acquisition.
- [Overlay transformations](./overlays.md) defines the common atomic
  transformation pipeline, archive batching, and all 17 low-level operations.
- [Materialized dist-git artifacts](./artifacts.md) defines the final tree
  namespace, placement, byte comparison, provenance association, and atomic
  publication.
- [Determinism and environmental inputs](./determinism.md) classifies every
  permitted or prohibited ambient influence and defines explicit operation
  context.
- [Validation and diagnostics](./validation.md) defines phase boundaries,
  diagnostic records and classes, deterministic ordering, and rollback.
- [Security and authorization](./security.md) defines credential, URI,
  filesystem, archive, sandbox, script, and resource-limit requirements.
- [Conformance examples and fixture format](./conformance.md) defines strict
  ordinary positive/negative examples and traceability.
- [Top-level objects](./objects.md) defines the canonical root vocabulary,
  name-keyed maps, common contract notation, and profile activation.
- [Project](./project.md), [Distros](./distros.md),
  [Components](./components.md), and
  [Component groups](./component_groups.md) define the core object model.
- [Build and release](./build.md), [Resources](./resources.md),
  [Packages and publishing](./packages.md), [Images](./images.md), and
  [Tests](./tests.md) define profiled fields.
- [Optional profile data and operations](./profiles.md) defines shared
  validation, preservation, selection, and unsupported-operation behavior.
- [Open decisions](./open-decisions.md) records non-normative alternatives,
  consequences, evidence, and recommendations.
- [Excluded and deferred fields](./compatibility.md) records characterized
  tool-specific, deprecated, and intentionally excluded keys.

Environmental inputs, validation, security, operation selection, and ordinary
fixture schemas are defined by this revision. Repository resources are shared
optional RPM-build/image profile data, long-term version policy remains
deferred, and no conformance class is enabled or claimed. OD-6 and OD-7 are resolved:
tag cardinality is fixed per operation, archive output is the semantic
extracted-tree result with content-detected compression preserved, and emitted
archive bytes remain bound by the configured post-overlay hash without a
canonical encoder or cross-tool byte-reproducibility claim.

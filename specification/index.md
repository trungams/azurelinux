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
observable output. A **producer** computes that output from the class input. A
**consumer** accepts an output supplied through an implementation-defined
transport and validates every invariant required at that boundary. When a
class is enabled, a claim MUST state producer, consumer, or both.

| Conformance class | Specification revision | Lifecycle token | Available role | Required suite ID/version | Future input | Future observable output | Required phase identifiers and chapters | Enablement boundary |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| **Source-document** | `0.1` | `draft` | None | Absent | One UTF-8 root document or included fragment; its root/fragment role; a canonical project root and canonical document target used only to derive and validate relocatable semantic location; and every applicable field contract | A validated source model or an error with structured diagnostics | Future use of [`SD-BYTES`, `SD-CONTROLS`, and `SD-MODEL`](./resolution.md#processing-and-validation-order); [Document format](./document.md), [Path provenance](./loading.md#path-provenance), [Validation](./validation.md), and every object/profile chapter | No current claim. The key and field vocabulary and fixture format are closed, but no normative complete source-model fixture-suite version and output oracle are enabled. |
| **Loading/composed-model** | `0.1` | `draft` | None | Absent | A project root and root document whose individual documents satisfy the future Source-document class | The ordered unique document evaluations, repeated-reach provenance, and one atomic composed model, or an error with structured diagnostics | Future use of [`LD-ROOT`, `SD-BYTES`, `SD-CONTROLS`, `SD-MODEL`, `LD-INCLUDES`, `CM-COMPOSE`, and `CM-VALIDATE`](./resolution.md#processing-and-validation-order); future Source-document requirements plus [Document loading and composition](./loading.md), [Determinism](./determinism.md), and [Validation](./validation.md) | No current claim. Repeated-reach behavior is defined, but no normative complete composed-model fixture-suite version and output oracle are enabled. |
| **Resolved-model** | `0.1` | `draft` | None | Absent | A conforming composed model, explicit immutable target distro/version input, immutable environmental-input record, and all other declared resolution inputs | One behaviorally defined typed resolved model with relocatable provenance, or an error with structured diagnostics | Future use of [`RM-TARGET`, `RM-EFFECTIVE`, `RM-REFERENCES`, `RM-VALIDATE`, and `RM-SOURCE-ID`](./resolution.md#processing-and-validation-order); [Resolution](./resolution.md), [Determinism](./determinism.md), [Validation](./validation.md), and all owning field/source chapters | No current claim. Inheritance and typed behavioral identity are defined, but no normative complete resolved-model fixture-suite version and output oracle are enabled; no canonical serialization is required. |
| **Materialized-tree** | `0.1` | `draft` | None | Absent | A conforming resolved model, all declared acquisition/transformation inputs, immutable environmental-input record, security context, and resource limits | A build-ready materialized dist-git tree with exact behavioral identity and semantic provenance, or an error with structured diagnostics | Future use of [`MT-MATERIALIZE`](./resolution.md#processing-and-validation-order); [Sources](./sources.md), [Overlay transformations](./overlays.md), [Materialized artifacts](./artifacts.md), [Determinism](./determinism.md), [Validation](./validation.md), [Security](./security.md), and selected profile chapters | No current claim. S3-R4-001, S3-R4-002, and OD-7 affect exact bytes; no Materialized-tree suite is enabled. |

The lifecycle column uses only the closed tokens defined by
[Claim lifecycle](./conformance.md#claim-lifecycle). Required suite ID/version
is absent while a class is `draft`; no free-form status text or implied suite
membership enables a claim.

All four classes are forward-declared roadmap architecture and none is currently
claimable. An implementation MUST NOT name any of them in a conformance claim
under this revision. The future input and output columns make the intended
architecture reviewable; they do not accept an input, establish a complete
success/error oracle, or authorize a producer or consumer claim.
No undefined or later-layer requirement is excluded or waived.

An owning later chapter can explicitly enable a class only after closing its
complete input vocabulary, observable output oracle, and every requirement
owned by that boundary, and after changing the lifecycle token and required
suite fields in this table. The
absence of a currently defined later-layer requirement is not an exclusion and
MUST NOT be treated as permission to omit that requirement from a future
claim.

Before Source-document can be enabled, its canonical project root and canonical
document target MUST satisfy the [Path provenance](./loading.md#path-provenance)
contract as a contained canonical root/target pair. That pair must derive a
normalized UTF-8 project-relative source-document identity with `/` separators
and no empty, `.` or `..` segments, plus its defining path base. These are
operational location inputs, not semantic output identity. Relocating both
paths together without changing that relative identity does not change future
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

When a class is explicitly enabled, a conformance claim MUST identify:

- the supported specification version;
- every claimed conformance class;
- whether each class is claimed as producer, consumer, or both;
- whether it covers complete core processing or a named profile; and
- any **SHOULD** or **SHOULD NOT** deviation.

Fixture structure, producer/consumer separation, deterministic repetition,
profile scope, reports, blocker handling, and the claim lifecycle are defined
in [Conformance methodology](./conformance.md).

When the applicable classes are enabled, core conformance includes component
construction and hash-verifiable source artifacts. Images, tests, publishing,
and executable custom-source generation are named optional profiles.
RPM-build is also a named profile. Profile data presence, validation,
preservation, operation selection, and unsupported-operation behavior are
defined in [Optional profile data and operations](./profiles.md).
Repository resources are shared optional RPM-build/image profile data; their
presence does not select either operation.

The forward declarations confer no claim, and undefined future requirements are
not exclusions. The questions and alternatives themselves remain non-normative
in [Open decisions](./open-decisions.md).

## Non-goals

This specification does not standardize:

- command names or command-line interfaces;
- implementation language, libraries, cache layout, work layout, or temporary
  storage;
- lock-file formats or implementation-specific fingerprints;
- exact diagnostic wording;
- a future semantic component-transformation API;
- final RPM or image reproducibility; or
- a long-term version compatibility and deprecation policy.

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
- [Conformance methodology](./conformance.md) defines fixture manifests,
  positive/negative/resolved/output expectations, profile claim scope, and
  claim lifecycle.
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

Environmental inputs, validation, security, operation selection, and
conformance methodology are defined by this revision. Repository resources are
shared optional RPM-build/image profile data, long-term version policy remains
deferred, and no conformance class is enabled. S3-R4-001 and S3-R4-002 remain
visible capped blockers. OD-6 and OD-7 keep disputed singleton-tag cardinality
and canonical transformed-archive encoding visible without permitting
incompatible successful output.

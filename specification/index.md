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

**Profile**
: A named, independently optional group of requirements. A profile can add
  behavior but cannot weaken core requirements.

## Conformance

A conformance class names an input boundary, required processing, and
observable output. A **producer** computes that output from the class input. A
**consumer** accepts an output supplied through an implementation-defined
transport and validates every invariant required at that boundary. When a
class is enabled, a claim MUST state producer, consumer, or both.

| Conformance class | Status and available role | Future input | Future observable output | Required phase identifiers and chapters | Enablement boundary |
| --- | --- | --- | --- | --- | --- |
| **Source-document** | Forward-declared; no current producer or consumer claim | One UTF-8 root document or included fragment; its root/fragment role; a canonical project root and canonical document target used only to derive and validate relocatable semantic location; and every applicable field contract | A validated source model or an error | Future use of [`SD-BYTES`, `SD-CONTROLS`, and `SD-MODEL`](./resolution.md#processing-and-validation-order); [Document format](./document.md), [Path provenance](./loading.md#path-provenance), and the source-model sections of [Resolution](./resolution.md) | No current claim. Slice 2 must close the complete object-key vocabulary and source-local field contracts before this class can be enabled. |
| **Loading/composed-model** | Forward-declared; no current producer or consumer claim | A project root and root document whose individual documents satisfy the future Source-document class | The ordered document reaches and one atomic composed model, or an error | Future use of [`LD-ROOT`, `SD-BYTES`, `SD-CONTROLS`, `SD-MODEL`, `LD-INCLUDES`, `CM-COMPOSE`, and `CM-VALIDATE`](./resolution.md#processing-and-validation-order); future Source-document requirements plus [Document loading and composition](./loading.md) | No current claim. Slice 2 must close the complete source and composed-model vocabularies and output oracle; repeated canonical-document reaches remain subject to OD-1. |
| **Resolved-model** | Forward-declared; no current producer or consumer claim | A conforming composed model and all declared resolution inputs | One semantically comparable typed resolved model with relocatable provenance, or an error | Future use of [`RM-EFFECTIVE`, `RM-REFERENCES`, `RM-VALIDATE`, and `RM-SOURCE-ID`](./resolution.md#processing-and-validation-order); [Resolution](./resolution.md) and all owning field and source chapters | No current claim. Future enabling is subject to the Resolution-owned [future resolved-model claim gate](./resolution.md#future-resolved-model-claim-gate); OD-4 concerns the additional byte-serialization contract. |
| **Materialized-tree** | Forward-declared; no current producer or consumer claim | A conforming resolved model and all declared acquisition and transformation inputs | A build-ready materialized dist-git tree or an error | Future use of [`MT-MATERIALIZE`](./resolution.md#processing-and-validation-order) and all owning source, overlay, artifact, validation, determinism, and profile chapters | No current claim. No undefined or later-layer requirement is excluded or waived. |

All four classes are forward-declared roadmap architecture and none is currently
claimable. An implementation MUST NOT name any of them in a conformance claim
under this revision. The future input and output columns make the intended
architecture reviewable; they do not accept an input, establish a complete
success/error oracle, or authorize a producer or consumer claim.

An owning later chapter can explicitly enable a class only after closing its
complete input vocabulary, observable output oracle, and every requirement
owned by that boundary, and after changing the status in this table. The
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

The additional OD-2/OD-3 precondition for eventually enabling Resolved-model is
owned by the
[future resolved-model claim gate](./resolution.md#future-resolved-model-claim-gate).
The conformance lifecycle does not weaken or duplicate that rule. OD-4 concerns
the additional byte-serialization contract.

Source documents are compared using
[TOML semantic equality](./document.md#semantic-equality). Composed models
compare typed values recursively using the same equality rules and compare
provenance using normalized project-relative identities, never absolute
checkout roots. When the Resolved-model class is enabled, its future semantic
contract will apply those principles together with its effective-value,
identity, provenance, and ordering requirements. Materialized-tree equality is
byte-oriented and will be completed by the artifact contract before that class
can be enabled.

When a class is explicitly enabled, a conformance claim MUST identify:

- the supported specification version;
- every claimed conformance class;
- whether each class is claimed as producer, consumer, or both;
- whether it covers complete core processing or a named profile; and
- any **SHOULD** or **SHOULD NOT** deviation.

When the applicable classes are enabled, core conformance includes component
construction. Images, tests, publishing, and executable custom-source
generation are candidates for named profiles; the final assignment of current
fields to core or profiles remains an
[explicit open decision](./open-decisions.md#od-5-final-core-and-profile-assignment).

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
- [Top-level objects](./objects.md) introduces the object families without
  completing their field contracts.
- [Open decisions](./open-decisions.md) records non-normative alternatives,
  consequences, evidence, and recommendations.

The object, source, overlay, artifact, validation, determinism, profile, and
conformance chapters are completed in later specification slices.

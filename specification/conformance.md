[Return to index](./index.md)

# Conformance examples and fixture format

This chapter defines a small strict format for ordinary positive and negative
examples. It also defines deterministic execution guidance and normative
traceability for those examples.

The format is not a conformance claim system. It defines no suite registry,
suite membership, producer or consumer role, class lifecycle, claim issuance,
claim invalidation protocol, external-operation replay, or publication
simulation.

## Current status

No conformance class is enabled or claimed in specification revision `0.1`.
The class names remain useful labels for specification boundaries:

| Class | Status | Specification boundary |
| --- | --- | --- |
| `Source-document` | Not enabled or claimed | One root or fragment document and its strict source-model validation result. |
| `Loading/composed-model` | Not enabled or claimed | Root discovery, include evaluation, and atomic composition. |
| `Resolved-model` | Not enabled or claimed | Typed effective values, identities, references, ordering, and semantic provenance. |
| `Materialized-tree` | Not enabled or claimed | The exact behavioral tree identity and semantic provenance produced by core materialization. |

These status values are authoritative facts, not lifecycle states. A later
specification revision may define a separate conformance system, but revision
`0.1` defines no transition or claim protocol.

The retained ordinary fixture contains Source-document and
Loading/composed-model examples only. It does not provide an executable
Resolved-model or Materialized-tree success case. The latter two names remain
specification boundaries whose contracts are normative even though this
fixture does not exercise them.

## Fixture package

A fixture package is a project-contained directory with one `manifest.toml`
using format version `1`. Every referenced file is beneath that directory and
is named by a portable relative model path.

The manifest root is closed and contains exactly:

| Field | Requirement |
| --- | --- |
| `format-version` | Integer `1`. |
| `fixture-id` | Stable non-empty ASCII token using lowercase letters, digits, and `-`. |
| `spec-version` | Exact specification version under test. |
| `description` | Non-normative UTF-8 text. |
| `inputs` | Ordered non-empty array of closed path/digest records. |
| `cases` | Ordered non-empty array of the closed case union below. |

An input record contains exactly `path` and `sha256`. `path` satisfies the
portable relative model-path grammar and is unique within the manifest.
`sha256` is the lowercase 64-digit SHA-256 of the complete referenced file.
The runner verifies every digest before processing any case. Symlinks, special
files, path escapes, missing files, undeclared consumed files, unconsumed
declared files, and digest mismatches are `conformance` errors.

Every fixture-file reference has the exact form `input:<path>`, where `<path>`
names one root `inputs` record. A runner MUST NOT infer an input from a
filename, directory convention, prior case, cache, or implementation-private
transport.

## Closed case union

Every case has these exact common keys:

- `id`: unique fixture token within the manifest;
- `kind`: exactly `positive` or `negative`;
- `class`: exactly `Source-document` or `Loading/composed-model`;
- `case-input`: exactly one class-discriminated input table;
- `expected-outcome`: exactly `success` or `error`; and
- exactly one expectation array selected by `kind`.

The complete root-key union is:

| `kind` | Required keys in addition to the common keys | Required outcome | Permitted class |
| --- | --- | --- | --- |
| `positive` | `expected-facts` | `success` | `Source-document` or `Loading/composed-model` |
| `negative` | `expected-diagnostics` | `error` | `Source-document` or `Loading/composed-model` |

No case key is optional. A missing required key, an extra key, two expectation
alternatives, an expectation table from another kind, an unknown kind, a wrong
fixture class, or a wrong TOML type is a `conformance` error.

### Class-discriminated input

`case-input.class` MUST equal the containing case `class`. The two exact
variants are:

| `class` | Exact remaining keys |
| --- | --- |
| `Source-document` | `document-role`, `project-relative-path`, `document-bytes` |
| `Loading/composed-model` | `project-root-id`, `root-document` |

`document-role` is exactly `root` or `fragment`.
`project-relative-path` satisfies the portable relative model-path grammar;
the root role requires `azldev.toml`, and a fragment forbids that path.
`project-root-id` uses the fixture-ID grammar. Every document field is an
`input:<path>` reference.

Unknown keys, keys from another class variant, a mismatched class
discriminator, a missing required field, or a wrong type are errors. These
fixture inputs are sufficient for the retained ordinary examples. They do not
define a canonical composed-model or resolved-model serialization.

### Positive fact records

`expected-facts` is a non-empty array. Each record contains exactly:

- `requirement`: one current specification `chapter.md#anchor`;
- `subject`: one non-empty semantic subject; and
- `fact`: one non-empty expected semantic fact.

The runner checks the named successful phase boundary and the stated fact. It
does not infer unlisted output properties from the absence of diagnostics.

### Negative diagnostic records

`expected-diagnostics` is a non-empty array. Each record contains exactly:

- `class`;
- `validation-phase`;
- `processing-phase`;
- `requirement`; and
- `subject`.

The diagnostic class and validation phase come from
[Validation and diagnostics](./validation.md). Exact message text, localized
text, absolute paths, stack traces, timestamps, and secret values are
forbidden expectations. The failed boundary produces no successful result.

## Execution guidance

A fixture runner:

1. parses the closed manifest and every selected nested union before executing
   a case;
2. verifies all declared file digests and exact input consumption;
3. binds only the declared class input;
4. executes the stated processing boundary without undeclared host input;
5. repeats the case from at least two distinct absolute checkout, temporary,
   and cache paths;
6. applies any ambient-state variations described by the ordinary determinism
   fixture; and
7. compares the exact typed success facts or diagnostic outcome selected by
   the case kind.

Representative validators and single-implementation semantic helpers are
supporting evidence only. They do not enable or establish a conformance claim.

## Normative traceability

Every `requirement` in a retained fixture MUST identify an existing normative
chapter anchor or stable matrix row. A fixture cannot replace, weaken, or add a
source-document field contract. The complete 508-path schema disposition, 52
opaque framework-field dispositions, 17 overlay operation contracts, and
environmental-input classifications remain independently validated by their
own matrices and aggregate gates.

## Explicit deferrals and non-goals

Revision `0.1` does not define or require:

- canonical composed-model or resolved-model serialization;
- a semantic comparison algorithm, comparison stream, or conformance tool;
- an external-operation transcript, replay mode, deterministic adapter, or
  publication simulation;
- a suite registry, suite membership/version/digest protocol, producer or
  consumer role protocol, class lifecycle, claim issuance, or claim report
  invalidation protocol;
- a portable custom-generator execution root, installed-package payload
  closure, kernel/CPU/clock/randomness proof, or reproducible sandbox;
- package-manager internals, an OpenPGP packet grammar, accepted packet
  encoding policy, or cryptographic packet vectors;
- a canonical archive encoder or cross-tool archive-byte reproducibility;
- final RPM/SRPM or image reproducibility;
- long-term version policy; or
- future semantic overlays.

The tracked [ordinary fixture example](./examples/conformance/README.md)
demonstrates both retained case kinds and their executable closed unions.

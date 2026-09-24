[Return to index](./index.md)

# Resolution

Resolution converts the composed model into the effective typed model consumed
by source acquisition and materialization. This chapter defines phase
boundaries without prematurely defining object-field contracts or the still
open inheritance and group-precedence rules.

## Resolution models

### Source model

A **source model** is the forward-declared typed representation of one parsed
root document or included fragment after document-control and source-model
validation. It retains:

- TOML value types;
- the exact parsed spelling of every object key whose field contract is not yet
  defined;
- semantic source-document identity;
- semantic defining path bases; and
- value-level provenance.

A source model can be partial. Required values that may be provided by another
document are not required to appear in every fragment. For every field whose
owning contract is defined, each present value has nevertheless passed the
source-local key, type, element-type, nested-shape, and local-constraint checks
defined by that contract.

Retaining the exact spelling and TOML type of a key whose owning contract is
not yet defined is preparatory representation behavior only. It does not make
the key known, satisfy `SD-MODEL`, accept the input for conformance, or weaken
the strict unknown-key policy that applies when Slice 2 closes the object
vocabulary. Source-document and Loading/composed-model therefore remain
non-claimable in this revision.

### Composed model

The **composed model** is the atomic output of
[composition](./loading.md#composition-sequence). It contains the combined
configuration and leaf provenance, but it has not yet applied effective-value
defaults or inheritance and has not resolved references or source selectors.

### Resolved model

The **resolved model** is the typed, self-consistent result after:

- effective values have been selected from direct values, defaults, and
  inheritance;
- all required named references have been resolved;
- all derived values required for core processing have been computed;
- every upstream source has an exact, complete commit identity; and
- all validation required before acquisition has succeeded.

A branch, snapshot, tag, or abbreviated commit is not an immutable final source
identity. Detailed source fields and selector precedence are defined in the
source and object chapters completed in Slice 2.

Resolved-model is forward-declared and is not currently claimable. Requirements
in this chapter define prerequisites for its future enabling; they do not
authorize a producer or consumer conformance claim before all owning chapters
are complete and the class status in [Conformance](./index.md#conformance) is
explicitly changed.

The resolved model MUST retain enough provenance to identify the defining
source document and path base of every path-valued effective value using the
relocatable semantic identities defined in
[Path provenance](./loading.md#path-provenance).

## Processing and validation order

The identifiers in this section are the stable namespace for phase references.
They describe dependencies and observable boundaries, not one flat loop that
runs each phase exactly once.

| Phase ID | Processing or validation phase |
| --- | --- |
| `LD-ROOT` | **Root discovery.** Select the exact root name, canonicalize its path, establish the project root, and create the first newly reached node. |
| `SD-BYTES` | **Byte and TOML validation.** Decode UTF-8 and parse one newly reached source document. |
| `SD-CONTROLS` | **Document-control validation.** Validate root identity, fragment version rules, and document-control value types for that node's expected role. |
| `SD-MODEL` | **Source-model validation.** Independently validate every present value against each defined local key, TOML type, array-element type, nested table or array-of-tables shape, and source-local constraint. Requiredness, defaults, effective values, and cross-document constraints are not validated here. |
| `LD-INCLUDES` | **Include expansion and reach classification.** Resolve include entries, require typed successful expansion, enforce containment, order matches, canonicalize reached targets, and classify active-chain cycles or previous reaches. |
| `CM-COMPOSE` | **Composition.** Apply the ordered type-based composition rules atomically after loading has produced the complete ordered sequence. |
| `CM-VALIDATE` | **Composed-model validation.** Validate requiredness, combined top-level shape, and cross-document constraints that do not depend on effective-value selection. Source-local checks are not deferred here. |
| `RM-EFFECTIVE` | **Effective-value resolution.** Apply field defaults and inheritance under the governing field contracts. |
| `RM-REFERENCES` | **Reference resolution.** Resolve named object references and reject missing, ambiguous, or wrong-kind targets. |
| `RM-VALIDATE` | **Resolved-value validation.** Validate invariants that depend on effective values or resolved references. |
| `RM-SOURCE-ID` | **Source-identity resolution.** Convert upstream selectors to exact, complete commit identities and validate the result. |
| `MT-MATERIALIZE` | **Acquisition and materialization.** Acquire declared inputs, apply transformations, and emit the materialized dist-git tree under later chapters. |

Processing begins with `LD-ROOT`; its newly reached root then passes through
`SD-BYTES`, `SD-CONTROLS`, and `SD-MODEL`. After those three phases succeed,
`LD-INCLUDES` processes that document's include entries. For each expanded
candidate, `LD-INCLUDES` canonicalizes the target and classifies active-chain
cycle or previously reached status before the target can enter any `SD-*`
phase. Only a newly reached canonical node enters `SD-BYTES`,
`SD-CONTROLS`, and `SD-MODEL`, after which its own `LD-INCLUDES` work proceeds
depth-first.

After loading has constructed the complete ordered reach sequence,
`CM-COMPOSE` and then `CM-VALIDATE` run once for that sequence. The `RM-*`
phases follow in their table order only after composed-model validation
succeeds. `MT-MATERIALIZE` follows only after all enabled resolved-model
requirements succeed.

A processor MUST NOT use a later phase to silently repair an error from an
earlier phase. It MAY accumulate multiple diagnostics within one phase if no
invalid partial result is consumed by a later phase.

## Defaults and inheritance boundary

Defaults and inheritance operate on the composed model, never on individual
file text. They MUST NOT change source-document loading order or retroactively
alter composition.

A field default can be applied only when the field's owning contract defines
the default and its interaction with explicit empty or zero values. Absence,
an empty string, `false`, `0`, an empty array, and an empty table are distinct
states unless that field contract states otherwise.

### Future resolved-model claim gate

Resolved-model is currently non-claimable for every input, independently of
OD-2 and OD-3. The following additional rule is a preparatory precondition for
any later revision that enables the class:

Before Resolved-model claims can be enabled, an input MUST be excluded whenever
unresolved candidate providers overlap at one effective field path, even if
their typed values are semantically equal, because the complete resolved result
includes the typed value, supplier provenance, effective-object identity, and
ordering.

Candidate providers include every applicable inheritance layer and component
group. Supplier provenance includes the source document and field, or the
specification rule for a synthesized default. Ordering includes arrays,
operation sequences, and any other ordered resolved data. This conservative
gate applies before asking which provider would win; disjoint contributions do
not overlap merely because they participate in the same object.

This precondition does not make a source or composed model invalid and does not
authorize a current Resolved-model claim. It prevents a future claim from
silently ignoring semantic differences caused by unresolved precedence.

**Open questions (non-normative):** The alternatives remain in
[inheritance layer precedence](./open-decisions.md#od-2-inheritance-layer-precedence)
and [multiple component groups](./open-decisions.md#od-3-multiple-component-groups).
This revision does not select either result.

## Reference-resolution boundary

Reference resolution begins only after the effective values needed to identify
references have been selected. It MUST NOT choose among conflicting inherited
values as a side effect of lookup.

Each reference contract MUST eventually define:

- the source field and target object kind;
- whether the reference is required or optional;
- name comparison and namespace;
- missing-target and duplicate-target behavior; and
- whether cycles are permitted.

Those object-specific contracts belong to Slice 2. A processor MUST report a
missing, ambiguous, wrong-kind, or forbidden cyclic reference as an error.

## Source-identity boundary

Source-identity resolution consumes a reference-valid effective model. For
every upstream component, it MUST produce a full exact commit identity before
source acquisition or transformation can begin.

Selectors such as branches and snapshots MAY be inputs to this phase but MUST
NOT remain as the only identity in the resolved model. Source-controlled exact
commit values are ordinary TOML inputs, not a standardized lock-file format.

Network access, service choice, selector precedence, and verification rules are
defined with the source contracts in Slice 2. A failure to select or verify an
immutable identity is an error and produces no resolved model.

## Provenance through resolution

When a direct composed value becomes the effective value, its original
semantic source-document identity and defining path base remain attached.

When a value is inherited, the resolved value MUST identify both:

- the source document and field that supplied the value; and
- the object for which the value became effective.

When a default is synthesized by the specification, provenance MUST identify
the governing specification rule rather than falsely attributing the value to
a source document.

When a reference is resolved, the referring value's provenance and the target
object's identity MUST both remain available for diagnostics and canonical
resolved-model output.

When the Resolved-model class is enabled, its future semantic equality contract
MUST compare source provenance using the project-relative source-document
identity and defining path base. Operational absolute paths MAY appear in
diagnostics or implementation state but MUST be ignored for that semantic
equality. Moving an otherwise identical project tree to a different absolute
checkout root therefore will not change its semantic resolved model.

## Canonical resolved representation

This section defines a future semantic contract, not current claim permission.
When the Resolved-model class is explicitly enabled, conforming implementations
MUST agree on typed values, effective object identity, array order, exact source
identities, and required provenance.

Under this revision, an implementation MUST NOT claim either semantic
Resolved-model conformance or conformance to a canonical resolved-model byte
stream. The class remains forward-declared until all owning chapters explicitly
enable it.

**Open question (non-normative):** The byte format alternatives remain in
[canonical resolved-model serialization](./open-decisions.md#od-4-canonical-resolved-model-serialization).
This question concerns the additional serialization contract only; it does not
weaken the future semantic provenance and value equality contract.

Materialized-tree byte equivalence is a separate later-layer requirement and is
also non-claimable until its owning chapters explicitly enable that class.

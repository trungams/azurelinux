[Return to index](./index.md)

# Resolution

Resolution converts the composed model into the effective typed model consumed
by source acquisition and materialization. Object-field contracts are defined
by their owning chapters; this chapter defines the inheritance and
group-provider rules.

## Resolution models

### Source model

A **source model** is the typed representation of one parsed
root document or included fragment after document-control and source-model
validation. It retains:

- TOML value types;
- the canonical spelling of every known object key;
- semantic source-document identity;
- semantic defining path bases; and
- value-level provenance.

A source model can be partial. Required values that may be provided by another
document are not required to appear in every fragment. For every field whose
owning contract is defined, each present value has nevertheless passed the
source-local key, type, element-type, nested-shape, and local-constraint checks
defined by that contract.

Every key is validated against the closed vocabulary during `SD-MODEL`;
unknown or noncanonical keys are errors. Source-document and
Loading/composed-model remain non-claimable because their output-oracle and
conformance packages are not complete, not because the vocabulary is open.
Retention of typed values and exact key spelling is
preparatory representation behavior only. Source-document and
Loading/composed-model therefore remain non-claimable at the vocabulary
boundary.

Source-document and Loading/composed-model therefore remain
non-claimable until their owning contracts close the complete output oracle
and explicitly enable each class.

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
identity. Detailed source fields and selector precedence are defined in
[Source identity and acquisition](./sources.md).

Resolved-model is forward-declared and is not currently claimable. Requirements
in this chapter define prerequisites for its future enabling; they do not
authorize a producer or consumer conformance claim before all owning chapters
are complete and the class status in [Conformance](./index.md#conformance) is
explicitly changed.

The resolved model MUST retain enough provenance to identify the defining
source document and path base of every path-valued effective value using the
relocatable semantic identities defined in
[Path provenance](./loading.md#path-provenance).

### Target distro/version evaluation input

Every component-resolution evaluation has one required **target
distro/version input**: an ordered pair of exact names
`(target-distro, target-version)` supplied by the operation requesting the
resolved model. It is evaluation input, not a TOML field, inherited value,
ambient host default, or source selector. Both names use exact Unicode-scalar
comparison and MUST identify one declared
`distros.<target-distro>.versions.<target-version>` object.

The target pair is immutable for the complete evaluation. It alone selects
the distro-version `default-component-config` provider used for every
component resolved in that evaluation. A missing pair, missing distro,
missing version, or ambiguous target is an `RM-TARGET` error and produces no
effective component.

`spec.upstream-distro` identifies an upstream source repository context.
`project.default-distro` is only the whole-table fallback for that source
reference. Neither value selects, defaults, changes, or reselects the target
distro/version provider, including when it is inherited from the selected
provider itself. The target and source distro/version pairs MAY be equal, but
equality has no additional semantics and MUST NOT be inferred when either pair
is absent.

This bootstrap rule fixes the applicable distro provider before inheritance.
The selected provider participates at the lowest precedence defined in
[Component inheritance order](#component-inheritance-order).

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
| `RM-TARGET` | **Target selection.** Resolve the required immutable target distro/version evaluation input to exactly one declared distro version and select its default component provider. |
| `RM-EFFECTIVE` | **Effective-value resolution.** Reconcile discovery, construct the component-group layer, compose inheritance layers from low to high precedence with typed field rules, and apply field defaults. |
| `RM-REFERENCES` | **Reference resolution.** Resolve named object references and reject missing, ambiguous, or wrong-kind targets. |
| `RM-VALIDATE` | **Resolved-value validation.** Validate invariants that depend on effective values or resolved references. |
| `RM-SOURCE-ID` | **Source-identity resolution.** Convert upstream selectors to exact, complete commit identities and validate the result. |
| `MT-MATERIALIZE` | **Acquisition and materialization.** Acquire declared inputs, assemble private candidate state, apply the [atomic overlay pipeline](./overlays.md#common-transformation-pipeline), validate the [artifact contract](./artifacts.md), and publish one complete materialized dist-git tree or an error. |

Processing begins with `LD-ROOT`; its newly reached root then passes through
`SD-BYTES`, `SD-CONTROLS`, and `SD-MODEL`. After those three phases succeed,
`LD-INCLUDES` processes that document's include entries. For each expanded
candidate, `LD-INCLUDES` canonicalizes the target and classifies active-chain
cycle or previously reached status before the target can enter any `SD-*`
phase. Only a newly reached canonical node enters `SD-BYTES`,
`SD-CONTROLS`, and `SD-MODEL`, after which its own `LD-INCLUDES` work proceeds
depth-first.

After loading has constructed the complete ordered sequence of unique
source-model evaluations and all repeated-reach provenance,
`CM-COMPOSE` and then `CM-VALIDATE` run once for that sequence. `RM-TARGET`
then fixes the immutable target and selected distro-version provider before
`RM-EFFECTIVE` can inspect any inherited component value. The remaining
`RM-*` phases follow in table order. `MT-MATERIALIZE` follows only after all
enabled resolved-model requirements succeed. It produces no partial result:
archive groups, non-archive transformations, final manifest rewriting,
behavioral-identity validation, and publication are one atomic component
attempt.

A processor MUST NOT use a later phase to silently repair an error from an
earlier phase. It MAY accumulate multiple diagnostics within one phase if no
invalid partial result is consumed by a later phase.

The cross-phase validation groups, structured diagnostics, deterministic
ordering, and rollback rules are defined in
[Validation and diagnostics](./validation.md). Before any selected profile
side effect or materialization acquisition, `V-OPERATION` binds the explicit
[environmental-input record](./determinism.md#environmental-input-record),
security context, and resource limits. This preflight does not add a new model
default or change the `SD-*` through `MT-MATERIALIZE` dependency order.

## Defaults and inheritance boundary

Defaults and inheritance operate on the composed model, never on individual
file text. They MUST NOT change source-document loading order or retroactively
alter composition.

A field default can be applied only when the field's owning contract defines
the default and its interaction with explicit empty or zero values. Absence,
an empty string, `false`, `0`, an empty array, and an empty table are distinct
states unless that field contract states otherwise.

### Component inheritance order

`RM-EFFECTIVE` applies component configuration in this fixed order from low to
high precedence:

1. the selected distro-version `default-component-config`;
2. the top-level project `default-component-config`;
3. the aggregate of every applicable component-group
   `default-component-config`; and
4. the direct composed `components.<name>` configuration.

Source-document composition has already reduced each provider to one typed
partial `ComponentConfig`. Discovery and explicit-component reconciliation
happens before inheritance and yields one direct component plus its complete
group membership set. No contributed value can change the target provider,
direct component identity, or group membership set during this evaluation.

The component-group aggregate has no winning group and no priority field. Group
names are sorted by unsigned UTF-8 bytes only to define the order of
append-composed array elements; that ordering MUST NOT choose a scalar,
replacing array, or same-key map value. For the group layer:

- an append-composed array may be contributed by several groups and is
  concatenated in sorted group-name order while preserving each group's
  element order;
- recursively present table or map leaves from several groups may be united
  only when their effective leaf paths are disjoint; and
- two or more groups contributing the same non-append effective leaf is a
  destructive overlap and an `RM-EFFECTIVE` error, even when the typed values
  are equal.

A scalar is one leaf. A replacing array, including an explicit empty array, is
one leaf at the array field path. A map or table contributes its recursively
present leaves; different map keys and different child fields are disjoint.
Mere empty-table presence contributes no leaf. Revision `0.1` defines neither
lexical-name precedence nor an explicit group-priority field.

After the group aggregate is valid, the four layers compose in the fixed order
using the same typed rules as document composition plus each field's explicit
exception. Later scalars replace earlier scalars, tables and maps compose
recursively by key, replacing arrays replace the earlier array, and
append-composed arrays concatenate in layer order. Thus a direct component may
replace a project or group scalar, while two groups cannot use ordering to
resolve that same scalar.

Specification defaults are applied only after all four layers compose and only
where the resulting field is absent. Replaced values retain the winning
provider's provenance. Map leaves and appended array elements retain their own
supplier provenance, including the exact contributing group where applicable.
The fixed package-publishing order is a separate profile-specific contract.

The [inheritance and group fixture](./examples/inheritance-overlap/README.md)
covers layer precedence, append composition, disjoint group leaves, and
destructive group overlap. The
[target distro fixture](./examples/target-distro/README.md) demonstrates that
source references cannot select or reselect the distro-version provider.

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

Those object-specific contracts are defined in the owning object/profile
chapters. A processor MUST report a
missing, ambiguous, wrong-kind, or forbidden cyclic reference as an error.

## Source-identity boundary

Source-identity resolution consumes a reference-valid effective model. For
every upstream component, it MUST produce a full exact commit identity before
source acquisition or transformation can begin.

Selectors such as branches and snapshots MAY be inputs to this phase but MUST
NOT remain as the only identity in the resolved model. Source-controlled exact
commit values are ordinary TOML inputs, not a standardized lock-file format.

Selector precedence, portable URI requirements, and verification rules are
defined in [Sources](./sources.md). A failure to select or verify an
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
object's identity MUST both remain available for diagnostics and resolved-model
inspection.

Operational absolute paths MAY appear in diagnostics or implementation state
but MUST NOT affect resolved-model behavior. Moving an otherwise identical
project tree to a different absolute checkout root therefore does not change
its resolved model.

## Resolved-model behavioral contract

The resolved-model contract is the typed object graph defined by this chapter
and the owning field chapters. Conforming implementations MUST agree on:

- each field's TOML type and effective value;
- effective object and resolved reference identities;
- ordered-array and append-composed sequence order;
- exact immutable source identities; and
- required semantic provenance, using project-relative source-document
  identities and defining path bases.

Revision `0.1` does not standardize resolved-model JSON, TOML, another byte
serialization, a semantic comparison algorithm, or a conformance tool.
Implementations MAY use any internal representation and comparison method that
preserves the behavioral contract above. Fixture metadata may select typed
leaves and provenance for review without becoming a canonical encoding.

The Resolved-model class remains forward-declared and non-claimable because no
complete normative fixture suite and output oracle is enabled. That lifecycle
status does not leave inheritance, typed values, or behavioral identity
undefined.

The materialized-tree behavioral identity is defined in
[Materialized artifacts](./artifacts.md#behavioral-tree-identity). That class
also remains non-claimable; S3-R4-001, S3-R4-002, and OD-7 still affect exact
active-spec or transformed-archive output.

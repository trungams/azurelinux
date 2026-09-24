[Return to index](./index.md)

# Open decisions

> **Non-normative review material.** This chapter records alternatives,
> observable consequences, evidence, and recommendations. It does not define
> `MUST`, `SHOULD`, or `MAY` requirements, even when those words appear while
> describing another chapter. Normative chapters link here to make an
> unresolved product decision visible.

## OD-1: Repeated canonical-document evaluation

**Question:** When one canonical fragment is reached a second or later time
outside the active recursion chain, does its body contribute once by canonical
identity or once per reach?

**Alternative A - once by canonical identity**

- Prevents duplicate contribution from an accidentally repeated include.
- Makes results depend on the first traversal position unless the graph is
  linearized independently of traversal.
- Requires a rule for which incoming edge supplies diagnostics and traversal
  context.

**Alternative B - once per reach**

- Resembles textual inclusion and makes every declared or expanded reach
  observable.
- Can duplicate append-composed operation sequences.
- Makes adding a second path to a shared defaults fragment change results even
  when the fragment itself is unchanged.

**Evidence:** The characterized azldev loader rejects a second visit when the
lexically normalized absolute-path string is the same, including duplicate
literals, overlapping matches that produce the same spelling, and non-cyclic
multiple-parent sharing. It does not canonicalize symbolic links before using
that string as identity, so rejection of two symlink aliases depends on their
path spellings and is not guaranteed. The baseline draft recurses per include
entry but does not identify the repeated-reach category.

**Recommendation:** Prefer once-by-canonical-identity evaluation, with the
node's contribution placed at its first deterministic traversal position and
later incoming edges recorded only as provenance. This minimizes accidental
duplicate operations. A fixture must still test whether first-position behavior
is acceptable before this becomes normative.

## OD-2: Inheritance layer precedence

**Question:** How do distro defaults, project defaults, component-group
defaults, and direct component values produce one effective component?

**Alternative A - fixed layer order with structural merge**

- Gives every layer a documented precedence.
- Supports partial defaults.
- Requires field-specific exceptions for append-composed sequences and clear
  operations.

**Alternative B - first-present field lookup**

- Is easy to explain for scalars.
- Cannot naturally compose nested tables or ordered operations.
- Matches the baseline prose but not the characterized implementation.

**Alternative C - reject overlapping definitions across layers**

- Avoids hidden precedence.
- Makes reusable partial defaults substantially less useful.

**Evidence:** For explicitly configured memberships, current azldev structurally
merges distro defaults, project defaults, lexically ordered group defaults, and
direct component values. A component discovered by multiple groups can instead
receive map-iteration-dependent effective config, as recorded in
[OD-3](#od-3-multiple-component-groups). The baseline draft describes
first-present lookup with no inherited merge. The approved general composition
direction uses recursive tables, scalar replacement, and array replacement
unless a field explicitly opts into another rule.

**Recommendation:** Use a fixed layer order with the same typed merge vocabulary
as document composition, while defining each inheritance exception at the field
contract. The actual layer order must be accepted together with the
multiple-group decision rather than inferred from current azldev.

## OD-3: Multiple component groups

**Question:** When a component belongs to multiple groups, how are group
defaults ordered and how are conflicts handled?

**Alternative A - lexical group-name order**

- Is deterministic for explicit memberships.
- Makes renaming a group change component behavior.
- Matches part of current azldev but is not stable for all discovered
  components.

**Alternative B - explicit group priority**

- Makes ordering intentional and reviewable.
- Adds a field and requires tie behavior.

**Alternative C - reject conflicting contributions**

- Prevents hidden precedence and rename sensitivity.
- Requires a precise definition of conflict and may reject useful composition
  of disjoint fields.

**Evidence:** Current explicit membership sorts group names lexically, while a
component discovered by multiple groups can receive map-iteration-dependent
effective config. The baseline draft also relies on lexical names.

**Recommendation:** Require an explicit priority for ordered group composition,
preserve declaration-independent ordering, and reject conflicting contributions
at equal priority while allowing disjoint fields. This requires field and
conflict definitions in Slice 2 before it can become normative.

## OD-4: Canonical resolved-model serialization

**Question:** What byte representation is used for exact resolved-model fixture
comparison?

**Alternative A - canonical TOML**

- Is familiar to document authors.
- Poorly represents provenance and distinctions between explicit and
  synthesized values.

**Alternative B - schema-defined canonical JSON**

- Has broad tooling support and straightforward object-key ordering.
- Requires explicit encodings for TOML date/time types, path provenance, and
  numeric constraints.

**Alternative C - semantic comparison only**

- Avoids a serialization design.
- Makes portable fixtures and cross-implementation diffing harder.

**Evidence:** The approved scope requires an exact canonical resolved
representation, but neither the baseline nor current azldev defines one.

**Recommendation:** Define a dedicated canonical JSON representation with
sorted object keys, preserved array order, explicit tagged TOML temporal
values, exact integer rules, and structured provenance. Prototype fixtures
before standardizing it.

## OD-5: Final core and profile assignment

**Question:** Which characterized fields are required core and which belong to
named optional profiles?

**Alternatives:** Put every current field in core; place independently optional
domains in profiles; or exclude tool-specific behavior from the portable
contract.

**Evidence:** The field inventory identifies component construction and its
required sources as core candidates, with images, tests, publishing, and
executable custom-source generation independently optional. Resource inputs
cross several workflows and need final assignment during the object-model
slice.

**Recommendation:** Keep component construction and hash-verifiable source
artifacts in core. Use named profiles for images, tests, publishing, and
executable source generation. Decide resource ownership from the complete
Slice 2 field contracts rather than from current file layout.

## Decisions assigned to later slices

The following approved questions remain open and are intentionally not resolved
by Slice 1:

| Decision | Owning slice |
| --- | --- |
| Environmental-input classification and defaults | Slice 4 |
| Per-overlay match cardinality where evidence conflicts | Slice 3 |
| Canonical transformed-archive bytes | Slice 3 |
| Long-term version compatibility and deprecation | Later revision / Slice 4 placeholder |
| Future semantic component-transformation model | Future specification revision |

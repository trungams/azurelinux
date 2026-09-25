[Return to index](./index.md)

# Document format

This chapter defines the identity and lexical contract for every source
document. Loading and composition are defined in
[Document loading and composition](./loading.md).

## TOML syntax and encoding

Every source document:

- MUST be a valid UTF-8 document;
- MUST conform to TOML 1.0.0 syntax;
- MUST be decoded without replacement characters or implementation-specific
  character-set conversion; and
- MUST be parsed before any include, composition, or object-model processing.

Invalid UTF-8, invalid TOML, a duplicate TOML key, redefining a value as a
table, or any other TOML parse failure is an error.

TOML strings retain their Unicode scalar values. This specification does not
apply Unicode normalization to keys, names, or path strings.

## Root document identity

The root document MUST contain the following key at the exact root-table TOML
path `spec-version`:

```toml
spec-version = "0.1"
```

`spec-version` MUST be a string and MUST equal `"0.1"` for this revision. A
processor MUST reject an unsupported value rather than interpret it as another
version.

`spec-version` SHOULD be the first key in the root document for readability and
early discovery. Its physical position does not affect meaning.

The root document MUST contain exactly one `spec-version` assignment. TOML
already makes a duplicate assignment a parse error.

The long-term version numbering, compatibility, and deprecation policy is not
defined by this revision.

## Included fragments

An included fragment MUST NOT contain a key at the exact root-table TOML path
`spec-version`. The version selected by the root document governs every
fragment in the include graph.

This is a valid fragment header:

```toml
includes = ["defaults/*.toml"]
```

The fragment can contain `includes` because includes are recursive. Fragment
validity does not depend on a separately selected version.

The strings `spec-version` and `includes` have document-control meaning only at
their exact root-table paths. At any other path, including as an entry name in
a name-keyed map, the same string is interpreted by the owning field contract.
For example, a name-keyed map can contain entries named `spec-version` and
`includes` when that map's contract permits arbitrary names. A fragment
therefore forbids only root-table `spec-version`, not that string at every
nested path.

## Strict key vocabulary

Each specification-defined field has one canonical, case-sensitive key
spelling. Keys are compared as the exact strings produced by TOML parsing.

In particular:

- `-` and `_` are distinct characters;
- `spec-version` and `spec_version` are different keys;
- quoting a key does not create an alias for another spelling; and
- physical key order does not change a document's meaning.

For example, the following TOML parses successfully but is not a valid root
document because `spec_version` is an unknown key and the required
`spec-version` key is absent:

```toml
spec_version = "0.1"
```

Unknown keys are errors at every closed table defined by this specification.
A table explicitly defined as a name-keyed map accepts arbitrary entry names,
but every value under those names remains subject to its defined closed
vocabulary. This revision defines no vendor-extension or ignore-unknown-key
mechanism.

Only the exact root-table paths `spec-version` and `includes` are fully defined
as document controls in this slice. Canonical object-field spellings become
normative in their owning field chapters. The complete canonical root
vocabulary is defined in [Top-level object model](./objects.md). An
implementation MUST NOT infer aliases from historical underscore- or
camel-case spellings.

An implementation may retain exact key spellings and typed TOML values for
not-yet-defined object fields as preparatory internal representation. That
retention does not classify a spelling as known, accept the field for
conformance, or weaken the strict unknown-key rule. Once an owning field chapter
closes a table vocabulary, every other key in that table is an error.

## TOML value types

The TOML value-type lattice used by parsing, source-model validation,
composition, and semantic equality is:

- scalar types: string, integer, float, Boolean, offset date-time, local
  date-time, local date, and local time;
- the array container type, whose elements retain their individual TOML value
  types; and
- the table container type.

Local date, local time, local date-time, and offset date-time are four distinct
types. Integer and float are distinct types. An inline table and a standard
table are both table values. An array of tables is an array whose elements are
table values; it is not an additional TOML value type.

Composition uses these exact types when deciding whether two values can be
replaced or recursively merged. Field contracts can further restrict an array
to one element type or a table to one shape, but those restrictions do not
change the TOML type lattice.

## Semantic equality

Two TOML values are semantically equal only when they have the same TOML type
and satisfy the rule for that type:

| Type | Equality rule |
| --- | --- |
| String | The Unicode scalar-value sequences are identical. No normalization or case folding is applied. |
| Integer | The signed integer values are equal. |
| Boolean | The Boolean values are equal. |
| Float | Both values are the same IEEE 754 binary64 value under the special rules below. |
| Local date | Year, month, and day are equal. |
| Local time | Hour, minute, second, and exact fractional second are equal. |
| Local date-time | Local date and local time components are equal. |
| Offset date-time | Both values denote the same UTC instant, including the exact fractional second. The written offset need not be identical. |
| Array | Lengths are equal and corresponding elements are semantically equal in order. |
| Table | Exact key sets are equal and the value for each key is semantically equal recursively. |

Every TOML floating-point value MUST be represented for these rules as an IEEE
754 binary64 value. All NaN values are semantically equal to one another
regardless of the sign spelling. Positive infinity and negative infinity are
equal only to the same infinity. Positive zero and negative zero are distinct.
Finite nonzero floats are equal when their binary64 values are identical.

A conforming processor MUST preserve every fractional-second digit accepted by
TOML as an exact decimal fraction; it MUST NOT silently truncate to its host
date-time library's precision. Fractions that differ only by trailing zeroes
are equal, so `.1`, `.10`, and `.100` denote the same fractional second.

Two source documents are structurally equivalent at the document layer when
TOML parsing produces semantically equal root tables under these rules.
Structural equivalence ignores:

- comments;
- insignificant whitespace;
- choice of equivalent TOML string or numeric syntax; and
- physical key order.

Implementations MUST NOT coerce between TOML types during parsing or equality.
In particular, integer `1`, float `1.0`, a local date-time, and an offset
date-time are pairwise different types even if an application could convert
between their values.

Structural equivalence does not imply equal path meaning. Relative path values
also carry the provenance defined in
[Path provenance](./loading.md#path-provenance).

## Document-layer validation

For each source document, a processor MUST perform these checks before adding
its body to the composition sequence:

1. Decode UTF-8 and parse TOML.
2. Check whether the document is the root or an included fragment.
3. Validate `spec-version` presence, absence, type, and value as applicable.
4. Validate document-control keys.
5. Record value provenance.

After these checks and before composition, every document MUST independently
pass **source-model validation**. For each field contract defined by this
specification, source-model validation checks every value present in that
document for:

- whether its local key is permitted by the owning closed-table or
  name-keyed-map contract;
- its TOML value type;
- the TOML value type of every array element;
- the complete recursively nested table or array-of-tables shape; and
- every constraint whose validity depends only on that value and its containing
  source document.

This validation is recursive and applies even when a later document would
replace the value. A wrong array-element type, invalid nested shape, unknown
key, or other source-local error therefore cannot be discarded by composition.

Requiredness, defaults, effective-value rules, reference checks, and other
cross-document constraints are not source-local checks. They are validated
after composition or during the later phase that owns them. A source model can
therefore be partial without permitting an invalid value that is present.

The [source-model validation fixture](./examples/source-model-validation/README.md)
is a metasyntactic falsifier for this phase boundary.

The [TOML equality cases](./examples/toml-equality/README.md) and
[document-control path cases](./examples/control-key-paths/README.md) provide
focused examples for the other document-layer boundaries.

[Return to index](./index.md)

# Top-level object model

The root table is closed. In addition to the document-control keys
`spec-version` and `includes`, the only recognized top-level keys are listed
below. `$schema` and every alternate underscore or camel-case spelling are
unknown keys and are errors.

## Contract notation

Every field table in this specification uses these attributes:

- **Type** includes whether a table is closed, name-keyed, or ordered.
- **Required/default** distinguishes source-document optionality from an
  effective-value requirement.
- **Constraints/path base** says `N/A` for non-path fields. Relative paths
  always use the defining document's retained path base.
- **Composition** uses the rules in
  [Document loading and composition](./loading.md#composition-sequence).
- **Inheritance** says `N/A` when a field is not part of an inherited
  `ComponentConfig` or `PackageConfig`.
- **Invariants/errors** states cross-field or reference requirements and the
  failure behavior.
- **Disposition** is `Core`, a named profile, `Deprecated`, or
  `Excluded/deferred`.

Unless a row says otherwise:

- a scalar uses later-value replacement;
- a closed table composes recursively;
- a name-keyed map composes by exact key and then recursively composes its
  value;
- an array uses later-array replacement, including replacement by `[]`;
- absence has no value and does not clear an earlier value;
- present values are validated during `SD-MODEL`, requiredness during
  `CM-VALIDATE`, and effective/reference invariants during the applicable
  `RM-*` phase;
- a violation is an error and no result from that phase is produced; and
- the diagnostic uses the applicable class and fields from
  [Validation and diagnostics](./validation.md); and
- component inheritance uses the fixed low-to-high layer order and the field's
  typed composition rule from
  [Component inheritance order](./resolution.md#component-inheritance-order);
  within the component-group layer, append-composed arrays may combine and
  recursively present map/table leaves may combine only when disjoint, while
  any repeated non-append leaf is an `RM-EFFECTIVE` error.

`overlays` is the sole Slice 2 document-composition array exception:
contributions append in document reach order within one composed provider.
Inheritance also appends `overlays` in low-to-high layer order. Contributions
from several component groups append in group-name UTF-8 order; that sequence
order does not establish scalar or replacing-array precedence.

## Names and references

A **name-keyed map** is a TOML table whose immediate child keys are object
names. A name MUST be non-empty, MUST contain no C0 control or U+007F, and MUST
not begin or end with Unicode whitespace. Names are compared as exact Unicode
scalar-value sequences without normalization, case folding, or locale rules.
Two exact-equal keys identify the same composed map entry.

A name reference uses the same comparison. A missing or wrong-kind target is an
error. Duplicate references in a set-like array are errors unless the owning
contract explicitly defines ordered repetition.

## Canonical top-level vocabulary

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `project` | Closed table | Optional; no default. | N/A. | Recursive table. | N/A. | At most one composed project object. | Core |
| `distros` | Name-keyed map of closed `DistroDefinition` tables | Optional; no default. | Names follow the common rule. | Map by key. | N/A. | Required when any effective reference names a distro. | Core |
| `resources` | Closed table containing name-keyed resource maps | Optional; no default. | N/A. | Recursive table and map-by-key. | N/A. | Presence requires validation and preservation but does not itself execute a repository operation. | Shared RPM-build/image profile data |
| `component-groups` | Name-keyed map of closed `ComponentGroup` tables | Optional; no default. | Names follow the common rule. | Map by key. | Supplies candidate inherited component values. | Discovery and explicit membership must resolve without duplicate or missing component identities. | Core |
| `default-component-config` | Closed `ComponentConfig` table | Optional; no default. | Field-specific paths retain their defining bases. | Recursive table with field exceptions. | Project-default provider at the second low-to-high layer. | A partial config is allowed; effective components must satisfy all required contracts. | Core |
| `components` | Name-keyed map of closed `ComponentConfig` tables | Optional; no default. | Names follow the common rule. | Map by key; same-name definitions compose recursively. | Direct candidate provider for the named component. | Explicit and discovered definitions of one exact name must identify one component or error. | Core |
| `images` | Name-keyed map of closed `ImageConfig` tables | Optional; no default. | Names follow the common rule. | Map by key. | N/A. | Presence selects image-profile data participation, not an image operation. | Image profile |
| `default-package-config` | Closed `PackageConfig` table | Optional; no default. | N/A. | Recursive table. | Candidate provider for package publishing values. | Presence selects publishing data participation, not publication. | Publishing profile |
| `package-groups` | Name-keyed map of closed `PackageGroup` tables | Optional; no default. | Names follow the common rule. | Map by key. | Candidate package provider. | Presence selects publishing data participation, not publication. | Publishing profile |
| `tests` | Name-keyed map of closed `TestDefinition` tables | Optional; no default. | Names follow the common rule. | Map by key. | N/A. | Presence selects test-profile data participation, not test execution. | Test profile |
| `test-groups` | Name-keyed map of closed `TestGroup` tables | Optional; no default. | Names follow the common rule. | Map by key. | N/A. | Presence selects test-profile data participation, not test execution. | Test profile |
| `tools` | Reserved tool-specific table | Forbidden in a conforming `0.1` document. | N/A. | N/A. | N/A. | Presence is an unknown-key error; a tool may interpret it only outside the portable specification contract. | Tool-specific |

Profile fields remain part of the canonical vocabulary so their spelling and
structure are portable. Presence, validation, preservation, explicit operation
selection, and unsupported-operation behavior are centralized in
[Optional profile data and operations](./profiles.md). Profile support cannot weaken core validation. Resources are shared optional
RPM-build/image profile data and do not activate either operation merely by
being present.

The tracked [field-disposition inventory](./field-disposition.tsv) maps every
characterized schema path to its normative chapter and reusable contract
section. The
[opaque framework inventory](./opaque-field-disposition.tsv) extends that
coverage below schema-opaque test tables, and the
[field/profile matrix](./profile-matrix.tsv) independently checks ownership
assignments. These are traceability artifacts, not substitutes for the
contracts.

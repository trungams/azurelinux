[Return to index](./index.md)

# Test profile

The optional test profile defines test metadata and references. Its data is
validated and preserved whenever present, but test selection or execution is
an explicit operation under [the common profile rules](./profiles.md). It does
not standardize a universal test-result byte format. Runner installation,
architecture, toolchain, network, credential scope, and resource limits are
explicit inputs to the selected `test` operation rather than ambient state.

## Test definitions

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `tests` | Name-keyed map of closed `TestDefinition` tables | Optional; default empty map. | Common name constraints. | Map by key and recursive table. | N/A. | Presence selects test-profile data participation, not execution. | Test profile |
| `tests.<test>.type` | string enum | Required; no default. | Exactly `pytest`, `lisa`, or `tmt`; path base N/A. | Scalar replace. | N/A. | Exactly the matching framework subtable is required. | Test profile |
| `tests.<test>.description` | string | Optional; no default. | Diagnostic UTF-8; path base N/A. | Scalar replace. | N/A. | No execution semantics. | Test profile |
| `tests.<test>.kind` | string enum | Optional; default `functional`. | Exactly `functional` or `performance`; path base N/A. | Scalar replace. | N/A. | Unsupported value is an error. | Test profile |
| `tests.<test>.long-running` | boolean | Optional; default `false`. | N/A. | Scalar replace. | N/A. | Scheduling metadata only. | Test profile |
| `tests.<test>.metrics-enabled` | boolean | Optional; default `false`. | N/A. | Scalar replace. | N/A. | Result-policy metadata only. | Test profile |
| `tests.<test>.required-capabilities` | Array of strings | Optional; default `[]`. | Unique names from the image capability vocabulary. | Array replace. | N/A. | Unknown or duplicate capabilities are errors. | Test profile |
| `tests.<test>.pytest` | Framework table | Required iff `type = "pytest"`; otherwise forbidden. | Closed portable subset below; relative paths use the test-defining document. | Recursive table. | N/A. | Mismatched framework table is an error. | Test profile |
| `tests.<test>.lisa` | Framework table | Required iff `type = "lisa"`; otherwise forbidden. | Closed portable subset below. | Recursive table. | N/A. | Mismatched framework table is an error. | Test profile |
| `tests.<test>.tmt` | Framework table | Required iff `type = "tmt"`; otherwise forbidden. | Closed portable subset below. | Recursive table. | N/A. | Mismatched framework table is an error. | Test profile |

### Pytest fields

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `tests.<test>.pytest.working-dir` | string path | Required for `pytest`; no default. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from the test-defining document; target is a directory. | Scalar replace retaining provenance. | N/A. | Missing/non-directory target is an error. | Test profile |
| `tests.<test>.pytest.test-paths` | Array of string paths or path patterns | Required and non-empty for `pytest`; no default. | Each entry is relative to `working-dir` and imports both the [portable pattern syntax](./loading.md#portable-relative-path-pattern-syntax) and [project-tree traversal](./loading.md#project-tree-pattern-traversal), including recursive `**`; final targets are regular files or directories accepted by the selected pytest operation. | Array replace. | N/A. | The component-discovery terminal `.spec` filter does not apply. Expansion preserves array order and sorts each pattern's matches; duplicate final paths and a literal or pattern with no match are errors. | Test profile |
| `tests.<test>.pytest.extra-args` | Array of strings | Optional; default `[]`. | Ordered exact arguments; path base N/A. | Array replace. | N/A. | Empty arguments are preserved. Only `{image-path}`, `{image-name}`, and `{capabilities}` are recognized placeholders; an unavailable or unknown placeholder is an operation-time error. | Test profile |
| `tests.<test>.pytest.install` | string enum | Optional; default `none`. | Exactly `none`, `pyproject`, or `requirements`; path base N/A. | Scalar replace. | N/A. | `pyproject` requires `pyproject.toml` and `requirements` requires `requirements.txt` in `working-dir` when execution is selected. | Test profile |

### LISA fields

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `tests.<test>.lisa.source` | Closed table | Optional; no default. | N/A. | Recursive table. | N/A. | Required only by an operation that must obtain the LISA framework from Git; absence permits metadata-only or externally supplied execution. | Test profile |
| `tests.<test>.lisa.source.git-url` | URI string | Required when `source` is present. | Absolute `https` Git URI; path base N/A. | Scalar replace. | N/A. | Redirects must remain `https`. | Test profile |
| `tests.<test>.lisa.source.ref` | string | Required when `source` is present. | Exactly 40 lowercase hexadecimal digits naming a Git commit. | Scalar replace. | N/A. | Branches, tags, abbreviated IDs, and missing objects are errors before checkout. | Test profile |
| `tests.<test>.lisa.criteria` | Closed criteria table or non-empty array of closed criteria tables | Conditionally required; no default. | Criteria fields are below; path base N/A. | Later complete value replaces. | N/A. | Exactly one of `criteria`, `name`, `testcase-name`, or `testcase-names` is present at the LISA-table level. | Test profile |
| `tests.<test>.lisa.criteria.{name,testcase-name}` | string | Optional. | Non-empty; path base N/A. | Entry scalar. | N/A. | At most one of `name`, `testcase-name`, and `testcase-names` is present in one criterion. | Test profile |
| `tests.<test>.lisa.criteria.testcase-names` | Array of strings | Optional. | Non-empty list of non-empty names; path base N/A. | Entry array. | N/A. | Mutually exclusive with criterion `name` and `testcase-name`. | Test profile |
| `tests.<test>.lisa.criteria.{area,category}` | string | Optional. | Non-empty; path base N/A. | Entry scalar. | N/A. | Exact LISA selector text. | Test profile |
| `tests.<test>.lisa.criteria.priority` | integer or array of integers | Optional. | Each integer is in `0..4`; an array is non-empty. | Entry scalar or array. | N/A. | Other TOML types and out-of-range values are errors. | Test profile |
| `tests.<test>.lisa.criteria.tags` | Array of strings | Optional. | Non-empty list of non-empty tags; path base N/A. | Entry array. | N/A. | Wrong element type is an error. | Test profile |
| `tests.<test>.lisa.name` | string | Conditionally required shorthand. | Non-empty; path base N/A. | Scalar replace. | N/A. | Used only when `criteria`, `testcase-name`, and `testcase-names` are absent; maps to one criterion `name`. | Test profile |
| `tests.<test>.lisa.testcase-name` | string | Conditionally required shorthand. | Non-empty; path base N/A. | Scalar replace. | N/A. | Used only when the other three table-level selectors are absent; maps to one criterion `testcase-name`. | Test profile |
| `tests.<test>.lisa.testcase-names` | Array of strings | Conditionally required shorthand. | Non-empty list of non-empty names; path base N/A. | Array replace. | N/A. | Used only when the other three table-level selectors are absent; maps to one criterion `testcase-names`. | Test profile |
| `tests.<test>.lisa.pip-pre-install` | Array of strings | Optional; default `[]`. | Ordered non-empty package requirement strings; path base N/A. | Array replace. | N/A. | Passed only to a selected local LISA operation. | Test profile |
| `tests.<test>.lisa.pip-extras` | Array of strings | Optional; default `[]`. | Ordered non-empty extra names; path base N/A. | Array replace. | N/A. | Passed only to a selected local LISA operation. | Test profile |
| `tests.<test>.lisa.extra-args` | Array of strings | Optional; default `[]`. | Same placeholder grammar as pytest `extra-args`; path base N/A. | Array replace. | N/A. | Unknown or unavailable placeholders are operation-time errors. | Test profile |

Every criteria table is non-empty and contains at least one of `name`, `area`,
`category`, `priority`, `tags`, `testcase-name`, or `testcase-names`. Criteria
array order is retained. The three top-level shorthand forms map to one
criterion and are not combined with an explicit `criteria` value.

### TMT fields

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `tests.<test>.tmt.plan` | string | Required for `tmt`; no default. | Exact absolute TMT plan identity beginning with `/`; path base N/A. | Scalar replace. | N/A. | Missing, empty, or relative values are errors. The identity is supplied unchanged to the selected pinned runner. | Test profile |
| `tests.<test>.tmt.source` | Closed table | Required for `tmt`; no default. | N/A. | Recursive table. | N/A. | Only `git-url` and `ref` are known. | Test profile |
| `tests.<test>.tmt.source.git-url` | URI string | Required; no default. | Absolute `https` Git URI; path base N/A. | Scalar replace. | N/A. | Redirects must remain `https`. | Test profile |
| `tests.<test>.tmt.source.ref` | string | Required; no default. | Exactly 40 lowercase hex digits naming a Git commit. | Scalar replace. | N/A. | Branches, tags, and abbreviated IDs are errors. | Test profile |

The current schema makes the three framework tables opaque. The tracked
[opaque-field disposition inventory](./opaque-field-disposition.tsv) closes
that hidden vocabulary. Any framework-subtable key not listed there is an
unknown-key error in `0.1`. The former LISA `test-cases`, `test-suites`, and
`framework` spellings are not aliases; their migrations are listed in that
inventory and in [Compatibility](./compatibility.md).

## Test references

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `TestRef.name` | string reference | Required iff `group` absent. | Exact `tests` key; path base N/A. | Entry scalar. | Inherited with containing component array. | Exactly one of `name` and `group`; missing target is an error. | Test profile |
| `TestRef.group` | string reference | Required iff `name` absent. | Exact `test-groups` key; path base N/A. | Entry scalar. | Inherited with containing component array. | Exactly one of `name` and `group`; missing target is an error. | Test profile |
| `ComponentConfig.tests.tests` | Ordered array of closed `TestRef` tables | Optional; default `[]`. | Path base N/A. | Array replace. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Expansion must not produce duplicate test names. | Test profile |
| `ImageConfig.tests.tests` | Ordered array of closed `TestRef` tables | Optional; default `[]`. | Path base N/A. | Array replace. | N/A. | Expanded tests must satisfy image capabilities. | Image plus test profiles |

## Test groups

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `test-groups` | Name-keyed map of closed group tables | Optional; default empty map. | Common name constraints. | Map by key and recursive table. | N/A. | Presence selects test-profile data participation, not execution. | Test profile |
| `test-groups.<group>.description` | string | Optional; no default. | Diagnostic UTF-8; path base N/A. | Scalar replace. | N/A. | No processing semantics. | Test profile |
| `test-groups.<group>.tests` | Ordered array of closed `{ name = string }` tables | Optional; default `[]`. | Exact test references. | Array replace. | N/A. | Group references are forbidden; duplicate/missing tests are errors. | Test profile |
| `test-groups.<group>.tests[].name` | string reference | Required; no default. | Exact `tests` key; path base N/A. | Entry scalar. | N/A. | Missing target is an error. | Test profile |

Group expansion preserves declared order. Nested test-group references are
forbidden, so cycles cannot occur.

Test content executes only inside the selected profile security boundary.
Presence of a test definition or reference grants no access to credentials,
the host home directory, caches, device nodes, container sockets, or
publication destinations. Result timing and external service state are not
portable conformance outputs.

[Return to components](./components.md)

# Build and release configuration

Build fields are portable data for the **RPM-build profile**. They are
validated and preserved whenever present; they affect behavior only when an
RPM-build operation is explicitly selected under
[the common profile rules](./profiles.md).

## Build configuration

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `build.with` | Array of strings | Optional; default `[]`. | Each value is a non-empty rpmbuild conditional name with no ASCII whitespace; path base N/A. | Array replace. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Duplicate names and overlap with `without` are errors. | RPM-build profile |
| `build.without` | Array of strings | Optional; default `[]`. | Same grammar as `with`; path base N/A. | Array replace. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Duplicate names and overlap with `with` are errors. | RPM-build profile |
| `build.defines` | Map of string to string | Optional; default empty map. | Macro names are non-empty and contain no ASCII whitespace; values are passed as exact strings; path base N/A. | Map by exact key; later value replaces. | Later layers replace the same key; several groups may contribute only different keys. | A key also present in `undefines` is an error. | RPM-build profile |
| `build.undefines` | Array of strings | Optional; default `[]`. | Macro-name constraints match `defines`; path base N/A. | Array replace. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Duplicate names or overlap with `defines` are errors. | RPM-build profile |
| `build.emit-upstream-provenance` | boolean | Optional; default `false`. | Path base N/A. | Scalar replace, including `false`. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Controls whether the RPM-build profile emits declared upstream provenance; any tree entry it creates must satisfy the [artifact and provenance contracts](./artifacts.md). | RPM-build profile |
| `build.check` | Closed table | Optional; no default table. | Path base N/A. | Recursive table. | Recursive typed composition in fixed low-to-high layer order; several groups may contribute only disjoint child leaves. | Only `skip` and `skip-reason` are known. | RPM-build profile |
| `build.check.skip` | boolean | Optional; default `false`. | Path base N/A. | Scalar replace. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | `skip-reason` is required when true and forbidden when false. | RPM-build profile |
| `build.check.skip-reason` | string | Conditionally required; no default. | Non-empty after trimming Unicode whitespace; path base N/A. | Scalar replace. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Required iff `skip = true`. The characterized `skip_reason` spelling is unknown and is an error. | RPM-build profile |
| `build.failure` | Reserved closed table | Forbidden. | Scheduler expectation data; path base N/A. | N/A. | N/A. | `expected` and `expected-reason` are tool-specific and are unknown keys in conformance mode. | Excluded/deferred |
| `build.hints` | Reserved closed table | Forbidden. | Scheduler hint data; path base N/A. | N/A. | N/A. | `expensive` is tool-specific and is an unknown key in conformance mode. | Excluded/deferred |

The profile's command-line spelling is not normative. A producer must translate
these typed values to its build engine without changing their set/map
semantics. The complete base macro map, undefined-name set, architecture,
toolchain, repository state, and environment are explicit selected-operation
inputs under
[RPM macro and external tool context](./determinism.md#rpm-macro-and-external-tool-context).
Host RPM configuration and environment-derived macros are prohibited.

Final RPM or SRPM bytes are not a conformance output in this revision.

## Render configuration disposition

`render.skip-file-filter` and the containing `render` table expose one tool's
materialization implementation rather than a portable output contract. Both are
excluded from conforming `0.1` documents and are unknown-key errors.
[Materialized artifacts](./artifacts.md) defines the tree namespace,
placement, exclusions, and comparison bytes directly; it does not preserve
this switch.

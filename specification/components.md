[Return to top-level objects](./objects.md)

# Components

`components` is a name-keyed map of closed `ComponentConfig` tables.
`default-component-config` and the identically shaped tables under distro
versions and component groups use the same reusable contract.

Same-name component tables from different source documents compose by exact
map key. They are not duplicate-object errors. Every leaf retains the
provenance of the document that supplied its effective value.

## ComponentConfig fields

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `spec` | Closed table | Optional in a partial config; required in an effective component unless discovery synthesizes it during component construction. | See [Upstream component source](./sources.md#upstream-component-source). | Recursive table. | Recursive typed composition in fixed low-to-high layer order; same-layer group leaves must be disjoint. | Explicit/discovered reconciliation follows the [discovery collision matrix](./component_groups.md#discovered-component-construction); the result must be exactly one valid local or upstream source. | Core |
| `release` | Closed table | Optional; `calculation` defaults to `auto` when the effective component is created. | Path base N/A. | Recursive table. | Recursive typed composition in fixed low-to-high layer order; same-layer group leaves must be disjoint. | Unknown release fields are errors. | RPM-build profile |
| `overlays` | Ordered array of closed overlay tables | Optional; default `[]`. | Operation fields, paths, and effects are defined in [Overlay transformations](./overlays.md). | Append exception in document reach order. | Append in low-to-high layer order; multiple groups append in group-name UTF-8 order without establishing precedence. | Every entry must pass the operation matrix and individual contract before materialization. | Core |
| `overlay-files` | Array of string paths | Optional; default `[]`. | Each entry uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document and targets a regular TOML file. | Array replace; explicit `[]` clears. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Loaded entries are appended after inline `overlays` and retain secondary-document provenance. | Core |
| `build` | Closed table | Optional; no default table. | See [Build configuration](./build.md#build-configuration). | Recursive table with replacing arrays/maps. | Recursive typed composition in fixed low-to-high layer order; same-layer group leaves must be disjoint except defined append fields. | Validated and preserved whenever present; used only by an explicitly selected RPM-build operation. | RPM-build profile |
| `render` | Reserved closed table | Forbidden in a conforming `0.1` document. | Tool-specific artifact filtering. | N/A. | N/A. | Presence, including `skip-file-filter`, is an unknown-key error in conformance mode. | Excluded/deferred |
| `source-files` | Array of closed `SourceFile` tables | Optional; default `[]`. | Field paths use defining provenance. | Array replace. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Effective filenames must be unique. See [Source artifacts](./sources.md#source-artifact-field-contracts). | Core |
| `packages` | Name-keyed map of closed `PackageConfig` tables | Optional; default empty map. | Package names use the common name rule. | Map by key. | Recursive map composition in fixed low-to-high layer order; several groups may contribute only disjoint package leaves. | Validated and preserved as publishing data; presence does not publish. | Publishing profile data |
| `publish` | Closed component publish table | Optional; no default. | N/A. | Recursive table. | Recursive typed composition in fixed low-to-high layer order; same-layer group channel leaves must be disjoint. | Defined in [Packages and publishing](./packages.md); presence does not publish. | Publishing profile |
| `tests` | Closed component test-reference table | Optional; no default. | N/A. | Recursive table; `tests` array replaces. | Recursive typed composition in fixed low-to-high layer order; several groups contributing the replacing `tests` array are a destructive overlap error. | Defined in [Tests](./tests.md); presence does not execute tests. | Test profile |

## Release calculation

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `release.calculation` | string enum | Optional; default `auto`. | Exactly `auto`, `autorelease`, `static`, or `manual`; path base N/A. | Scalar replace. | Participates as a scalar. | Unsupported value is an error. | RPM-build profile |

`manual` leaves the parsed `Release:` value unchanged. `autorelease` requires
the spec to use `%autorelease` and leaves that value unchanged. `static`
requires a single decimal integer optionally followed by `%{dist}` or
`%{?dist}` and increments the integer by one. `auto` selects `autorelease`
when the parsed release uses `%autorelease`, otherwise `static`. A failed
precondition is an error; it is not a silent fallback.

Component construction and its source identity remain core. Release rewriting,
rpmbuild controls, publishing, and tests belong to their named profiles.

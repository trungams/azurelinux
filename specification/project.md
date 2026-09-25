[Return to top-level objects](./objects.md)

# Project

`project` is one optional closed table. It carries project-wide descriptive
values and a default source distro reference. Operational directory settings
from current tooling are explicitly outside the portable model.

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `project.description` | string | Optional; no default. | Any UTF-8 string without C0 controls except tab/newline; path base N/A. | Scalar replace. | N/A. | No processing semantics. Wrong type is an error. | Core |
| `project.default-distro` | Closed `DistroReference` table | Optional; no default. | N/A. | Recursive table. | Used only when the entire effective `spec.upstream-distro` source-reference table is absent. | If present, `name` and `version` are required and must resolve exactly. A present component table is never completed field-by-field from this default. It never supplies or changes the immutable target distro/version evaluation input. | Core |
| `project.default-distro.name` | string reference | Required when `default-distro` is present; no default. | Exact `distros` key; path base N/A. | Scalar replace. | N/A. | Missing distro is an error. | Core |
| `project.default-distro.version` | string reference | Required when `default-distro` is present; no default. | Exact version key in the referenced distro; path base N/A. | Scalar replace. | N/A. | `distros.<name>.default-version` does not fill an omitted value. Missing version is an error. | Core |
| `project.default-distro.snapshot` | RFC 3339 timestamp string | Optional; no default. | Explicit offset or `Z` required; path base N/A. | Scalar replace. | N/A. | Selector only; it cannot replace an exact `upstream-commit`. | Core |
| `project.default-author-email` | string | Optional; no default. | Non-empty when present; no C0 controls; path base N/A. | Scalar replace. | N/A. | It is provenance text only; this revision does not impose mail-delivery syntax. | Core |
| `project.log-dir` | reserved string path | Forbidden in a conforming `0.1` document. | Tool work-layout path. | N/A. | N/A. | Presence is an unknown-key error in conformance mode. | Excluded/deferred |
| `project.work-dir` | reserved string path | Forbidden in a conforming `0.1` document. | Tool work-layout path. | N/A. | N/A. | Presence is an unknown-key error in conformance mode. | Excluded/deferred |
| `project.output-dir` | reserved string path | Forbidden in a conforming `0.1` document. | Tool output-layout path. | N/A. | N/A. | Presence is an unknown-key error in conformance mode. | Excluded/deferred |
| `project.rendered-specs-dir` | reserved string path | Forbidden in a conforming `0.1` document. | Tool rendered-tree layout path. | N/A. | N/A. | Presence is an unknown-key error in conformance mode. | Excluded/deferred |
| `project.lock-dir` | reserved string path | Forbidden in a conforming `0.1` document; no normative default. | Tool lock/cache layout path. | N/A. | N/A. | Presence is an unknown-key error. It does not establish a normative lock file. | Excluded/deferred |

**Review note:** Current azldev accepts the five operational directory fields
and defaults `lock-dir`. The portable specification instead defines observable
inputs and outputs and does not standardize those layouts.

[Return to top-level objects](./objects.md)

# Distros and versions

`distros` is a name-keyed map. Each value is a closed
`DistroDefinition`; its `versions` value is another name-keyed map of closed
`DistroVersion` tables.

## Distro fields

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `distros.<distro>.description` | string | Optional; no default. | Descriptive UTF-8; path base N/A. | Scalar replace. | N/A. | No processing semantics. | Core |
| `distros.<distro>.default-version` | string reference | Optional; no default. | Exact key in this distro's `versions`; path base N/A. | Scalar replace. | N/A. | When present it must resolve. It is advisory only: it neither supplies the required target-version evaluation input nor fills a source `DistroReference.version`. | Core |
| `distros.<distro>.dist-git-base-uri` | URI template string | Conditionally required; no default. | Canonical source URI template with exactly one `$name`, optional single `$releasever`, and no artifact placeholders, as defined in [Sources](./sources.md#source-uri-templates); path base N/A. | Scalar replace. | N/A. | Required for any upstream component using this source distro. Invalid template, value, or final URI is an error before repository I/O. | Core |
| `distros.<distro>.lookaside-base-uri` | URI template string | Conditionally required; no default. | Canonical source URI template with exact lookaside placeholder cardinalities from [Sources](./sources.md#source-uri-templates); path base N/A. | Scalar replace. | N/A. | Required when upstream lookaside artifacts are acquired. Invalid template, value, or final URI is an error before request I/O. | Core |
| `distros.<distro>.disable-origins` | boolean | Optional; default `false`. | Path base N/A. | Scalar replace, including explicit `false`. | N/A. | When true, download-origin fallback after a lookaside miss is forbidden. It does not disable an explicitly selected custom profile operation. | Core |
| `distros.<distro>.versions` | Name-keyed map of closed `DistroVersion` tables | Optional; no default. | Version names follow the common name rule. | Map by key. | N/A. | Every referenced version must exist. | Core |
| `distros.<distro>.repos` | Array of closed `{ base-uri = string }` tables | Optional; default `[]`. | Each value uses the [repository URI-template algorithm](./resources.md#repository-uri-templates) with optional `$releasever` and `$$`; path base N/A. | Array replace. | N/A. | Used only by the RPM-build profile; invalid template, missing `base-uri`, or invalid final URI is an error before repository access. | RPM-build profile |

## Version fields

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `distros.<distro>.versions.<version>.description` | string | Optional; no default. | Descriptive UTF-8; path base N/A. | Scalar replace. | N/A. | No processing semantics. | Core |
| `distros.<distro>.versions.<version>.release-ver` | string | Required in the composed version; no default. | Non-empty, no C0 controls; path base N/A. When used by a source URI template it also contains no `/` or `\`. | Scalar replace. | N/A. | Missing or empty is an error; a separator-bearing source-template value is rejected before encoding. | Core |
| `distros.<distro>.versions.<version>.dist-git-branch` | string selector | Conditionally required; no default. | Non-empty Git branch name; path base N/A. | Scalar replace. | N/A. | Required to refresh an upstream pin from branch/snapshot selectors. It is not a final identity. | Core |
| `distros.<distro>.versions.<version>.default-component-config` | Closed `ComponentConfig` table | Optional; no default. | Field-specific path bases. | Recursive component-config composition. | Lowest-precedence provider only when this exact version was selected by the immutable target input at `RM-TARGET`. | Partial values are allowed. Source references inside this table cannot reselect the provider. | Core |
| `distros.<distro>.versions.<version>.mock-config` | string path | Optional; no default. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document; target is a regular file. | Scalar replace. | N/A. | Mutually exclusive at a selected architecture with its architecture-specific replacement. | RPM-build profile |
| `distros.<distro>.versions.<version>.mock-config-x86_64` | string path | Optional; no default. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document; target is a regular file. | Scalar replace. | N/A. | Selected only for `x86_64`; missing selected config is an RPM-build error. | RPM-build profile |
| `distros.<distro>.versions.<version>.mock-config-aarch64` | string path | Optional; no default. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document; target is a regular file. | Scalar replace. | N/A. | Selected only for `aarch64`; missing selected config is an RPM-build error. | RPM-build profile |
| `distros.<distro>.versions.<version>.kiwi-config-override` | string path | Optional; no default. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document; target is a regular file. | Scalar replace. | N/A. | Consumed only by the image profile. | Image profile |
| `distros.<distro>.versions.<version>.inputs` | Closed table | Optional; defaults to empty lists. | References RPM resources; path base N/A. | Recursive table; each list replaces. | N/A. | Defined in [Resources and repository inputs](./resources.md#distro-version-inputs); presence alone activates no operation. | Shared RPM-build/image profile data |

The nested `default-component-config` uses the reusable field contracts in
[Components](./components.md), [Sources](./sources.md),
[Build and release](./build.md), [Packages](./packages.md),
[Tests](./tests.md), and [Overlay integration](./overlays.md). Nesting does not
change a field's profile disposition.

[Return to index](./index.md)

# Image profile

`images` is a name-keyed map of closed `ImageConfig` tables. Entries are
validated and preserved as optional image-profile data; an image operation is
selected only under [the common profile rules](./profiles.md). This profile
describes image inputs and metadata; it does not claim reproducible final image
bytes. `image-build` and `publish-image` have separate explicit operation
inputs and neither operation is selected by data presence.

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `images.<image>.description` | string | Optional; no default. | Diagnostic UTF-8; path base N/A. | Scalar replace. | N/A. | No processing semantics. | Image profile |
| `images.<image>.definition` | Closed table | Required in an effective image; no default. | N/A. | Recursive table. | N/A. | Must contain valid `type` and `path`. | Image profile |
| `images.<image>.definition.type` | string enum | Required; no default. | Exactly `kiwi`; path base N/A. | Scalar replace. | N/A. | Unsupported image definition is an error. | Image profile |
| `images.<image>.definition.path` | string path | Required; no default. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document; target is a regular file. | Scalar replace retaining provenance. | N/A. | Missing/non-regular target is an error. | Image profile |
| `images.<image>.definition.profile` | string | Optional; no default. | Non-empty when present; path base N/A. | Scalar replace. | N/A. | Selects a profile inside the definition; missing named profile is an error. | Image profile |
| `images.<image>.architectures` | Array of strings | Optional; default `["x86_64", "aarch64"]`. | Unique values from `x86_64`, `aarch64`; path base N/A. | Array replace; `[]` is invalid. | N/A. | Build request architecture must be listed. | Image profile |
| `images.<image>.capabilities` | Closed table | Optional; no default table. | N/A. | Recursive table. | N/A. | Unspecified booleans remain unknown, not false, for capability matching. | Image profile |
| `images.<image>.tests` | Closed table | Optional; no default. | N/A. | Recursive table; array replaces. | N/A. | References resolve under the test profile. | Image plus test profiles |
| `images.<image>.publish` | Closed table | Optional; no default. | N/A. | Recursive table. | N/A. | Presence selects image-publishing data participation but does not select a publish operation. | Image profile |
| `images.<image>.publish.channels` | Array of strings | Optional; default `[]`. | Unique channel names using the publishing channel grammar. | Array replace. | N/A. | Empty means no publishing; duplicates are errors. | Image profile |

## Capability fields

The capability fields are optional booleans with no default. Each uses scalar
replacement, has no path base or inheritance, and is part of the image profile:

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `images.<image>.capabilities.{machine-bootable,container,systemd,runtime-package-management,wsl,installer-media,fips-enabled,cvm}` | boolean | Optional; no default (unknown). | Path base N/A. | Scalar replace, including explicit `false`. | N/A. | Capability matching requires explicit `true`; unknown and `false` do not satisfy a requirement. | Image profile |

| Field | Meaning and invariants |
| --- | --- |
| `machine-bootable` | Image can boot on bare metal or a virtual machine. |
| `container` | Image can run on an OCI container host. |
| `systemd` | Image runs systemd as init. |
| `runtime-package-management` | Packages can be installed or removed after deployment. |
| `wsl` | Image targets the Windows Subsystem for Linux runtime. |
| `installer-media` | Image installs another system rather than being the end-state system. |
| `fips-enabled` | Image is configured to operate in FIPS mode. |
| `cvm` | Image supports confidential-VM execution. |

A test requiring a capability is compatible only when that capability is
explicitly `true`. Unknown and explicit `false` both fail the requirement.
This specification does not infer one capability from another.

## Image test references

`images.<image>.tests.tests` is a replacing array of the reusable closed
`TestRef` objects in [Tests](./tests.md#test-references). Missing, duplicate, or
capability-incompatible references are errors.

`distros.<d>.versions.<v>.kiwi-config-override` uses the portable relative
model-path grammar and requires a regular-file target. It is an image-profile
input and retains its defining-document provenance.

Final image bytes, image-tool logs, and publication replication are not
conformance outputs. The selected operation still validates exact architecture,
toolchain, repository, network, credential-scope, and resource-limit inputs
under [Profiles](./profiles.md#selected-operation-contracts).

For `publish-image`, `images.<image>.publish.channels` is the exact attempt
input order after duplicate validation; it is not an unordered set. Every
channel is supplied with the same exact image identity and digest to the
selected publishing implementation.

Revision `0.1` does not standardize the remote publication protocol, replay,
retry, receipt, transaction, or rollback behavior. A publication error does
not imply that a remote system reversed any prior effect.

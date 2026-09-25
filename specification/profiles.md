[Return to index](./index.md)

# Optional profile data and operations

All profile fields in this revision are canonical vocabulary. Their presence
selects **data participation**, not execution: every processor that accepts a
source document MUST validate known profile data at the normal `SD-*`,
`CM-*`, and applicable `RM-*` boundaries and MUST preserve its typed value and
provenance in any model it emits. It MUST NOT silently drop, reinterpret, or
execute that data merely because it does or does not implement the associated
operation.

A profile operation is selected only by an explicit implementation interface
request. Command names and transport are not normative. One request contains:

- one exact operation identifier from this chapter;
- the exact target object or artifact identities;
- every required target distro/version and architecture input;
- the immutable
  [environmental-input record](./determinism.md#environmental-input-record);
- the applicable [security](./security.md) context, including only opaque
  credential references; and
- the requested observable result boundary.

Before side effects, a processor MUST either select all profile capabilities
required by that operation and validate their operation-time preconditions, or
return `unsupported-operation`. Unselected profile data remains validated and
preserved but has no operational effect. A selected operation cannot infer
another operation: building an image does not publish it, building an RPM does
not publish it, and materializing a component does not run tests.

Core processing consists of document loading and composition, resolution,
component construction, source acquisition, overlay application, and
materialized dist-git production. Those boundaries require no optional profile
operation identifier. RPM build, images, tests, publishing, and executable
custom-source generation are optional profiles. Repository resources are
shared optional data for RPM-build and image operations. Tool configuration is
tool-specific and outside the portable source-document vocabulary.

| Data family | Presence trigger | Validation and preservation | Operations that may select it | Unsupported selected operation |
| --- | --- | --- | --- | --- |
| RPM-build fields | Any canonical `build`, `release`, legacy distro repo, mock-config, or RPM-build input field | Validate and preserve as RPM-build profile data. | `rpm-build`. Release calculation is an internal pre-build step of this operation, not another selected operation. | Error before build setup or source mutation. |
| Publishing fields | Any canonical package group, package/default publish field, component publish field, or image publish field | Validate and preserve as publishing data; image publish remains part of the image profile. | `publish-packages` or `publish-image`. | Error before credentials, upload, or repository mutation. |
| Image fields | Any canonical `images` entry or image-only distro field | Validate and preserve as image profile data. | `image-build` or `publish-image`. | Error before image-tool invocation. |
| Test fields | Any canonical `tests`, `test-groups`, component test reference, or image test reference | Validate and preserve as test profile data; image references require both image and test support when executed. | `test`. | Error before runner installation, VM creation, or test execution. |
| Custom source generation | `source-files[].origin.type = "custom"` and its variant fields | Validate and preserve the custom-source-generation profile data; core may consume a `custom-result` instead. | `custom-source-generate`. | Error before script execution or build-root creation. |
| RPM repository resources | Any `resources` field or distro-version resource reference | Validate and preserve as shared RPM-build/image profile data; presence makes no operation mandatory. | `rpm-build` or `image-build` when it selects a resource reference. | If the selected operation cannot implement the referenced resource contract, error before repository access. |

The independent [field/profile matrix](./profile-matrix.tsv) maps the frozen
schema inventory and the auxiliary opaque-framework inventory to these
families. Its third column is `applicability`, not an operation selector.
Values containing operation identifiers use only the six exact identifiers in
this chapter, separated by `|` when several apply. `core-processing` and
`none` are non-executable applicability labels. Applicability means only
structural participation in the named operation's model or input context. It
cannot select or require a capability or independently cause
`unsupported-operation`. Matrix patterns use exact path text plus `*` as the
only wildcard; rows are evaluated top to bottom and the final `*` row is the
core default.
The shared resource rows are optional profile data rather than core-processing
rows; they apply to both `rpm-build` and `image-build`.

## Selected operation contracts

The selected-profile operation namespace contains exactly these six values and
no aliases: `rpm-build`, `publish-packages`, `image-build`, `publish-image`,
`test`, and `custom-source-generate`. Core processing, including
`MT-MATERIALIZE`, is selected through its processing boundary and uses no
profile operation identifier.

| Operation identifier | Required explicit operation inputs | Deterministic boundary in this revision | Excluded result |
| --- | --- | --- | --- |
| `rpm-build` | Exact resolved component, materialized tree identity, target distro/version, target architecture, complete RPM macro context, selected repository references, immutable repository/package manifest including every required repository GPG-key binding, immutable toolchain manifest, evaluation instant, network context, resource limits, and any required credential references. | Capability selection, input validation, repository expansion, GPG-key byte verification before repository use, macro-context construction, and structured success/error reporting. | Final RPM/SRPM bytes, build logs, timestamps, and build reproducibility are not conformance outputs. |
| `publish-packages` | Exact ordered artifact identities and digests, evaluated publishing routes, destination identities, network context, resource limits, and route-scoped credential references. | Capability selection, complete input/security preflight, preservation of the requested artifact/route order, and a typed success or error result from the selected implementation. | Publication wire protocol, retries, replay, receipts, remote transaction semantics, retention, replication timing, repository metadata bytes, and package build reproducibility. |
| `image-build` | Exact image identity, target distro/version, one architecture listed by the image, selected repository references, immutable component/artifact and repository/package manifests including every required repository GPG-key binding, immutable toolchain manifest, evaluation instant, network context, and resource limits. | Capability selection, reference/resource expansion, GPG-key byte verification before repository use, input validation, and structured success/error reporting. | Final image bytes and image reproducibility. |
| `publish-image` | Exact image artifact identity and digest, exact ordered channels, destination identity, network context, resource limits, and route-scoped credential references. | Capability selection, complete input/security preflight, preservation of the requested channel order, and a typed success or error result from the selected implementation. | Publication wire protocol, retries, replay, receipts, remote transaction semantics, retention, replication timing, and final image reproducibility. |
| `test` | Exact test or group selection, resolved component or image identity, required capabilities, architecture when the selected runner needs one, immutable runner/toolchain manifest, network context, resource limits, and permitted credential references. | Reference expansion, capability matching, runner selection, and structured invocation/result classification. | Test timing, performance measurements, external service state, and a universal test-result byte format. |
| `custom-source-generate` | Exact `custom` source entry, snapshotted script, target architecture matching `[A-Za-z0-9][A-Za-z0-9._+-]*`, evaluation instant, declared input artifact bytes, declared mock-package names, implementation-specific isolation availability, and resource limits. | Declared-input/package availability, isolated execution, semantic output-tree validation, archive/hash boundary, and exact accepted artifact digest defined in [Sources](./sources.md#custom-source-generation-profile). | Portable execution-root or package-payload closure, kernel-observation proofs, final RPM/image bytes, and portable archive-byte conformance. |

An operation MUST NOT begin a network request, load a credential, create a
build root, execute a child process, or mutate a destination until all
required inputs in its row have passed `V-OPERATION`. Missing architecture,
macro, package, toolchain, network, credential-scope, or resource-limit input
is not filled from the host.

For `custom-source-generate`, the target architecture is an exact nonempty
ASCII operation input matching `[A-Za-z0-9][A-Za-z0-9._+-]*`. It MUST NOT be
normalized, defaulted, or inferred from the host.

A selected `rpm-build` or `image-build` operation that references an effective
repository set with `disable-ssl-verify = true` fails `security-policy` at
`V-OPERATION`. This preflight occurs before repository expansion or access and
does not weaken certificate-chain or hostname verification.

Selecting `rpm-build` or `image-build` may select exact repository resource
references. Resource-data presence alone does not activate either operation.

## Publishing boundary

Revision `0.1` standardizes profile-data resolution, selected-operation
identifiers, explicit input/security preflight, and the order supplied to the
selected publishing implementation. It does not standardize a publication wire
protocol, request/response transcript, replay behavior, retry algorithm,
receipt format, idempotency protocol, remote transaction protocol, rollback
proof, or transport simulator.

A publishing error is terminal for the selected operation and produces no
success result. Remote systems may nevertheless have effects that this
specification cannot observe or reverse. A processor MUST NOT describe those
effects as atomically rolled back under this specification. Implementations
may expose additional operational reports, but those reports are outside the
portable result boundary and cannot contain credential values.

## Status and output exclusions

No conformance class is enabled or claimed. The operation-selection contract
does not create a core or profile claim.

The explicit input and deterministic-boundary rows above are normative, while
every excluded result remains outside revision `0.1`. The ordinary fixture
format intentionally exposes no selected-profile operation transport.
The tracked [profile-behavior fixture](./examples/profile-behavior/README.md)
is instead a closed set of representative validation and preflight scenarios:
its operation and outcome select exact fixture-only key/type sets, and
unknown, missing, wrong-type, cross-operation, or cross-scenario fields are
errors.

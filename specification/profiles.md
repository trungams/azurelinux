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
| `publish-packages` | Exact ordered artifact identities and digests, evaluated publishing routes, destination identities and capabilities, network context, resource limits, route-scoped credential references, and publication idempotency identities. | Complete preflight, exact attempt order, request/idempotency association, receipt ledger, and explicit partial-remote-effect reporting. Remote all-or-none behavior exists only with an explicit destination transaction capability. | Remote retention, replication timing, repository metadata bytes, and package build reproducibility. |
| `image-build` | Exact image identity, target distro/version, one architecture listed by the image, selected repository references, immutable component/artifact and repository/package manifests including every required repository GPG-key binding, immutable toolchain manifest, evaluation instant, network context, and resource limits. | Capability selection, reference/resource expansion, GPG-key byte verification before repository use, input validation, and structured success/error reporting. | Final image bytes and image reproducibility. |
| `publish-image` | Exact image artifact identity and digest, exact ordered channels, destination identity and capabilities, network context, resource limits, route-scoped credential references, and publication idempotency identities. | Complete preflight, exact attempt order, request/idempotency association, receipt ledger, and explicit partial-remote-effect reporting. Remote all-or-none behavior exists only with an explicit destination transaction capability. | Remote retention, replication timing, and final image reproducibility. |
| `test` | Exact test or group selection, resolved component or image identity, required capabilities, architecture when the selected runner needs one, immutable runner/toolchain manifest, network context, resource limits, and permitted credential references. | Reference expansion, capability matching, runner selection, and structured invocation/result classification. | Test timing, performance measurements, external service state, and a universal test-result byte format. |
| `custom-source-generate` | Exact `custom` source entry, snapshotted script, target architecture, evaluation instant, declared input artifact bytes, declared mock-package names, implementation-specific isolation availability, and resource limits. | Declared-input/package availability, isolated execution, semantic output-tree validation, archive/hash boundary, and exact accepted artifact digest defined in [Sources](./sources.md#custom-source-generation-profile). | Portable execution-root or package-payload closure, kernel-observation proofs, final RPM/image bytes, and portable archive-producer conformance. |

An operation MUST NOT begin a network request, load a credential, create a
build root, execute a child process, or mutate a destination until all
required inputs in its row have passed `V-OPERATION`. Missing architecture,
macro, package, toolchain, network, credential-scope, or resource-limit input
is not filled from the host.

A selected `rpm-build` or `image-build` operation that references an effective
repository set with `disable-ssl-verify = true` fails `security-policy` at
`V-OPERATION`. This preflight occurs before repository expansion or access and
does not weaken certificate-chain or hostname verification.

Selecting `rpm-build` or `image-build` may select exact repository resource
references. Resource-data presence alone does not activate either operation.

## Remote publication attempts

`publish-packages` and `publish-image` bind one immutable ordered publication
plan before the first remote side effect. Every item contains:

- its zero-based ordinal;
- exact artifact identity and digest;
- exact route or image channel and destination identity;
- the route-scoped credential reference, if any;
- the destination capability record; and
- a semantic idempotency identity equal to the tuple `(operation identifier,
  destination identity, route or channel, artifact identity, artifact digest)`.

The destination capability record states whether it supports one atomic
transaction covering the complete ordered plan. Capability absence means
unsupported, not an inferred default. A semantic idempotency identity remains
part of the request and receipt evidence, but specification revision `0.1`
never uses it to authorize another attempt.

All items, credentials, request bodies, limits, and destination capabilities
are validated at `V-OPERATION` before any request. The one resource-limit
field `request-attempts` MUST equal `1`. Each plan item is therefore attempted
at most once in increasing ordinal order, and its receipt record has attempt
count `1`; an unattempted later item has count `0`. Processing stops after the
first result other than confirmed acceptance. Later items are recorded as
`not-attempted`. An ambiguous transport result is final even when the
destination supports idempotent replay. No retry request, delayed replay, or
second transaction submission is conforming in revision `0.1`.

The credential-free receipt ledger has one record per plan item in ordinal
order. Each record contains the item identities, attempt count, idempotency
identity, and exactly one outcome:

- `not-attempted`;
- `accepted`, with a non-secret destination receipt identity;
- `rejected`, with the typed authorization, policy, or destination error;
- `transport-unknown`, when the processor cannot determine whether the remote
  side effect occurred; or
- `rolled-back`, with a transaction receipt proving that a previously accepted
  item is no longer effective.

The failed operation additionally reports `partial-remote-effect` as `none`,
`confirmed`, `possible`, or `confirmed-and-possible`. An accepted item outside
a successfully aborted transaction is confirmed; `transport-unknown` is
possible. The processor MUST NOT report remote rollback merely because the
local request failed.

When the selected operation supplies an explicit complete-plan transaction
capability, the processor begins that transaction before the first item and
commits only after every item is accepted. On failure it requests abort.
`partial-remote-effect = none` and `rolled-back` outcomes are permitted only
when a destination receipt confirms that abort or rollback removed every
effect. An unsupported, failed, or ambiguous abort is reported using the same
partial-effect rules; it is not converted into local atomicity.

## Claims and outputs

All conformance classes remain forward-declared and non-claimable. This
operation-selection contract closes behavioral ambiguity without enabling a
core or profile claim.

When claims are eventually enabled, a profile claim is attached to an enabled
class and operation; it is not a standalone declaration that a tool "supports"
the profile. The claim records every explicit input above and excludes results
not named as deterministic boundaries. The lifecycle and fixture requirements
are defined in [Conformance methodology](./conformance.md#profile-claim-scope).

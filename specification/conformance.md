[Return to index](./index.md)

# Conformance methodology

This chapter defines fixture packages, evidence comparison, profile claim
scope, and claim lifecycle. It does not enable a conformance class by itself.
The authoritative class status remains the table in
[Conformance](./index.md#conformance).

## Fixture package

A conformance fixture package is a project-contained directory with one
`manifest.toml` using format version `1`. Every referenced input, class-transport document, environmental context
document, expected byte file, and expected report is
beneath that directory and is named by a portable relative model path.

The manifest root is closed and contains:

| Field | Requirement |
| --- | --- |
| `format-version` | Integer `1`. |
| `fixture-id` | Stable non-empty ASCII token using lowercase letters, digits, and `-`. |
| `spec-version` | Exact specification version under test. |
| `description` | Non-normative UTF-8 text. |
| `claimable` | Boolean. `false` is required when any listed blocker affects the case. |
| `blocked-by` | Ordered unique array of exact open-decision or finding identifiers; default `[]`. |
| `suite-id` | Optional stable suite token using the `fixture-id` grammar. |
| `suite-version` | Optional non-empty ASCII token using letters, digits, `.`, `_`, and `-`. |
| `suite-classes` | Optional ordered unique non-empty array of class names from the index. |
| `inputs` | Array of closed path/digest records for every referenced fixture file. |
| `cases` | Ordered non-empty array of closed case tables. |

`suite-id`, `suite-version`, and `suite-classes` are either all absent or all
present. Presence identifies suite membership; it does not enable a class.
The representative package in this revision names a draft suite that is
enumerated by the tracked registry. That registry and package both remain
non-claimable, and the authoritative index names no required suite while every
class is `draft`.

An input record contains exact `path` and lowercase 64-digit `sha256`. A
fixture runner verifies all input digests before processing a case. Symlinks,
devices, sockets, FIFOs, path escapes, undeclared files consumed by a case, and
digest mismatches are `conformance` errors.

Every external fixture-file reference has the exact form `input:<path>`, where
`<path>` names exactly one root `inputs` record. A path nested in a referenced
closed manifest follows the same rule. Semantic model paths, URI paths, and
Materialized-tree entry paths are values inside a digest-declared input and
are not external fixture-file references. A runner MUST NOT guess a file from
its name, consume every package file, or make an undeclared package-wide input
available to a case.

Each case contains:

- `id`, unique within the manifest;
- `kind`, exactly `positive`, `negative`, `resolved`, or `output`;
- `class`, one class name from the index;
- `role`, exactly `producer`, `consumer`, or `both`;
- `profile`, `core` or one named profile;
- `claimable` and `blocked-by`, under the exact narrowing rule below;
- `operation`, using one of the six exact identifiers in Profiles or `none`;
- optional `processing-boundary`, required only when `operation = "none"` and
  the case invokes a later core boundary such as `MT-MATERIALIZE`;
- `case-input`, one closed class-discriminated table defined below;
- `environment`, a closed environmental-input record subset;
- optional `environment-assertions`, which describes fixed, prohibited,
  irrelevant, external-failure, or inapplicable matrix rows without inserting
  assertion sentinels into the operation input;
- optional `expected-output-manifest`, an `input:<path>` reference permitted
  only for an `output` case; and
- `expected-outcome`, exactly `success` or `error`; and
- one or more expectation tables appropriate to `kind`.

The root `blocked-by` array defines the only blockers available to its cases.
A case `blocked-by` value MUST be an ordered subsequence of the root array:
every case identifier occurs at the root, appears at most once, and retains
root relative order. A case cannot add or reorder a blocker. A case may set
`claimable = false` even when the root is claimable; it may set
`claimable = true` only when the root is claimable and its own `blocked-by`
array is empty.

The tracked [fixture-format example](./examples/conformance/README.md) is
representative, not an enabled normative suite.

### Class-discriminated case input

`case-input` is required exactly once per case. It has a required `class`
discriminator equal to the containing case `class`; unknown keys and keys from
another class variant are `conformance` errors. Its four closed variants are:

| `class` | Exact remaining keys and requirements |
| --- | --- |
| `Source-document` | `document-role` is exactly `root` or `fragment`; `project-root`, `document-target`, and `document-bytes` are three required `input:<path>` references. The root input describes the canonical fixture project-root snapshot, the target input describes the canonical project-relative target and role, and the bytes input supplies the exact UTF-8 document bytes. The target's nested bytes reference MUST equal `document-bytes`, and its role MUST equal `document-role`. |
| `Loading/composed-model` | `project-root` and `root-document` are required `input:<path>` references. The root input is the complete project-root snapshot and the root-document input is its exact root target descriptor. The root snapshot MUST name that same root-document reference. |
| `Resolved-model` | `model-input-kind` is exactly `composed-model` or `prior-class-output`; `model-input` is one required `input:<path>` reference of that declared kind; and `environment-binding` is exactly `case-environment`. The complete containing `environment`, including the required target distro/version pair and every applicable resolution input, is consumed as part of this class input. A prior-class output is a digest-declared fixture transport envelope, not a standardized resolved-model serialization. |
| `Materialized-tree` | `model-input-kind` is exactly `resolved-model` or `prior-class-output`; `model-input` is one required `input:<path>` reference; `environment-binding` is exactly `case-environment`; and `acquisition-inputs` and `transformation-inputs` are required non-empty ordered unique arrays of `input:<path>` references. Those referenced closed manifests completely enumerate the acquired tree/artifact bytes and transformation declarations consumed by the case. |

The Source-document and Loading variants bind bytes and locations rather than
relying on package layout. The Resolved-model and Materialized-tree variants
bind either the immediately preceding semantic input or an explicitly
digest-declared prior-class output; a runner cannot substitute an in-memory
producer value that was not declared. Every referenced transport is parsed by
its `format-version` and `document-kind`, never by its filename. Every root
input record MUST be reached by at least one case field, environmental
reference, nested closed-document reference, expected-output reference, or
suite package reference. An unconsumed package input is a `conformance` error
because it could otherwise become a guessed ambient input.

### Version-1 class-transport documents

Every class-transport document is a closed TOML document with integer
`format-version = 1` and one exact `document-kind` discriminator. Unknown root
or nested keys, a wrong TOML type, an unsupported format version or kind, an
undeclared `input:<path>` reference, or a kind that does not match the
referring `case-input` field is a `conformance` error. The version-1 kinds are:

| `document-kind` | Exact remaining root keys |
| --- | --- |
| `document-target` | `document-role`, `project-relative-path`, `bytes` |
| `project-root` | `root-id`, `root-document`, `documents` |
| `composed-model` | `output-class`, `project-root`, `root-document`, `ordered-document-reaches` |
| `resolved-model` | `output-class`, `input-kind`, `input`, `target-distro`, `target-version` |
| `prior-class-output` | `output-class`, `payload-kind`, `payload`, `producer-case-id` |
| `acquisition-inputs` | `entries` |
| `transformation-inputs` | `operations` |

`document-target` has `document-role` exactly `root` or `fragment`,
`project-relative-path` satisfying the portable relative model-path grammar,
and `bytes` as one `input:<path>` reference to the exact source-document
bytes. A `root` target has path `azldev.toml`; a fragment target MUST NOT have
that path.

`project-root` has `root-id` using the fixture-ID grammar, `root-document` as
one `input:<path>` reference, and `documents` as an ordered non-empty array of
unique `input:<path>` references. Every referenced document MUST have kind
`document-target`; their project-relative paths are unique; exactly one has
role `root`; and `root-document` equals that reference and occurs first in
`documents`.

`composed-model` has `output-class = "Loading/composed-model"`,
`project-root` referencing one `project-root` document, `root-document`
referencing its exact root `document-target`, and
`ordered-document-reaches` as a non-empty ordered array of
`document-target` references. The first reach is the root document and every
reach occurs in the referenced project's `documents`. Each canonical document
appears exactly once at its first depth-first evaluation position; later
incoming reaches do not repeat its body in this sequence.

`resolved-model` has `output-class = "Resolved-model"`, non-empty
`target-distro` and `target-version`, and one closed input union.
`input-kind = "composed-model"` requires `input` to reference a
`composed-model` document. `input-kind = "prior-class-output"` requires
`input` to reference a `prior-class-output` document whose `output-class` is
`Loading/composed-model`.

`prior-class-output` is a transport envelope, not a standardized
resolved-model serialization. `producer-case-id` uses the fixture-ID grammar.
Its closed mapping is:

| `output-class` | Required `payload-kind` | Required payload document |
| --- | --- | --- |
| `Loading/composed-model` | `composed-model` | one `composed-model` reference |
| `Resolved-model` | `resolved-model` | one `resolved-model` reference |

The named producer case MUST exist in the same digest-verified package, have
that class, and have `expected-outcome = "success"`.

`acquisition-inputs.entries` is a non-empty ordered array with unique portable
relative paths. Each entry is one closed `type`-discriminated table:

| `type` | Exact keys in addition to `path` and `type` |
| --- | --- |
| `directory` | `mode` |
| `file` | `mode`, `content-hex`, `sha256` |
| `symlink` | `target` |

`mode` is a four-digit octal string. `content-hex` has even length and
lowercase hexadecimal only, and its decoded bytes MUST have the listed
lowercase SHA-256. `target` satisfies the portable symbolic-link target
grammar. A non-directory cannot be an ancestor of another entry; every
non-root parent either occurs earlier as a directory or is an allowed implicit
directory under the Materialized-tree contract.

`transformation-inputs.operations` is an ordered array and may be empty. Each
record has exactly integer `ordinal`, string `operation`, string `model-path`,
and array `input-references`. Ordinals are contiguous from zero. `operation`
is one of the 17 exact operation identifiers in the overlay-operation matrix;
`model-path` is a non-empty semantic path to that resolved declaration; and
`input-references` is an ordered unique array of `input:<path>` references to
every external payload consumed by that declaration. The resolved declaration
at `model-path` MUST have the same operation discriminator. This manifest
records exact transformation inputs and does not define a second overlay
schema.

### Environmental record encoding

The case `environment` table is closed. Absence is represented only by
omitting a key. Strings such as `absent`, `ignored`, `prohibited`, or
`not-consulted` have no sentinel meaning and are ordinary invalid values when
the owning field grammar does not permit them.

The exact keys and TOML types are:

| Key | TOML type and meaning |
| --- | --- |
| `target-distro`, `target-version` | Non-empty strings that MUST occur together and bind `ENV-TARGET-DISTRO-VERSION`. |
| `architecture` | Non-empty string binding `ENV-TARGET-ARCHITECTURE`. |
| `evaluation-instant` | TOML offset date-time in UTC binding `ENV-EVALUATION-INSTANT`. |
| `network-context` | `input:<path>` reference to one digest-declared closed version-1 network-context file defined below. |
| `resource-limits` | `input:<path>` reference to one digest-declared closed version-1 resource-limit document defined below. |
| `credential-references` | Ordered unique array of non-secret `credential-ref:<token>` strings; an empty array means the selected operation receives no credential reference. |
| `rpm-macro-context` | `input:<path>` reference to a digest-declared closed version-1 RPM macro context defined below. |
| `toolchain-manifest` | `input:<path>` reference to a digest-declared closed version-1 immutable toolchain manifest defined below. |
| `package-manifest` | `input:<path>` reference to a digest-declared closed version-1 package manifest. |

An `input:<path>` reference uses the portable relative model-path grammar
after the literal `input:` prefix and MUST name exactly one root `inputs`
record. The referenced file's digest is verified before the case is bound.
Unknown environment keys, wrong TOML types, undeclared references, duplicate
credential references, or keys inapplicable to the selected operation and
processing boundary are `conformance` errors.

`rpm-build` requires the target pair, architecture, evaluation instant, RPM
macro context, toolchain manifest, package manifest, network context, resource
limits, and the credential-reference array. The architecture and target pair
MUST equal their values in the RPM macro context, and the architecture MUST
equal the toolchain and package manifest architectures. `image-build` requires
the target pair, architecture, evaluation instant, toolchain manifest, package
manifest, network context, resource limits, and credential-reference array;
its architecture MUST equal both manifest architectures.
`publish-packages` and `publish-image` require evaluation instant, network
context, resource limits, and the credential-reference array.
`custom-source-generate` requires architecture, evaluation instant, resource
limits, and the empty credential-reference array required by this revision;
its resolved `custom` source entry supplies declared input filenames and mock
package names. `test`
binds the exact applicable subset named by its runner contract and includes
evaluation instant whenever it includes network context. With
`operation = "none"`, only values consumed by the exact
`processing-boundary` or expectation kind are permitted.

#### Version-1 environmental context documents

The four common context documents below are closed TOML documents. Each has
integer `format-version = 1` and the exact `document-kind` named here.

`document-kind = "resource-limits"` has exactly these additional integer keys:
`input-bytes`, `expanded-bytes`, `entry-count`, `file-bytes`, `path-bytes`,
`redirects`, `request-attempts`, `process-count`, `memory-bytes`, and
`execution-milliseconds`. Boolean values are not integers for this format.
Every value is non-negative and has the byte, count, or millisecond unit named
by its key. In specification revision `0.1`, `request-attempts` MUST equal
`1`; this document is the sole owner of request and remote-publication attempt
count. A process-using operation requires non-zero process, memory, and
execution limits.

`document-kind = "rpm-macro-context"` has exactly `architecture`,
`target-distro`, `target-version`, `undefined`, `undefines`, `base`, and
`defines`. The first three are non-empty strings. `undefined` and `undefines`
are ordered unique arrays of non-empty macro-name strings. `base` and
`defines` are string-to-string tables; dynamic macro names are the only keys
permitted in those two tables. No name occurs in both `base` and `undefined`,
and no name occurs in both `defines` and `undefines`. The root architecture
and target pair equal the containing environment, `base` is the complete base
map, `undefined` is the complete base undefined-name set, and `defines` and
`undefines` are the exact applied component inputs.

`document-kind = "toolchain-manifest"` has exactly `manifest-id`,
`architecture`, and non-empty ordered `entries`. `manifest-id` uses the
fixture-ID grammar and `architecture` is non-empty. Entry paths are unique and
UTF-8-byte ordered absolute sandbox paths. Each entry uses the same closed
`directory`/`file`/`symlink` key variants, mode grammar, digest rule, and
symlink-target rule as `acquisition-inputs`.

`document-kind = "package-manifest"` has exactly `manifest-id`,
`architecture`, and non-empty ordered `packages`. Each closed package record
has exactly non-empty `key`, non-empty immutable `identity`, `architecture`,
and lowercase 64-digit `content-sha256`. Keys and identities are each unique,
and every record architecture equals the manifest and containing environment
architecture.

For any of these documents, a missing, extra, wrong-type, duplicate,
out-of-order, mismatched, or operation-inapplicable value is a `conformance`
error before the selected operation begins.

#### Version-1 network-context file

The referenced network-context file is a closed TOML document. Its exact
keys are:

| Key | TOML type | Required value |
| --- | --- | --- |
| `format-version` | integer | Exactly `1`. |
| `document-kind` | string | Exactly `network-context`. |
| `trust-policy-id` | string | Non-empty lowercase ASCII token matching `[a-z0-9][a-z0-9._-]*`. |
| `trust-material` | string | One `input:<path>` reference to the complete trust-material bytes. |
| `trust-material-sha256` | string | Lowercase 64-digit SHA-256 of those exact bytes; it MUST equal the referenced root input record and the verified file bytes. |
| `proxy` | string | Exactly `none` or one canonical HTTPS origin under the Sources authority/origin grammar. An origin value selects that proxy only; host proxy variables are forbidden. |
| `connect-timeout-ms` | integer | Required and non-negative, in milliseconds. |
| `idle-read-timeout-ms` | integer | Required and non-negative, in milliseconds. |
| `overall-timeout-ms` | integer | Required and non-negative, in milliseconds; it MUST be greater than or equal to each component timeout. |

The file contains no evaluation instant. `ENV-EVALUATION-INSTANT` is bound
only by the containing case `environment.evaluation-instant`; the network
context consumes that value for TLS and credential validity and cannot
override or duplicate it.

No other key is permitted. Request-attempt count is absent because the
referenced resource-limit document is its sole owner. Wrong types, an adapter
or replay mode key, a non-canonical proxy origin, or duplicated evaluation
time is a `conformance` error before network access. This context freezes the
inputs that affect request construction and validation; it does not encode
external requests or responses.

Each `environment-assertions` entry is a closed table containing `row-id` and
`expectation`. `row-id` names one `row_id` in the environmental matrix.
`expectation` is exactly `explicit-bound`, `fixed-rule`,
`prohibited-unobserved`, `irrelevant-no-effect`, `external-failure-only`, or
`not-applicable`. These records are expected observations, never operation
inputs.

## Expectation kinds

### Positive expectations

A `positive` case identifies required successful phase boundaries and semantic
facts such as selected operation, document reach order, reference target,
acquisition source, or number of produced entries. It MUST NOT treat absence
of an expected diagnostic as proof of an output not otherwise checked.

### Negative expectations

A `negative` case expects no result from the failed boundary and contains at
least one expected diagnostic with:

- `class`;
- `validation-phase`;
- `processing-phase`;
- `requirement`; and
- `subject`.

Exact message text, absolute path, stack trace, timestamp, and localization are
forbidden expectations. A negative local artifact-publication case identifies prior destination bytes
and verifies they remain unchanged. A negative remote publication case instead
checks the exact attempt/receipt ledger and known, possible, or absent partial
remote effects under the selected publication contract.

### Resolved expectations

A `resolved` case compares selected typed leaves, effective object identities,
array order, source identities, references, and semantic provenance. Each leaf
expectation contains an exact model path, TOML type name, TOML value encoded as
one standalone `value = ...` assignment, and expected supplier/effective-object
semantic identities.

This leaf-predicate format is fixture metadata, not a canonical serialization
of the complete resolved model and does not define a comparison algorithm.
Cases may exercise valid fixed-order inheritance and negative destructive
component-group overlaps.

### Output expectations

An `output` case checks the exact observable output boundary. A materialized
tree expectation lists every entry path, type, mode, regular-file SHA-256 or
exact symlink target. Expected file bytes too large or unsuitable for inline
TOML reside in a digest-declared fixture file.

For transformed archives, the expected complete archive bytes and configured
post-overlay digest are required. Such a case remains `claimable = false`
while the Materialized-tree class is in `draft`, even when one implementation
reproduces the fixture. Reproduction is not evidence of a canonical encoder or
cross-tool archive-byte reproducibility.

RPM and image bytes are not output expectations in this revision. Test
results, publishing receipts, and operational logs likewise are not core
materialized-tree outputs.

## Execution methodology

A fixture runner:

1. validates the closed manifest and every declared digest;
2. binds the class-discriminated `case-input`, then the exact
   environmental-input record, package manifest, toolchain manifest,
   credentials-by-reference, and resource limits, rejecting every undeclared
   or unconsumed package input;
3. ensures no undeclared host input is visible to the claimed boundary;
4. executes the case at least twice from different absolute checkout,
   temporary, and cache paths;
5. for an ambient-state case, repeats the declared locale, timezone, clock,
   architecture, umask, concurrency, and cache variations;
6. compares the expected phase result, structured diagnostics, selected
   resolved predicates, output entries, and semantic provenance requirements;
   and
7. emits a credential-free report containing manifest digest, implementation
   identity, class/role/profile scope, case results, deviations, and exact
   blockers.

A producer and consumer are tested separately. A producer fixture computes the
output. A consumer fixture is given that output without trusting producer
metadata and independently validates every boundary invariant. A `both` claim
must pass both paths; feeding a producer's in-memory representation directly
to its consumer is insufficient.

Representative project validators, schema registries, structural counts, and
single-implementation oracles are supporting evidence. They are not a
portable conformance suite until their fixture cases satisfy this chapter and
the owning class is enabled.

## Profile claim scope

A profile claim is never standalone. It extends one or more enabled
conformance classes and identifies:

- every selected profile operation;
- the named profile data families included;
- all explicit environmental, security, toolchain, package, network, and
  credential-reference inputs;
- the operation's exact deterministic output boundary; and
- every excluded final artifact, such as RPM or image bytes.

Resource validation and preservation do not create an operation claim. An
RPM-build or image operation records the exact shared profile-data references
it selected, but the claim remains about that operation's defined boundary.

Custom-source-generation evidence includes the resolved source entry, declared
input filenames, declared mock-package names, isolation availability, semantic
output tree or file, configured output digest, and archive-byte blocker status.
It does not claim portable sandbox or package-payload closure, kernel
observation equivalence, final RPM bytes, or final image reproducibility.

## Suite registry

A suite registry is one closed `suite-registry.toml` document with integer
`format-version = 1`, a `registry-id` using the fixture-ID grammar,
`spec-version`, boolean `claimable`, and an ordered `suites` array. Unknown
root keys are errors. The registry itself cannot enable a class; the
authoritative index must name a matching suite ID/version while that class has
lifecycle `enabled`.

Each closed suite record contains exactly:

- `suite-id` and `suite-version` under the manifest grammars;
- `claimable`, which MUST be `false` unless every class it covers is
  `enabled` in the index;
- non-empty ordered unique `packages`; and
- non-empty ordered unique `requirements`.

Each package record contains exactly `manifest`, a portable relative path from
the registry, and lowercase 64-digit `sha256` of that complete fixture
manifest. The manifest digest is verified before any case enumeration. Its
`suite-id` and `suite-version` MUST equal the suite record, its
`spec-version` MUST equal the registry `spec-version`, and its
`suite-classes` MUST equal the ordered first occurrence of classes in the
requirements.

Each requirement record contains exactly `class`, `role`, `profile`, and
`case-ids`. `class` is a class from the index; `role` is exactly `producer` or
`consumer`; `profile` is `core` or one named profile; and `case-ids` is a
non-empty ordered unique array. Every ID MUST resolve exactly once among the
digest-verified packages, and its case class and profile MUST equal the
requirement. A case with role `both` may satisfy one producer and one consumer
requirement; a producer-only or consumer-only case cannot satisfy the other
role.

The requirements array is the complete case enumeration for that suite
version. An assessor MUST NOT infer required cases from a directory, accept an
unlisted case as a substitute, omit a listed case, or combine another registry
version. Duplicate `(class, role, profile)` requirement tuples are errors.
For every requirement, the registry and every contributing package
`spec-version` MUST also equal the specification revision listed for that
class in the authoritative index. A suite cannot combine package or class
revisions.
Changing a package digest, required ID, role, profile, or order creates a
different suite definition and invalidates prior reports.

The tracked
[draft suite registry](./examples/conformance/suite-registry.toml) enumerates
one package and all of its current cases by class, role, and profile. Its
registry, suite, package, and cases remain `claimable = false`; it is not named
by the index and enables no claim.

## Claim lifecycle

A conformance scope has exactly one closed lifecycle token from this set:
`draft`, `candidate`, `enabled`, `suspended`, or `withdrawn`.

1. **`draft`**: requirements or fixtures are being written; no claim is allowed.
2. **`candidate`**: the closed input vocabulary, output oracle, positive and
   negative fixtures, environmental matrix coverage, diagnostics, and security
   review are complete, but the index has not enabled the class.
3. **`enabled`**: a normative revision changes the index lifecycle token and
   names one required suite ID and version. Claims may be issued only for
   passing producer/consumer roles and named profiles.
4. **`suspended`**: a blocking defect, open decision, security issue, or failed
   required fixture is discovered. New claims are prohibited and existing
   reports identify the suspension.
5. **`withdrawn`**: the implementation or specification revision no longer
   supports the scope. A withdrawn claim is not silently mapped to another
   version.

The authoritative class table in the index stores the lifecycle token and an
optional required suite ID/version. Required suite fields MUST be absent for
`draft` and `candidate`, MUST be present for `enabled`, and retain the last
applicable value for `suspended` or `withdrawn`. Before a class can become
`enabled`, one digest-verified registry suite record MUST match the named suite
ID/version, every listed package MUST carry that membership, and the complete
requirements enumeration MUST include that class, every claimable role, and
every named profile. Every required case ID MUST resolve through the registry
rules above. A package or directory that is not listed by digest cannot
contribute a required case.

Any change to specification version, required fixture manifest or input
digest, implementation behavior, toolchain/environment input, claimed role,
profile operation, security policy, or documented **SHOULD** deviation
invalidates the prior report and requires re-execution. Passing a subset does
not preserve a broader claim.

## Current revision status

All four classes remain in `draft` status and non-claimable:

| Class | Blocking boundary |
| --- | --- |
| Source-document | The manifest format and representative fixtures exist, but no normative complete source-model fixture-suite version and output oracle are enabled. |
| Loading/composed-model | Repeated canonical-document behavior is defined, but no normative complete composed-model fixture-suite version and output oracle are enabled. |
| Resolved-model | Typed inheritance and behavioral identity are defined, but no normative complete resolved-model fixture-suite version and output oracle are enabled. |
| Materialized-tree | Overlay and artifact behavior is defined, but no normative complete Materialized-tree fixture-suite version and output oracle are enabled. |

Draft boundaries remain visible and non-claimable. A fixture encountering an
actual unresolved blocker lists its exact identifier in `blocked-by`; it MUST
NOT convert the case to success by choosing an alternative silently.

Long-term version compatibility, deprecation timing, and future semantic
overlay profiles remain outside this lifecycle until a later revision defines
them.

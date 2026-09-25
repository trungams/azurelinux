[Return to index](./index.md)

# Validation and diagnostics

This chapter defines when validation occurs, what an error prevents, and the
portable diagnostic information needed by fixtures and interoperable tooling.
It does not standardize exact human wording, terminal formatting, localization,
or a command-line exit code.

## Validation phase order

Validation is progressive but never reparative. A later phase MUST NOT coerce,
drop, default, retry, or reinterpret an invalid earlier-phase value so that the
attempt can continue. The stable processing identifiers in
[Resolution](./resolution.md#processing-and-validation-order) remain the
fine-grained phase namespace.

For diagnostic and conformance grouping, the validation phases are:

| Validation phase | Processing covered | Required boundary |
| --- | --- | --- |
| `V-DOCUMENT` | `LD-ROOT`, `SD-BYTES`, `SD-CONTROLS`, and `SD-MODEL` | Every newly reached document is independently valid before its body can participate in composition. |
| `V-COMPOSE` | `LD-INCLUDES`, `CM-COMPOSE`, and `CM-VALIDATE` | Loading produces one complete reach sequence and composition is atomic. |
| `V-RESOLVE` | `RM-TARGET`, `RM-EFFECTIVE`, `RM-REFERENCES`, `RM-VALIDATE`, and `RM-SOURCE-ID` | One complete self-consistent resolved input exists before acquisition. |
| `V-OPERATION` | Profile capability selection, environmental-input binding, resource-limit binding, and security preflight | Unsupported or unsafe operations fail before credentials, network, subprocess, build root, or output mutation. |
| `V-ACQUIRE` | Git, local filesystem, HTTPS, repository, and declared artifact acquisition | Every selected input has exact identity and verified bytes before candidate insertion. |
| `V-TRANSFORM` | Overlay source snapshots, archive interpretation, operation execution, and custom-source generation inside materialization | All preconditions and every sequential postcondition succeed in private state. |
| `V-ARTIFACT` | Final tree, `sources`, hashes, and provenance ledger | The complete candidate satisfies its owning artifact contracts. |
| `V-PUBLISH` | Local artifact commit and selected-profile publication preflight/result classification | Local artifact observers see the prior complete result or the new complete result. Remote publication validates required inputs before side effects and returns a typed success or error without a standardized transport, replay, receipt, transaction, or rollback protocol. |
| `V-CONFORMANCE` | Ordinary fixture format, declared-input, expectation, and traceability verification | The fixture is a closed union, every declared digest and requirement reference is valid, and no undeclared input is consumed. |

`LD-INCLUDES` is the sole owner of include expansion, canonical target
classification, active-chain cycle detection, and previous-reach
classification. Every diagnostic from that processing phase uses
`V-COMPOSE`; `V-DOCUMENT` does not reclassify or duplicate an include-reach
failure.

Within one validation phase a processor MAY continue checking independent
subjects to produce more than one diagnostic. It MUST NOT feed an invalid
subject to a later phase or use a partial successful prefix as output. If
continued checking could disclose a secret, cross a security boundary, mutate
state, or make a later diagnostic depend on invalid data, processing stops at
the safe boundary.

## Diagnostic record

Every error required by this specification produces at least one structured
diagnostic record. The transport and serialization are implementation-defined,
but the record has these required semantic fields:

| Field | Required value |
| --- | --- |
| `class` | One diagnostic class from the table below. |
| `validation-phase` | One `V-*` phase from this chapter. |
| `processing-phase` | The most specific applicable stable phase identifier from Resolution, or the selected profile operation identifier when no `SD-*` through `MT-MATERIALIZE` identifier applies. |
| `requirement` | A stable specification chapter and anchor, matrix row identity, or typed outcome name whose rule was violated. |
| `subject` | The exact semantic object path, operation index, request URI with userinfo/query secrets removed, artifact filename, or fixture case identity that failed. |
| `locations` | Zero or more semantic source-document identities and TOML key paths relevant to the failure. A source line and column MAY be included when known. |
| `related` | Zero or more ordered semantic subjects needed to explain a cycle, duplicate, collision, conflict, redirect chain, or provenance relation. |

`message` MAY provide human text but is not compared for conformance. Absolute
host paths, temporary paths, localized text, stack traces, timestamps, and tool
versions MAY be attached as operational fields, but fixtures MUST exclude
them. A processor MUST NOT replace a required semantic path with only an
absolute path.

Diagnostic ordering is deterministic. Records are ordered first by the
earliest validation phase, then processing dependency order, then semantic
source identity or object path by UTF-8 bytes, then operation or array index,
then `class`, `requirement`, and `subject` by UTF-8 bytes. An implementation
MAY stream records in another order for display, but the ordered diagnostic
set used by fixtures and reports follows this rule.

## Diagnostic classes

| Class | Applies to |
| --- | --- |
| `document-syntax` | Invalid UTF-8, TOML syntax, duplicate TOML assignment, or root/fragment document-control syntax. |
| `model-validation` | Unknown key, wrong TOML type, invalid local constraint, missing required value, invalid default, or effective-value invariant. |
| `path-containment` | Invalid portable path/pattern, undecodable host name, canonical escape, wrong target kind, symlink traversal, or forbidden namespace entry. |
| `reference-resolution` | Missing, duplicate, ambiguous, wrong-kind, or cyclic named reference or immutable source object. |
| `conflict` | Composition type mismatch, repeated-reach exclusion, inheritance overlap, collision, duplicate identity, overlay scope conflict, or publication destination conflict. |
| `unsupported-operation` | A selected profile capability, required explicit environmental input, host guarantee, algorithm, format, or output boundary is unavailable before side effects. |
| `security-policy` | Credential-scope, URI, archive, sandbox, script, privilege, secret-handling, resource-limit, or other security requirement is violated. |
| `authorization` | Required authorization is unavailable or the owning typed protocol outcome is authorization failure. |
| `transport` | TLS or transport failure, malformed network response, or redirect-policy failure under the owning network contract. |
| `integrity` | Commit, digest, signature, package, archive, cache, manifest, or generated-output identity does not match the declared value. |
| `transformation` | An overlay or generator precondition, match cardinality, sequential parse, change postcondition, archive semantic result, or rollback requirement fails. |
| `artifact-validation` | Final tree, manifest, mode, behavioral identity, provenance ledger, or atomic-publication invariant fails. |
| `conformance` | Fixture format, declared input, expected outcome, closed-union, or normative traceability requirement fails. |

The typed HTTPS outcomes in [Sources](./sources.md#https-artifact-fetch-and-source-selection)
retain their exact names. `authorization-failure` maps to `authorization`;
`tls-failure`, `transport-failure`, and `malformed-response` map to
`transport`; and `integrity-failure` maps to `integrity`.

A resource limit reached while parsing an otherwise untrusted input is
`security-policy`. A configured digest mismatch remains `integrity`. An
implementation failure that prevents producing a required record is not a
successful or conforming result and MUST be reported outside the portable
diagnostic set rather than disguised as one of the input-error classes.

## Security and secret redaction

The diagnostic `subject`, `locations`, `related`, `message`, and operational
attachments MUST NOT contain credential values, authorization headers, private
keys, bearer tokens, secret query values, cookies, or secret-bearing
environment variables. A redaction marker cannot stand in for a required
semantic identifier; the processor instead records a non-secret credential
reference and origin/purpose scope.

Fixtures and reports follow the same rule. The complete credential contract is
defined in [Security](./security.md#credentials-and-sensitive-values).

## Failure and rollback

Every error is terminal for the subject boundary owned by its phase:

- a source-document error contributes no source model;
- a loading/composition error produces no composed model;
- a resolution error produces no resolved model for the affected evaluation;
- an operation preflight error causes no profile side effect;
- acquisition or transformation failure publishes no candidate artifact;
- local artifact validation or commit failure leaves the prior local artifact
  unchanged; and
- remote package or image publication failure produces no portable success
  result. The specification does not infer, simulate, or claim reversal of
  remote effects.

An implementation MUST NOT emit a success-shaped empty result, warning-only
fallback, skipped operation, placeholder file, failure marker, or partially
updated local destination where this specification requires an error. A
remote error is not evidence that a destination rolled back a prior effect.

The tracked [conformance manifest](./examples/conformance/README.md) includes
positive and negative diagnostic expectations using classes, phases, and
requirement anchors rather than exact prose.

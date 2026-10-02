[Previous: Optional workflows](./optional-workflows.md) ·
[Overview](./index.md) ·
[Implementation resource: Reference](./reference.md)

# Errors, determinism, and security

Processing never turns an invalid partial result into success. It either
produces the complete result for the current boundary or fails without letting
that partial state escape.

An invalid attempt stops at its current boundary. Resolution, materialization,
and selected operations use only inputs frozen for their attempt. Processing
treats credentials, paths, archives, and executable work as untrusted. The
binding rules are in
[Validation and diagnostics](./validation.md),
[Determinism and environmental inputs](./determinism.md), and
[Security and authorization](./security.md).

## Fail at the owning boundary

Ordinary processing uses eight validation groups from `V-DOCUMENT` through
`V-PUBLISH`. They identify ownership, dependencies, and observable boundaries
rather than one flat loop. Later processing cannot repair, coerce, drop, retry,
or reinterpret an earlier error. `V-CONFORMANCE` is separate fixture checking
for format, declared inputs, expectations, and traceability.

The result of a failure depends on where it occurs:

- an invalid source document contributes no source model;
- loading or composition failure produces no composed model;
- resolution failure produces no resolved model for that evaluation;
- optional-operation preflight failure causes no side effect;
- acquisition, transformation, or artifact failure publishes no candidate;
- failed local publication leaves the prior local artifact unchanged; and
- failed remote publication produces no portable success result, without
  claiming that the remote system rolled back.

A warning, skipped operation, placeholder, failure marker, or success-shaped
empty result cannot stand in for a required error.

Details: [Validation phase order](./validation.md#validation-phase-order) and
[Failure and rollback](./validation.md#failure-and-rollback).

## Make environmental inputs explicit

An operation can observe only the values frozen in its immutable
environmental-input record. Every resolution, materialization, or selected
profile operation receives one record. A retry, redirect, subprocess, or
cache lookup cannot replace a value.

When an operation needs architecture, time, network policy, RPM macros,
toolchain identity, or another environmental value, the caller supplies it
explicitly. The host cannot fill in a missing value. See the
[Environmental-input record](./determinism.md#environmental-input-record).

## Exclude ambient host behavior

Ambient host state cannot change a portable result. Local paths, process
settings, host configuration, timing, scheduling, and randomness are either
explicitly replaced or excluded. A cache entry is usable only after its
identity and bytes are verified.

If the host cannot represent a required path, mode, namespace, isolation
boundary, or atomic publication guarantee, the operation returns
`unsupported-operation` or fails under its owning rule. It does not adapt the
portable model to the host.

Every tracked influence is classified as `explicit`, `fixed`, `prohibited`,
`irrelevant`, or `external-failure`. The
[environmental-input matrix](./environmental-input-matrix.tsv) applies those
classifications and records the only source-selection exception. For the
exact HTTPS outcomes and lookaside `not-found` transition to the configured
origin, see
[HTTPS artifact fetch and source selection](./sources.md#https-artifact-fetch-and-source-selection).

## Report structured diagnostics

When processing fails, the diagnostic tells the reader what failed, where it
failed, and which rule was violated. It uses semantic identities instead of
depending on host paths, and it never includes secrets.

Each diagnostic uses the required fields and one of 13 classes. Diagnostics
compare in a deterministic order; message text and serialization remain
implementation-defined. Absolute paths and operational details may be
attached, but they cannot replace the semantic subject.

Details: [Diagnostic record](./validation.md#diagnostic-record),
[Diagnostic classes](./validation.md#diagnostic-classes), and
[Security and secret redaction](./validation.md#security-and-secret-redaction).

For example, an include that escapes the project can report
`class = "path-containment"`, `validation-phase = "V-COMPOSE"`,
`processing-phase = "LD-INCLUDES"`, the requirement
`loading.md#match-handling-and-containment`, and a project-relative semantic
subject. Implementations may choose their own message text and serialization.

## Bind credentials to purpose and origin

Credentials are opaque operation inputs. Each reference binds a value to one
purpose and canonical HTTPS origin and may narrow it to a realm or
destination. It also belongs to the selected operation and environmental-input
record.

Credential values never come from ambient host configuration or appear in
model data, artifacts, provenance, or portable diagnostics.

A redirect checks credential scope again. Authorization is not forwarded to a
different canonical origin unless another reference is explicitly bound to
that target origin and purpose. See
[Credentials and sensitive values](./security.md#credentials-and-sensitive-values).

## Treat paths, archives, and scripts as untrusted

Validate paths before host lookup. Enforce containment at every required
boundary, snapshot declared inputs, keep writes in private staging, and create
destinations without following an attacker-replaced link.

Treat archives as data. Any archive that violates the containment, entry,
metadata, or resource rules fails the whole attempt.

Core materialization runs no source-controlled scripts. Optional generator,
RPM-build, image, and test processes run only inside the boundary of their
selected profile with its declared isolation, inputs, outputs, network,
credential, and limit policy. Custom generation receives no credentials and
has network access denied by default.

Details: [Paths, filesystems, and publication](./security.md#paths-filesystems-and-publication),
[Archives](./security.md#archives), and
[Custom generator isolation](./security.md#custom-generator-isolation).

## Apply limits without truncating results

Before reading untrusted input or starting executable work, bind every
applicable non-negative limit listed in
[Resource limits and denial of service](./security.md#resource-limits-and-denial-of-service).

Zero forbids the resource; it does not mean unlimited. Reaching any limit
fails the whole attempt. It never authorizes a truncated artifact or partial
output.

## Further reading

- [Processing and validation order](./resolution.md#processing-and-validation-order)
- [Conformance examples and fixture format](./conformance.md)

The narrative is complete. During implementation, use the Reference to find
the detailed owner, matrix, or fixture for the task at hand.

[Continue during implementation: Reference](./reference.md)

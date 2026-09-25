[Previous: Optional workflows](./optional-workflows.md) ·
[Overview](./index.md) ·
[Implementation resource: Reference](./reference.md)

# Errors, determinism, and security

> **Non-normative reading guide.** This page explains the common failure and
> safety model. The primary normative owners are
> [Determinism and environmental inputs](./determinism.md),
> [Validation and diagnostics](./validation.md), and
> [Security and authorization](./security.md).

A conforming attempt either produces the complete result owned by its phase or
fails without using invalid partial state. Inputs that can affect portable
behavior are declared or fixed by the specification; ambient host state does
not silently become configuration.

**First-pass takeaways:**

- an error stops at the boundary that owns it and cannot be repaired later;
- portable behavior depends only on declared, fixed, or explicitly classified
  influences; and
- diagnostics retain semantic identities while excluding secret values.

## Fail at the owning boundary

Ordinary processing validates documents through local/profile publication.
Separate conformance-fixture checking uses `V-CONFORMANCE`. These phase
identifiers express ownership and dependencies, not one flat runtime loop; no
later phase repairs, coerces, drops, retries, or reinterprets an earlier error.

The main rollback boundaries are:

- an invalid source document contributes no source model;
- loading or composition failure produces no composed model;
- resolution failure produces no resolved model for that evaluation;
- optional-operation preflight failure causes no side effect;
- acquisition, transformation, or artifact failure publishes no candidate;
- failed local publication leaves the prior local artifact unchanged; and
- failed remote publication produces no portable success result, without
  claiming remote rollback.

Warnings, skipped work, placeholder files, failure markers, and
success-shaped empty results cannot replace a required error. Exact phase
ownership is in [Validation phase order](./validation.md#validation-phase-order)
and [Failure and rollback](./validation.md#failure-and-rollback).

## Make environmental inputs explicit

Every resolution, materialization, or selected profile operation has one
immutable environmental-input record. Depending on the operation, it can
contain architecture, evaluation instant, network context, resource limits,
toolchain identity, and RPM macro context. Required values are bound before
use and cannot be replaced by a retry, redirect, subprocess, or later phase.

The environmental-input matrix classifies each influence as:

- **explicit**, supplied by a named operation input;
- **fixed**, completely defined by the specification;
- **prohibited**, not consulted;
- **irrelevant**, allowed to affect performance or non-semantic messages only;
  or
- **external-failure**, able to prevent success through a defined typed
  failure but not to change accepted bytes or select another source except for
  the specified lookaside not-found transition.

The normative classifications are in
[Environmental-input record](./determinism.md#environmental-input-record) and
[the environmental-input matrix](./environmental-input-matrix.tsv).

## Exclude ambient host behavior

Core processing uses strict UTF-8 and specification-defined comparison and
ordering. Process locale, host timezone, ambient wall clock, host architecture,
CPU count, scheduler order, process identifiers, current directory, checkout
root, cache location, temporary path, `HOME`, `PATH`, proxy variables, RPM
configuration, Git configuration, `umask`, and random values cannot change a
portable result.

An operation that needs architecture receives it explicitly. Time-dependent
external validity uses one explicit UTC evaluation instant. Network operations
receive exact trust material, proxy choice, and deadlines. RPM build receives
a complete macro context and immutable toolchain identity. Caches are usable
only after the requested immutable identity and bytes are verified.

If a host cannot represent a required path, mode, namespace, or atomic
publication guarantee exactly, the operation fails or is unsupported; it does
not adapt the portable model to host behavior.

## Report structured diagnostics

Every required error has at least one diagnostic with a class, validation
phase, most specific processing phase, requirement identity, semantic subject,
and any relevant semantic locations and related subjects. Exact message text,
terminal formatting, localization, and serialization are not standardized.

The 13 diagnostic classes distinguish document syntax, model validation, path
containment, reference resolution, conflicts, unsupported operations, security
policy, authorization, transport, integrity, transformation, artifact
validation, and conformance errors. Records have deterministic comparison
order.

Absolute paths and operational details may be attached, but they cannot
replace semantic identities. Credential values, authorization headers, private
keys, tokens, cookies, and secret query values never appear in required
diagnostics, fixtures, provenance, or reports. See
[Diagnostic record](./validation.md#diagnostic-record),
[Diagnostic classes](./validation.md#diagnostic-classes), and
[Security and secret redaction](./validation.md#security-and-secret-redaction).

Non-normative diagnostic sketch: an escaping include can be reported as
`class = "path-containment"`, `validation-phase = "V-COMPOSE"`,
`processing-phase = "LD-INCLUDES"`, with requirement
`loading.md#match-handling-and-containment` and a project-relative semantic
subject. Exact serialization and message text remain implementation-defined.

## Bind credentials to purpose and origin

Credentials enter only through opaque operation-time references. Each
reference is bound to an exact purpose, canonical HTTPS origin, optional realm
or destination, selected operation, and environmental-input record. Credential
values do not appear in TOML, URIs, resolved models, artifacts, provenance, or
portable diagnostics.

Redirects re-evaluate credential scope. Authorization is not forwarded across
a canonical origin change unless a separate reference is explicitly bound to
the target origin and purpose. Ambient environment variables, Git
configuration, RPM configuration, netrc, browser state, and home-directory
stores cannot supply a credential implicitly.

Exact rules are in
[Credentials and sensitive values](./security.md#credentials-and-sensitive-values).

## Treat paths, archives, and scripts as untrusted

Portable path grammar and lexical rejection run before host lookup.
Containment is checked at every required boundary, declared file inputs are
snapshotted, writes occur only in private staging, and destinations are
created without following attacker-replaced links.

Archives are parsed as data rather than extracted by a host utility into the
project tree. Path traversal, absolute paths, hardlinks, unsupported types,
escaping links, duplicate paths, malformed metadata, trailing data, and limit
exhaustion fail the complete attempt.

Core materialization executes no source-controlled scripts. Optional generator,
RPM-build, image, and test processes run only inside their selected profile
boundary with explicit inputs, writable outputs, network policy, credentials,
and limits. Custom generation receives no credentials and has network denied
by default.

See [Paths, filesystems, and publication](./security.md#paths-filesystems-and-publication),
[Archives](./security.md#archives), and
[Custom generator isolation](./security.md#custom-generator-isolation).

## Apply limits without truncating results

Before untrusted input or executable processing, the operation binds applicable
limits for bytes, expanded size, entry count, individual files, path length,
redirects, request attempts, processes, memory, and execution time. Zero
forbids the resource; it does not mean unlimited. Reaching a limit fails the
attempt and never authorizes a truncated artifact or partial output.

## Further reading

- [Processing and validation order](./resolution.md#processing-and-validation-order)
- [Conformance examples and fixture format](./conformance.md)

[Continue during implementation: Reference](./reference.md)

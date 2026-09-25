[Return to index](./index.md)

# Determinism and environmental inputs

This chapter defines the environmental-input boundary for normative
resolution and materialization. An implementation MUST NOT consult ambient
host state when this specification supplies an explicit input or fixed rule.
If an implementation cannot prevent an undeclared influence from changing a
required success result, output bytes, semantic provenance, or diagnostic
class, the affected operation is unsupported.

## Environmental-input record

Every operation that performs `RM-*`, `MT-MATERIALIZE`, or a selected profile
operation has one immutable **environmental-input record**. It is operation
interface data, not a new TOML table. The record contains every explicit
environmental value that the operation is permitted to consult, including the
target architecture, evaluation instant, network context, resource limits,
toolchain context, and RPM macro context when applicable.

The record is frozen before the first phase that can consume one of its values.
A later retry, redirect, subprocess, cache lookup, or profile operation MUST
NOT replace a frozen value. The processor MUST reject two differently spelled
inputs that purport to supply the same record field and MUST NOT fill a missing
required field from the host.

Each influence in the normative
[environmental-input matrix](./environmental-input-matrix.tsv) has exactly one
classification:

- **explicit**: the value is present in the environmental-input record or
  another named operation input;
- **fixed**: this specification supplies the complete value or algorithm;
- **prohibited**: the processor and every conforming child process MUST NOT
  consult the influence; or
- **irrelevant**: it may affect implementation performance or an operational
  message but cannot affect a required result, diagnostic class, or claim; or
- **external-failure**: the value is not an operation input and cannot change
  accepted bytes, provenance, or source selection, but live availability may
  prevent success through the exact typed failure owned by the applicable
  protocol. The only source-selection exception is the specified lookaside
  `404`/`410` transition.

The matrix is normative. A chapter-specific rule may further restrict a row
but cannot silently relax it. A processor that uses a value classified
`prohibited` or uses an undeclared value in place of an `explicit` input
produces no conforming result.

## Locale, timezone, and wall clock

Core processing is locale-independent. Text decoding is strict UTF-8;
comparison, matching, and ordering use the Unicode-scalar or UTF-8-byte rules
owned by the applicable chapter. A processor MUST NOT use the process locale,
locale collation, locale case conversion, localized number parsing, localized
date parsing, or localized diagnostic text as a semantic input.

Core processing has no ambient timezone. Declared offset date-times are
interpreted by their explicit offset, and an instant that requires a canonical
zone is represented in UTC. A processor MUST NOT interpret a local timestamp
using the host timezone or use the `TZ` environment variable to change a
normative value.

The ambient wall clock is not an input to `SD-*`, `LD-*`, `CM-*`, `RM-*`, or
core `MT-MATERIALIZE` processing. A processor MUST NOT synthesize source
timestamps, commit metadata, archive metadata, file content, ordering, or
provenance from the current date or time. A selected operation that must
evaluate time-dependent external validity uses the explicit evaluation instant
defined by this chapter's environmental-input contract; absence of that input
is an error before the time-dependent access.

Diagnostics MAY contain an operational observation time, but that field is
non-semantic, is not a required diagnostic field, and MUST be excluded from
fixture comparison. Its presence cannot change the diagnostic class or any
normative output.

The tracked [ambient locale/time fixture](./examples/determinism/README.md)
uses conflicting host locale, timezone, and wall-clock values and requires one
identical normative projection.

## Architecture, platform, and concurrency

The host machine architecture is prohibited as an implicit input. An operation
that selects architecture-dependent data MUST receive one exact non-empty
`architecture` value in its environmental-input record. The owning profile
constrains the accepted values. An operation that does not define an
architecture input MUST NOT inspect the host architecture.

The operating-system name, kernel version, CPU features, CPU count, scheduler
order, process identifier, hostname, container identifier, and parallelism
level are prohibited semantic inputs. A processor MAY use parallel execution
only when the result and ordered diagnostics are identical to the specified
serial dependency order. A race, cache collision, or unavailable host feature
is an error or unsupported operation, not permission to produce a different
tree.

Filesystem case folding, Unicode normalization, timestamp granularity,
ownership, ACLs, extended attributes, and default permissions do not change
the portable namespaces. Where an exact host representation is impossible,
the operation fails. A host `umask` MUST NOT change a mode required by this
specification.

## Process environment and operational paths

Core phases use an empty semantic process-environment allowlist. In particular,
`HOME`, `XDG_CONFIG_HOME`, `PWD`, `OLDPWD`, `TMPDIR`, `PATH`, `LANG`, `LC_*`,
`TZ`, proxy variables, RPM variables, Git configuration variables, and
source-date variables MUST NOT add source documents, defaults, repositories,
credentials, macros, or transformations.

The current working directory, absolute checkout root, cache path, temporary
path, output path, and staging path are implementation details. Only the
normalized semantic identities and declared path bases participate in model or
artifact equality. Moving the same declared input tree or changing a temporary
directory name cannot change a conforming result.

An implementation MAY use caches only after validating that the cached object
has the exact immutable identity and bytes required by the current operation.
Cache presence, age, path, eviction order, and prior failed state are
irrelevant. A stale or unverifiable entry is ignored or diagnosed; it is never
accepted as a hidden input.

## Network and external validity context

A network-using operation has one explicit **network context** in its
environmental-input record:

- an exact trust-policy identity and digest of the trust material;
- explicit proxy use or the fixed value `none`;
- connect, idle-read, and overall deadlines as non-negative integer
  milliseconds.

Request-attempt count is not a network-context field. The operation's
resource-limit document is its sole owner and, in specification revision
`0.1`, binds exactly one request attempt. Redirect hops and the one permitted
lookaside-to-configured-origin source transition are state transitions inside
their respective single fetch attempts; no terminal outcome is retried.

The evaluation instant is a separate field of the same environmental-input
record, represented as an RFC 3339 UTC instant. It is bound exactly once and
is consumed by the network context for certificate and credential validity.
A network-context file, trust record, or request MUST NOT contain another
evaluation-instant value.

Operational interfaces MUST supply these values and invariants. Proxy `none`
forbids proxy discovery; a proxy origin is canonicalized by the one Sources
authority/origin grammar. The ordinary fixture format does not define a
portable network-context document.

DNS results, route selection, live server state, retry timing, and packet
ordering are `external-failure` influences, not values in the environmental
record. They may cause the typed acquisition failure defined by the owning
chapter, but they MUST NOT change accepted bytes or select a different source
except for the exact lookaside `404`/`410` transition defined by Sources.
After the single attempt reaches any typed outcome, processing stops and
reports that outcome. A processor MUST NOT retry it.

Revision `0.1` does not define portable live-network or replay evidence,
external-operation transcripts, deterministic network adapters, or transport
simulation.
Ordinary acquisition fixtures may exercise declared request construction,
redirect policy, typed outcomes, and digest verification without standardizing
an external-operation protocol.

Credential values and forwarding policy are governed by
[Security](./security.md#credentials-and-sensitive-values).

## RPM macro and external tool context

The bounded RPM representation used by core overlay processing performs no
RPM macro expansion and MUST NOT read host RPM macro files. Therefore an RPM
macro context cannot change core materialized spec bytes.

An RPM-build operation has an explicit **RPM macro context** containing the
complete base macro map, complete undefined-name set, target architecture,
target distro/version, and exact applied `build.defines` and
`build.undefines`. It MUST NOT read `/etc/rpm`, a user's RPM configuration,
environment-derived macros, or a tool default that is absent from that
context. A missing complete context is `unsupported-operation` before build
setup. This rule does not make final RPM bytes a conformance output.

When an external tool participates in a selected profile operation, the
environmental-input record identifies the toolchain and its immutable package
or image manifest. Tool name or version alone is insufficient when two
installations with that name can contain different bytes. Core algorithms
remain defined by this specification: using a different TOML, Git, hash,
regular-expression, HTTP, archive, or filesystem library cannot change their
required result.

## Randomness and resource limits

Core processing uses no random input. Random numbers, entropy devices,
host-generated UUIDs, randomized map iteration, and nondeterministic temporary
names MUST NOT appear in semantic output, provenance, ordering, or required
diagnostic fields.

Every operation that parses untrusted remote data or executes a profile
process receives explicit resource limits before access: maximum input bytes,
maximum expanded bytes, maximum entry count, maximum individual file bytes,
maximum path bytes, maximum redirects, exactly one artifact-fetch request
attempt, maximum process count, maximum memory bytes, and maximum execution
milliseconds as applicable. Limits constrain whether an operation can
complete; they never truncate an otherwise required result. Reaching a limit
is an error and produces no partial output.

The exact security application of these limits is defined in
[Security](./security.md#resource-limits-and-denial-of-service).

[Return to index](./index.md)

# Security and authorization

This chapter defines security constraints that apply in addition to the
syntax, path, acquisition, archive, artifact, and profile contracts. A
processor MUST fail closed: inability to enforce a required boundary is
`unsupported-operation` or `security-policy`, never permission to continue
with weaker behavior.

## Credentials and sensitive values

Credentials are explicit operation inputs supplied through an opaque
credential reference. This specification does not standardize a credential
store, prompting mechanism, operating-system secret service, hardware token,
or wire representation.

Each reference is bound before access to:

- one exact purpose such as source acquisition, repository access, package
  publication, or image publication;
- one exact canonical HTTPS origin under
  [Sources](./sources.md#canonical-https-authority-and-origin);
- an optional exact authorization realm or destination identity; and
- the selected operation and environmental-input record.

Credential values MUST NOT appear in source TOML, a URI, a resolved model,
the materialized tree, the provenance ledger, a fixture, an implementation
report, or a required diagnostic field. They MUST NOT be inherited from
ambient process variables, Git configuration, RPM configuration, netrc files,
browser stores, or a user's home directory. Exact storage and in-memory
protection mechanisms are implementation-defined, but a processor MUST limit
credential disclosure to the request or publisher that owns the bound scope.

An authorization header or cookie MUST NOT be forwarded across a canonical
origin change. DNS case, explicit port 443, or an alternate valid IPv6 text
spelling cannot create a distinct credential scope because they canonicalize
before lookup. A redirect to another canonical origin receives no credential
unless the operation input contains a separate credential reference explicitly
bound to that target origin and purpose. Query parameters from a request URI
are not credentials under this contract; a producer that treats one as secret
MUST exclude its value from diagnostics and conformance evidence.

The custom-source-generation profile receives no credentials in this revision.
A generator requiring authenticated network access is unsupported rather than
receiving a host or operation credential implicitly.

## URI and network rules

Every normative remote access uses the exact HTTPS URI and redirect rules in
the owning chapter. Userinfo, plaintext HTTP, downgrade redirects, invalid
certificate or hostname validation, and trust-on-first-use are forbidden.

`resources.rpm-repo-sets.<set>.disable-ssl-verify` is compatibility data, not
permission to weaken this rule. If its effective value is `true`, any selected
operation that references that set MUST fail with `security-policy` at
`V-OPERATION` before repository expansion, credential loading, DNS, proxy, TLS,
or HTTP access. A processor MUST NOT translate the value into a disabled
certificate-chain check, hostname check, or other reduced TLS policy.

The network context in
[Determinism](./determinism.md#network-and-external-validity-context) supplies
the trust material, evaluation instant, proxy decision, and deadlines. A proxy
MUST NOT receive an origin credential unless the credential is separately
bound for proxy authentication. DNS and proxy resolution cannot authorize a
different URI origin or weaken TLS hostname verification.

Artifact acquisition uses the exact request contract in
[Sources](./sources.md#https-artifact-fetch-and-source-selection). Ambient
cookies, referrers, authorization, user-agent fields, conditional requests, or
content-coding negotiation are forbidden. Redirects reconstruct a new request
and re-evaluate credential scope; they do not forward ambient or prior-hop
state.

Response size is checked incrementally against the bound resource limits.
Exceeding a limit aborts the request; no truncated prefix is hashed, parsed, or
used as an artifact. Redirect bodies and non-success bodies never become
source bytes.

## Paths, filesystems, and publication

All project, materialized-tree, archive, and symbolic-link paths use their
owning portable grammar before host lookup. A processor MUST:

- perform lexical rejection before opening a host path;
- enforce canonical containment at every required lookup;
- avoid following a symbolic link as a directory where the owning contract
  forbids it;
- snapshot declared file bytes, type, executable classification, and semantic
  identity before a transformation or child process consumes them;
- write only to private staging state beneath an implementation-controlled
  root;
- create destinations without following an attacker-replaced symlink;
- revalidate the final staged namespace before publication; and
- publish atomically under the artifact contract.

A detected change between validation and snapshot, or between snapshot and a
required immutable read, is a `security-policy` error. An implementation MAY
use descriptor-relative APIs, immutable snapshots, content-addressed storage,
or another race-resistant mechanism; the mechanism is not normative.

No profile child process receives the host project root, credential store,
home directory, cache, container socket, device tree, or publication
destination as a writable mount unless the owning profile explicitly defines
that capability. Core materialization never executes source-controlled
scripts.

## Archives

Archive bytes are parsed as data under
[Archive extraction and batching](./overlays.md#archive-extraction-and-batching);
they are not extracted by a host utility into the project or publication tree.
Path traversal, absolute paths, unsupported entry types, hardlinks, escaping
symbolic links, duplicate normalized paths, non-zero padding, trailing data,
and unsupported metadata are errors before an entry is exposed to an
operation.

The processor applies the explicit archive resource limits before
decompression and while reading every header and payload. It MUST count
decompressed bytes, semantic entries, individual file bytes, and normalized
path bytes. Limit exhaustion aborts the complete component attempt and leaves
no extracted partial tree.

Repacking occurs only from the validated semantic archive result. The
configured post-overlay hash remains required. This chapter does not select a
canonical encoder or establish cross-tool archive-byte reproducibility.

## Custom generator isolation

The custom-source-generation profile executes only the exact snapshotted
script declared by `origin.script`. The selected implementation MUST isolate
one invocation from undeclared project files, host credentials, prior output,
ambient package state, and host-enabled network access. The isolation
mechanism, execution-root bytes, installed package payloads, kernel interfaces,
numeric identities, process namespace, environment layout, and proof method
are implementation-specific in revision `0.1`.

Every declared `origin.inputs` artifact is made available read-only under its
exact filename, and no undeclared artifact is available as an input. Every
declared `origin.mock-packages` name must be provided before execution or the
operation returns `unsupported-operation`; the specification does not define
their dependency closure or installed byte layout.

The script is invoked with no arguments. Its only accepted product is the
semantic archive root beneath `/azldev-gen/output/`; no other writable output
is consumed. Network and credential access are denied by default. Revision
`0.1` defines no portable network-enabled generator evidence or replay
protocol. A host proxy variable, default route, ambient credential, or package
manager configuration cannot enable access.

The exact output and archive boundary is defined in
[Custom-source generation](./sources.md#custom-source-generation-profile).

## Scripts in other profiles

RPM-build, image, and test operations MAY execute tools or test content only
inside their explicitly selected profile boundary. Their operation inputs
identify the toolchain, architecture, environment, writable outputs, network
policy, credentials, and resource limits. They MUST NOT gain access merely
because related profile data is present.

Publishing performs no source-controlled script execution. It consumes an
already identified artifact and exact route, then uses only the credential
bound to that route.

## Resource limits and denial of service

Before untrusted input or executable processing, the environmental-input record
contains every applicable non-negative limit:

- input and response bytes;
- decompressed or expanded bytes;
- archive and filesystem entry count;
- individual regular-file bytes;
- normalized path UTF-8 bytes;
- redirects and request/publication attempts, with attempts exactly `1` in
  specification revision `0.1`;
- child process count;
- memory bytes; and
- execution milliseconds.

A value of zero forbids the corresponding resource; it does not mean
unlimited. This revision defines no implicit unlimited value. The owning
operation may require a non-zero minimum and otherwise returns
`unsupported-operation` before access. Limit diagnostics contain counts and
limit names but no secret or untrusted payload bytes.

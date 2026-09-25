[Return to index](./index.md)

# Source identity and acquisition

This chapter defines component source identity and source-artifact inputs. It
does not standardize a lock file, a generated-directory layout, a refresh
command, or an implementation cache.

## Source URI templates

`distros.<distro>.dist-git-base-uri` and `lookaside-base-uri` use the exact
template contract in this section. This contract does not apply to direct
`source-files[].origin.uri` values or the legacy RPM repository variables in
[Resources](./resources.md).

A source URI template MUST be an ASCII absolute URI with lowercase scheme
`https`, a non-empty ASCII host, no userinfo, query, fragment, backslash,
space, control character, or empty authority. Placeholders are permitted only
in the path. The path MUST NOT contain an empty interior segment, a literal or
percent-decoded `.` or `..` segment, or a percent-encoded `/` or `\`.

Every existing percent escape in the template MUST have exactly two uppercase
hexadecimal digits. It MUST NOT encode an RFC 3986 unreserved byte
(`ALPHA`, `DIGIT`, `-`, `.`, `_`, or `~`), because that byte has one
unescaped canonical spelling. Existing valid escapes are retained
byte-for-byte; they are not decoded and re-encoded during substitution.

The only template tokens are `$name`, `$releasever`, `$hashtype`, `$hash`,
`$filename`, and `$$`. `$$` is a literal dollar token and emits `%24`.
Another `$`, including `$pkg`, is an unknown-placeholder error. Tokens may
occur anywhere in a path segment, including adjacent to literal suffix text,
but never in the scheme or authority.

The field-specific token cardinalities are:

| Template field | Required tokens | Optional tokens | Forbidden tokens |
| --- | --- | --- | --- |
| `dist-git-base-uri` | `$name` exactly once | `$releasever` zero or one time; `$$` any number of times | `$hashtype`, `$hash`, `$filename` |
| `lookaside-base-uri` | `$name`, `$hashtype`, and `$hash` exactly once each; `$filename` one or more times | `$releasever` zero or one time; `$$` any number of times | None beyond unknown tokens |

All substitutions are computed from the original template simultaneously.
Text introduced by one value is never scanned as a token. The case-sensitive
values are:

- `$name`: the exact effective `spec.upstream-name`, after its default to the
  component name;
- `$releasever`: the exact `release-ver` of the source distro version named by
  the effective `spec.upstream-distro`, not the independent target
  distro/version evaluation input;
- `$hashtype`: the exact canonical artifact spelling `MD5`, `SHA256`, or
  `SHA512`;
- `$hash`: the exact lowercase digest text associated with that artifact; and
- `$filename`: the exact artifact filename.

Every substitution value MUST be non-empty and MUST NOT contain `/` or `\`.
This validation occurs before UTF-8 encoding and percent encoding.
Consequently, substitution MUST NOT generate `%2F` or `%5C`. The field-specific
contracts may impose additional constraints, including the digest and filename
grammars below.

For each placeholder occurrence, UTF-8 encode its complete value and emit each
byte unchanged only when it is an RFC 3986 unreserved byte. Emit every other
byte as `%` followed by two uppercase hexadecimal digits. Therefore `%`, `$`,
`?`, `#`, spaces, and non-ASCII bytes in permitted values cannot create URI
syntax or new placeholders. Literal template bytes and existing canonical
escapes remain in place around the encoded value.

After expansion, validate the complete result again as a canonical absolute
HTTPS URI under the rules above, now with no `$` token permitted. In
particular, an encoded value that makes a complete path segment `.` or `..`,
an invalid port, or any malformed final URI is an error. URI-template failure
occurs before repository or lookaside I/O.

The [source URI-template fixture](./examples/source-uri-template/README.md)
defines exact outputs for case-sensitive hashes, simultaneous substitution,
UTF-8 and reserved bytes, existing escapes, and literal dollar tokens.

## Upstream component source

The reusable `SpecSource` contract applies at:

- `components.<name>.spec`;
- `default-component-config.spec`;
- `distros.<distro>.versions.<version>.default-component-config.spec`; and
- `component-groups.<group>.default-component-config.spec`.

The reusable `DistroReference` contract applies at each
`SpecSource.upstream-distro` path and at `project.default-distro`.
These are source-repository references, not the immutable target
distro/version evaluation input defined in
[Resolution](./resolution.md#target-distroversion-evaluation-input).

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `spec` | closed table | Optional in a source document; no default. Required in an effective component unless component discovery supplies it. | Only the fields in this table are known. | Recursive table composition. | Recursive typed composition in fixed low-to-high layer order; several groups may contribute only disjoint child leaves. | An effective component without a complete local or upstream source is an error. | Core |
| `spec.type` | string | Required in an effective `spec`; no default. | Exactly `local` or `upstream`. | Later scalar replaces earlier scalar. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Empty or another value is an error. Variant-forbidden fields are errors. | Core |
| `spec.path` | string path | Required for `type = "local"`; otherwise forbidden. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document; target is a regular RPM spec file. | Later scalar replaces earlier scalar while retaining its own provenance. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | The containing directory is the local dist-git input. `upstream-distro`, `upstream-name`, and `upstream-commit` are forbidden. | Core |
| `spec.upstream-distro` | closed `DistroReference` table | Required for `type = "upstream"` unless the entire table is absent and the effective `project.default-distro` supplies the complete reference; no default. | Only `name`, `version`, and `snapshot` are known. | Recursive table composition. | Recursive typed composition in fixed low-to-high layer order; several groups may contribute only disjoint child leaves. | A present table must itself contain `name` and `version`; it is never completed field-by-field from `project.default-distro`. The effective reference must resolve exactly one declared source distro version. It cannot select or reselect the target provider and is forbidden for `type = "local"`. | Core |
| `spec.upstream-distro.name` | string | Required when the table is present; no default. | Non-empty exact key in `distros`. | Later scalar replaces earlier scalar. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Missing target is an error. | Core |
| `spec.upstream-distro.version` | string | Required when the table is present; no default. | Non-empty exact key in the referenced distro's `versions` map. | Later scalar replaces earlier scalar. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | The distro's `default-version` does not silently fill this source-controlled reference. Missing target is an error. | Core |
| `spec.upstream-distro.snapshot` | RFC 3339 timestamp string | Optional; no default. | Must include an explicit offset or `Z`; fractional seconds are allowed. It is a selector, not an identity. | Later scalar replaces earlier scalar. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | It may guide pin refresh only. It never overrides an effective `upstream-commit` and cannot be the final resolved identity. | Core |
| `spec.upstream-name` | string | Optional; defaults to the effective component name. | Non-empty and contains no `/` or `\`. It names the upstream dist-git repository and supplies the required `$name` source-template value. | Later scalar replaces earlier scalar. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Forbidden for `type = "local"`. During `RM-SOURCE-ID`, its exact effective value MUST also satisfy the materialization-only [active-spec stem grammar](./artifacts.md#materialization-input-and-candidate-assembly) before acquisition. That same validated spelling derives the sole top-level source path `<upstream-name>.spec`; it is not a second semantic name, basename transform, or overlay selector. | Core |
| `spec.upstream-commit` | string | Optional in source and composed models; no default. Required and non-empty for every resolved upstream component. | Exactly 40 lowercase ASCII hexadecimal digits. It identifies a Git commit object, not a tag, branch, abbreviated object ID, or tree. | Later scalar replaces earlier scalar under ordinary composition. An explicit pin in a later normal component document therefore replaces an earlier generated snapshot pin. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | The selected repository must contain that exact object and it must be a commit. Snapshot and branch selectors never override it. Missing, abbreviated, uppercase, non-hex, nonexistent, or non-commit values are errors before acquisition. | Core |

`distros.<distro>.versions.<version>.dist-git-branch` is the branch selector
associated with a `DistroReference`. A branch and a snapshot MAY be used by a
pin-refresh producer to choose a commit. Branch HEAD is used only when no
snapshot is present. Selector resolution MUST return a full lowercase 40-digit
commit ID and MUST NOT begin component acquisition or materialization with only
a branch or snapshot.

For pin refresh, the deterministic selector algorithm is:

1. resolve the exact configured branch reference and record its full commit ID;
2. if no snapshot is present, select that branch-tip commit;
3. otherwise enumerate all commits reachable from that branch tip, retain
   commits whose Git committer timestamp is less than or equal to the snapshot
   instant, choose the greatest committer timestamp, and break an equal-time tie
   by the lexicographically smallest lowercase full commit ID; and
4. fail when the branch is missing, the timestamp is invalid, no reachable
   commit is eligible, or the selected object is not a commit.

Author timestamps, local checkout order, server listing order, and wall-clock
refresh time do not participate. A pin-refresh producer writes the selected ID
to an ordinary `upstream-commit` field before materialization.

The selected commit becomes portable source truth only through an
`upstream-commit` value in an ordinary source document. The specification does
not distinguish a generated pin document from any other included fragment.
Consequently the loading order and normal scalar composition rule alone decide
which pin wins. A processor MUST NOT give a generated pin, lock file, cache
entry, branch, or snapshot hidden precedence over the effective TOML value.

Before changing any managed `upstream-commit`, a pin-refresh producer MUST
construct the complete prospective document-reach sequence and identify, for
each component it will change, every managed pin contribution and every
author-controlled contribution to that same leaf. Each managed contribution
MUST precede every author-controlled contribution that it is intended to
default. The proof uses ordinary reach order only; a filename, directory, or
producer label grants no precedence.

If the producer cannot establish that ordering, it MUST fail before writing any
TOML. A repeated canonical-document reach creates no second pin contribution;
the proof uses the document's first deterministic depth-first position and
retains every later incoming reach as provenance. The check is atomic across
the requested refresh: failure for one managed pin leaves every source
document byte-for-byte unchanged. The producer MUST NOT reorder includes,
rewrite an author-controlled contribution, or rely on a hidden merge rule to
make the proof succeed. When the proof succeeds, later author TOML still wins
by ordinary scalar composition. The
[pin-order fixture](./examples/pin-order/README.md) exercises both orders.

An already effective exact `upstream-commit` is authoritative and selector
resolution is skipped for that component. This permits a deliberate later pin
to remain fixed while an earlier generated snapshot-pin fragment is refreshed.

During `RM-SOURCE-ID`, a processor MUST verify the effective
`upstream-commit` against the repository selected by `upstream-distro` and
`upstream-name`. Verification failure is an error and produces no resolved
model. Acquisition MUST check out the verified commit rather than the branch
HEAD observed at acquisition time.

**Review note:** Current azldev accepts abbreviated 7-40 digit, mixed-case pins,
permits omitted distro-reference versions through `default-version`, rejects
some direct component snapshots, and can materialize from branch HEAD. Those
behaviors are characterized evidence, not this contract.

## Local component source

For `spec.type = "local"`, `spec.path` identifies the RPM spec file and its
canonical target MUST be a regular file. The **local source root** is the
canonical parent directory of that target. Its semantic root identity is the
root's normalized project-relative path, or the empty string when it is the
project root.

Before materialization, a processor MUST walk the local source root without
following directory symlinks, using the strict filesystem-name adapter for
every entry. Directories are traversal nodes and do not receive records. Every
regular file and symbolic link receives one record, including the spec file.
Sockets, devices, FIFOs, unreadable entries, invalid host names, dangling
symlinks, and symlinks whose canonical target escapes the local source root are
errors. A symlink, including one targeting a directory, is recorded and is not
traversed.

The local identity input bytes use this versioned, domain-separated binary
grammar:

```text
manifest = %x41.5A.4C.2D.4C.4F.43.41.4C.2D.53.4F.55.52.43.45
           %x2D.4D.41.4E.49.46.45.53.54 %x00 %x01
           root-record entry-record*
root-record = %x52 u32be(root-byte-length) root-utf8
file-record = %x46 u32be(path-byte-length) path-utf8 executable
              sha256-content
link-record = %x4C u32be(path-byte-length) path-utf8
              u32be(target-byte-length) target-utf8
entry-record = file-record / link-record
executable = %x00 / %x01
sha256-content = 32 raw digest bytes
```

`u32be` is an unsigned four-byte big-endian integer. `root-utf8`, `path-utf8`,
and `target-utf8` are exact UTF-8 bytes without Unicode normalization. Entry
paths use `/`, are relative to the local source root, and contain no empty,
`.` or `..` segment. They are sorted by unsigned lexicographic comparison of
their `path-utf8` bytes before records are emitted. The root record occurs
once; there is no entry-count field or terminator.

For a regular file, `sha256-content` is SHA-256 over its exact bytes.
`executable` is `01` when any POSIX execute bit in `0111` is set and `00`
otherwise. All read, write, special, ownership, timestamp, ACL, xattr, and
directory mode information is ignored. Symlink mode bits are ignored;
`target-utf8` is the exact decoded link text returned by the filesystem-name
adapter, not a normalized or resolved replacement path.

Because directories have no records, adding, removing, or renaming an empty
directory does not change version-1 local source identity. A future contract
that makes directory-only state semantic MUST use a new identity version.

The local source identity is lowercase
`sha256:` followed by the SHA-256 digest of the complete manifest byte
sequence. It is computed, not a TOML field. A change to the semantic root,
entry path, file type, executable flag, regular-file bytes, or symlink target
changes the identity. The
[local identity fixture](./examples/local-source-identity/README.md) has the
expected identity
`sha256:57732d25eddb56e2b21a43a4ffbc694bb7a5e50a375d8818255e39add967f0e7`.
The surviving entries and their placement are defined by
[Materialized artifacts](./artifacts.md#materialization-input-and-candidate-assembly).

## Source artifact field contracts

The reusable `SourceFile` contract applies to every `source-files[]` entry in a
`ComponentConfig`. The array is replacing, not append-composed.

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `source-files` | Array of closed `SourceFile` tables | Optional; default `[]`. | Each entry has the provenance of its defining document. | Later array replaces earlier array. | Later inheritance layer replaces; contributions from several groups are a destructive overlap error. | Effective `filename` values must be unique. | Core |
| `source-files[].filename` | string | Required; no default. | Exact [modern manifest filename](#upstream-sources-manifest): one or more Unicode scalars; no `)`, `/`, `\`, NUL, C0 control, or U+007F; not `.` or `..`; and no leading or trailing space or tab. Path base N/A. | Entry scalar. | Inherited with its array entry. | Validate every present value during `SD-MODEL` and the effective sequence again before any acquisition or materialization. A duplicate configured or ambiguous upstream filename is an error. Accepted spelling and case are preserved exactly. | Core |
| `source-files[].hash-type` | string enum | Required; no default. | Exactly `SHA256` or `SHA512`; path base N/A. | Entry scalar. | Inherited with its array entry. | Must agree with `hash` length. | Core |
| `source-files[].hash` | lowercase hex string | Required; no default. | 64 digits for `SHA256`, 128 for `SHA512`; path base N/A. | Entry scalar. | Inherited with its array entry. | Bytes are rejected on mismatch before insertion. For `overlay`, this is the required post-transformation hash. | Core |
| `source-files[].origin` | Closed `Origin` table | Required; no default. | Variant fields below. | Recursive within the entry. | Inherited with its array entry. | Missing or incomplete origin is an error. | Core/profile by variant |
| `source-files[].origin.type` | string enum | Required; no default. | Exactly `download`, `local`, `custom-result`, `overlay`, or `custom`; path base N/A. | Entry scalar. | Inherited with its array entry. | Variant-forbidden fields are errors. `custom` execution requires its profile and a supported archive filename. | Core except `custom` execution profile |
| `source-files[].origin.uri` | absolute URI string | Required for `download`; otherwise forbidden. | `https` only; path base N/A. | Entry scalar. | Inherited with its array entry. | Fetches use the shared [HTTPS artifact-fetch state machine](#https-artifact-fetch-and-source-selection). Redirects must remain `https`; credentials are not part of the URI and follow [Security](./security.md#credentials-and-sensitive-values). | Core |
| `source-files[].origin.path` | string path | Required for `local` and `custom-result`; otherwise forbidden. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document; target is a regular file. | Entry scalar retaining provenance. | Inherited with its array entry. | `custom-result` asserts generated provenance but core only reads and hashes the supplied file. | Core |
| `source-files[].origin.script` | string path | Required for `custom`; otherwise forbidden. | Uses the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from its defining document; target is an executable regular file. | Entry scalar retaining provenance. | Inherited with its array entry. | Core never executes it. | Custom-source-generation profile |
| `source-files[].origin.mock-packages` | Array of strings | Optional for `custom`, default `[]`; otherwise forbidden. | Non-empty package names; path base N/A. | Entry array replaces. | Inherited with its array entry. | Duplicates are errors. | Custom-source-generation profile |
| `source-files[].origin.inputs` | Array of strings | Optional for `custom`, default `[]`; otherwise forbidden. | Plain filenames using the `filename` grammar; path base N/A. | Entry array replaces. | Inherited with its array entry. | Each input must name an already acquired upstream or earlier configured artifact; duplicates, forward references, and missing inputs are errors. | Custom-source-generation profile |
| `source-files[].replace-upstream` | boolean | Optional; default `false`. | Path base N/A. | Entry scalar. | Inherited with its array entry. | When true, exactly one upstream artifact of the same filename must exist and `replace-reason` is required. When false, any collision is an error. | Core |
| `source-files[].replace-reason` | string | Conditionally required; no default. | Non-empty after trimming Unicode whitespace; path base N/A. | Entry scalar. | Inherited with its array entry. | Required iff `replace-upstream = true`; otherwise forbidden. | Core |

The `local` and `custom-result` variants are additions to the portable model.
The characterized `custom` variant remains representable, but execution is
optional. A compatibility producer can convert a completed custom output into a
core `custom-result` entry without changing its filename or hash.

The configured filename rule intentionally equals the modern `sources`
filename grammar rather than a broader filesystem basename rule. Therefore
every accepted configured artifact can be emitted byte-for-byte in the
required canonical modern record without escaping. This validation completes
before lookaside/origin selection, network or local-file access, hashing,
candidate assembly, or manifest generation.

## Upstream `sources` manifest

For an upstream component, the manifest location is the single
repository-relative path `sources` in the root of the verified dist-git
checkout. If that path is absent, the upstream artifact sequence is empty. If
it is present, it MUST be a regular file, not a directory or symlink, and its
complete contents MUST be readable before any artifact is fetched. An empty or
comment-only file also produces an empty sequence. A filesystem, decoding, or
syntax failure is an acquisition error.

The file is UTF-8 without a byte-order mark. Lines are separated by LF or CRLF;
a final line terminator is optional and a bare CR is an error. Grammar
whitespace is ASCII space or tab. After removing leading and trailing grammar
whitespace, an empty line is blank and a line whose first character is `#` is
a comment. Comments occupy a complete line; inline comments are not accepted.
Every other line MUST match exactly one of these record forms:

```text
modern = hashtype 1*WSP "(" modern-filename ")" 1*WSP "=" 1*WSP digest
legacy = md5-digest 1*WSP legacy-filename

hashtype = "SHA256" / "SHA512"
digest = 64-lower-hex when hashtype is "SHA256"
       / 128-lower-hex when hashtype is "SHA512"
md5-digest = 32-lower-hex
WSP = %x20 / %x09
lower-hex = DIGIT / %x61-66
```

A modern filename is one or more Unicode scalar values other than `)`, `/`,
`\`, NUL, a C0 control, or U+007F. A legacy filename additionally contains no
space or tab. In either form the filename is not `.` or `..` and does not begin
or end with space or tab. Filename comparison is exact Unicode-scalar equality:
there is no normalization, case folding, escaping, or trimming.

The legacy form has the canonical hash type `MD5`; it exists only for
compatibility with existing dist-git manifests. A producer writing a new or
updated record SHOULD use the modern form with `SHA512`. Algorithm names are
case-sensitive, and digest text MUST be lowercase and exactly the length
specified above. `MD5` in modern form, an inferred algorithm other than legacy
MD5, unsupported names, uppercase digest digits, short or long digests, and
otherwise unmatched non-comment lines are errors.

Each accepted record maps, in line order, to one upstream artifact record
`(filename, hash-type, hash)`. The legacy hash type maps to `MD5`; modern hash
types retain their spelling. Two records with the same filename are an error
even when their algorithms and digests agree. A malformed record invalidates
the complete manifest; processors MUST NOT use a successfully parsed prefix.
Manifest comments and formatting are not part of the artifact records.
Preservation and rewriting of manifest bytes are defined in
[Materialized artifacts](./artifacts.md#sources-manifest-bytes).

## Acquisition and collision order

For an upstream component, acquisition proceeds as follows:

1. validate the effective configured filename sequence under the modern
   manifest filename grammar, reject duplicates, and validate the effective
   component and upstream active-spec stems;
2. verify and check out the exact `upstream-commit`;
3. parse the upstream artifact manifest (`sources`) and reject duplicate
   filenames;
4. acquire and hash each upstream lookaside artifact;
5. process configured `source-files` in array order:
   - `download` first attempts a hash-addressed lookaside object and then the
     declared `uri` only when `disable-origins = false`;
   - `local` and `custom-result` read the declared contained file;
   - `custom` invokes the optional profile and must yield exactly one file named
     `filename`;
   - `overlay` performs no fetch and reserves the matching upstream artifact
     for the archive transformation and post-overlay hash contract;
6. verify each configured hash at the boundary defined by its origin; and
7. apply the replacement/collision rules before constructing the effective
   artifact set.

For a local component there is no upstream lookaside manifest. `overlay` and
`replace-upstream = true` are therefore errors. A local component may still use
`download`, `local`, `custom-result`, or the optional `custom` profile. A
configured `download` on a local component goes directly to its declared
`origin.uri`; it does not attempt a distro lookaside lookup.

Acquisition failure, hash mismatch, missing replacement target, unexpected
collision, or an origin-policy failure aborts the complete component attempt.
No partially acquired set is a conforming output.

### Canonical HTTPS authority and origin

This is the single authority/origin grammar for every configured initial URI
and every resolved redirect target consumed by the HTTPS artifact-fetch state
machine. Repository GPG-key acquisition reuses the same grammar and state
machine. An implementation MUST canonicalize the URI before credential lookup,
logical request construction, same-origin testing, or redirect-loop testing.

The complete URI MUST be ASCII, use scheme `https` (compared
ASCII-case-insensitively and serialized lowercase), contain no userinfo or
fragment, and have one non-empty host. A raw non-ASCII IDN U-label and any
percent-encoded reg-name byte are forbidden. An internationalized DNS name
MUST instead be supplied as IDNA2008 A-labels satisfying STD3 ASCII rules; its
canonical spelling is lowercase ASCII. No implementation performs an implicit
locale or platform IDN conversion.

The host is exactly one of these forms:

1. A DNS name of one or more dot-separated labels. Each label is 1 through 63
   ASCII bytes, contains only letters, digits, and `-`, and begins and ends
   with a letter or digit. The complete name is at most 253 bytes and has no
   trailing dot. ASCII letter case is insignificant on input and lowercase in
   canonical form. An `xn--` label is accepted only when it is a valid
   IDNA2008 A-label.
2. An IPv4 address in canonical dotted-decimal form: exactly four decimal
   octets from 0 through 255, with no leading zero in a multi-digit octet.
3. An IPv6 address in brackets. Zone identifiers are forbidden. The parsed
   128-bit address is serialized inside brackets using RFC 5952 lowercase
   hexadecimal, suppressing leading zeroes and compressing the longest
   leftmost run of two or more zero fields. An embedded IPv4 spelling is
   converted to that same hexadecimal serialization.

An optional port is ASCII decimal from 1 through 65535, with no leading zero.
An absent port and explicit `443` both have effective port 443 and serialize
without `:443`; every other port serializes as `:` plus its canonical decimal
value. The **canonical authority** is the canonical bracketed or unbracketed
host plus that serialized non-default port. The **canonical origin** is
`https://` plus the canonical authority and denotes the tuple `(https,
canonical host value, effective port)`.

The origin-form request target is the URI path, or `/` when the path is empty,
followed by `?` and the exact query when present. This authority
canonicalization does not decode or re-encode path data. Source-template
substitution cannot introduce encoded separators because substitution values
reject `/` and `\` before encoding. The **canonical request URI** is the
canonical origin followed by that origin-form target.

All users consume these values identically:

- `Host` is the canonical authority;
- DNS TLS SNI is the canonical lowercase DNS name, while an IP literal sends
  no SNI; certificate identity validation uses the same canonical DNS name or
  parsed IP address;
- credential lookup and authorization forwarding use canonical-origin
  equality;
- same-origin redirect tests use canonical-origin equality; and
- redirect-loop identity is the complete canonical request URI, so DNS case,
  explicit default-port, and IPv6-text aliases cannot evade loop detection.

A syntactically valid absolute HTTPS URI that violates this policy produces
`transport-failure` when obtained from `Location`; a malformed URI produces
`malformed-response`. A configured initial URI that violates the grammar is a
model or operation-input error before DNS, credentials, TLS, or request
transmission.

### HTTPS artifact fetch and source selection

Every hash-addressed lookaside request and every configured
`source-files[].origin.uri` request uses the same HTTPS artifact-fetch state
machine. Repository GPG-key fetches invoke this state machine through
[Resources](./resources.md#repository-gpg-key-bindings), without defining a
parallel request algorithm. Its inputs are an initial absolute HTTPS request
URI and the expected artifact or key digest. The initial URI is converted to
the canonical request URI above before the first request. Source selection is
not part of this state machine.

Each hop uses this exact logical HTTP request contract:

1. HTTP version is `HTTP/1.1`, method is `GET`, the request target is the
   canonical origin-form path plus optional query from the current URI, and
   the request has no body or trailers.
2. `Host` is the canonical authority. TLS SNI and peer identity validation use
   the canonical DNS/IP rules above.
3. The complete end-to-end header set is `Accept: */*`,
   `Accept-Encoding: identity`, and at most one `Authorization` field inserted
   from the opaque credential reference bound to the current origin and
   purpose. The authorization value is never fixture data.
4. `Cookie`, `Cookie2`, `Referer`, `Origin`, `Range`, conditional request
   fields, cache validators, `User-Agent`, ambient authorization, and every
   other unspecified end-to-end field are forbidden. No cookie jar, netrc,
   browser state, library default, or process environment may add one.
5. A redirect request is rebuilt from the resolved target URI. It retains only
   the fixed `Accept` and `Accept-Encoding` fields and independently inserts an
   authorization field only when a credential reference is explicitly bound
   to that target origin. No response field or prior request field is copied.

An implementation may add protocol-required hop-by-hop framing only when it
cannot change the logical request above. A library default that changes the
method, target, authority, body, end-to-end fields, credential scope, or
representation request is a `security-policy` error before the request is
sent.

One HTTPS artifact fetch has exactly one typed outcome:

| Outcome | Definition |
| --- | --- |
| `hit(bytes)` | A complete successful HTTPS representation was received with identity content coding, and its exact representation octets match the expected digest. |
| `not-found` | The final HTTPS response status is exactly 404 or 410. No response body is used. |
| `authorization-failure` | Authentication could not be supplied or the final status is 401 or 403. |
| `tls-failure` | Certificate, hostname, protocol-version, or TLS-handshake validation failed. |
| `transport-failure` | DNS, connection, timeout, redirect-policy, or another transport failure occurred before syntactically valid final response headers were received. This outcome excludes premature EOF while reading a framed successful body. |
| `malformed-response` | Response headers are malformed or ambiguous; successful-body framing is invalid; a non-identity, malformed, or ambiguous content coding is present; or premature EOF occurs after syntactically valid final `200` headers while reading the framed body. |
| `integrity-failure` | Complete identity-coded representation octets were received but do not match the manifest or configured digest. |

The HTTP result is derived deterministically:

1. Begin with the canonical request URI derived from the selected fully
   expanded and validated HTTPS lookaside URI, configured `origin.uri`, or
   repository GPG-key URI. A TLS validation failure produces `tls-failure`.
   DNS, connection, timeout, or another failure before syntactically valid
   final response headers are received produces `transport-failure`.
2. Statuses 301, 302, 303, 307, and 308 are the only recognized redirects.
   Such a response MUST contain exactly one non-empty syntactically valid
   `Location` field. Missing, duplicate, or malformed `Location` produces
   `malformed-response`.
3. Resolve a relative `Location` against the current canonical request URI
   using RFC 3986 reference resolution, then apply the canonical HTTPS
   authority/origin grammar before any credential lookup or request. An
   `http` or other-scheme target, or an otherwise valid but policy-forbidden
   target, produces `transport-failure`; a syntactically malformed target
   produces `malformed-response`.
4. Ignore redirect response bodies. Compare the resulting canonical request
   URI with every canonical request URI already requested in this chain. A
   repeat is a redirect loop and produces `transport-failure`.
5. Follow at most ten redirects. Ten redirects followed by a final response is
   allowed; receiving an eleventh recognized redirect produces
   `transport-failure` without requesting its target.
6. A final status exactly 200 is eligible for `hit(bytes)` only after the
   complete response body has been received under valid HTTP framing.
   Premature EOF after syntactically valid final headers, including EOF before
   the declared `Content-Length` or terminating chunk, produces
   `malformed-response`, not `transport-failure`.
7. For that final `200`, `Content-Encoding` MUST either be absent or consist of
   exactly one field occurrence whose field value, after removing leading and
   trailing HTTP optional whitespace (`SP` or `HTAB`), is ASCII
   case-insensitively equal to `identity`. A repeated field, comma-separated
   coding list, empty value, invalid field value, or any coding other than
   `identity` produces `malformed-response`. The artifact bytes are the
   complete representation octets after HTTP transfer framing is removed,
   exactly as received and without content decoding or any other
   transformation. Digest verification hashes precisely those octets.
8. A final status exactly 404 or 410 produces `not-found`. A final status
   exactly 401 or 403 produces `authorization-failure`. Every other final
   status, including other 2xx, unrecognized 3xx, 4xx, and 5xx statuses,
   produces `transport-failure`.

Digest verification is the final state transition from complete eligible
representation octets to either `hit(bytes)` or `integrity-failure`. In
specification revision `0.1`, each invocation of this state machine performs
exactly one request attempt. All redirect hops belong to that attempt. The
processor stops after its first typed outcome, records that outcome, and MUST
NOT retry a transport, authorization, TLS, malformed-response, not-found, or
integrity result. The sole
lookaside-to-configured-origin transition invokes a new selected fetch, also
with exactly one attempt; it is source selection, not retry.
Credential sourcing, forwarding, cross-origin authorization, trust material,
deadlines, and the evaluation instant are governed by the explicit network context and
[Security](./security.md#credentials-and-sensitive-values). In particular,
credentials are never inferred from a URI, process environment, home
directory, or redirect.

| Artifact kind | Lookaside outcome | Next action |
| --- | --- | --- |
| Upstream manifest artifact | `hit(bytes)` with matching digest | Accept the artifact. |
| Upstream manifest artifact | Any other outcome | Terminal component acquisition error; no origin exists. |
| Configured `download` on an upstream component | `hit(bytes)` with matching configured digest | Accept the artifact; do not contact `origin.uri`. |
| Configured `download` on an upstream component | `not-found` and `disable-origins = false` | Fetch `origin.uri`, then require its configured digest. |
| Configured `download` on an upstream component | `not-found` and `disable-origins = true` | Terminal origin-policy error. |
| Configured `download` on an upstream component | Authorization, TLS, transport, malformed-response, or integrity failure | Terminal error; do not contact `origin.uri`. |
| Configured `download` on a local component | No lookaside attempt | Fetch `origin.uri`, then require its configured digest. |

Only a lookaside `not-found` outcome authorizes selection of the configured
origin, and only when origins are enabled. Once `origin.uri` is selected, every
outcome other than `hit(bytes)` is terminal, including `not-found`.
In particular, an origin redirect or status is interpreted by the same state
machine, while an origin not-found or integrity failure never authorizes
another source choice.

Live DNS, connection, timeout, and server availability may prevent one fetch
from completing through its exact typed failure. They are not environmental
record values and never authorize a different request, accepted byte stream,
credential scope, or source. The lookaside final status `404` or `410`, mapped
to `not-found`, is the sole source-selection exception and authorizes only the
configured-origin transition shown above.

## Overlay-origin association

An `origin.type = "overlay"` entry MUST set `replace-upstream = true`, MUST
match exactly one upstream archive filename, and MUST be referenced by at least
one archive-scoped overlay operation. Conversely, every transformed upstream
archive MUST have exactly one overlay-origin entry recording its post-overlay
hash. Archive recognition, batching, semantic output, and byte-level hash
containment are defined in
[Overlay transformations](./overlays.md#archive-extraction-and-batching).

## Custom-source-generation profile

The `custom` origin represents executable archive generation. Core never
executes it. A processor selects the profile only through the exact
`custom-source-generate` operation in
[Profiles](./profiles.md#selected-operation-contracts); otherwise selection of
the containing materialization returns `unsupported-operation` before sandbox
creation.

For a `custom` entry, the declared `filename` MUST have one of the supported
archive names `.tar`, `.tar.gz`, `.tgz`, `.tar.xz`, `.txz`, `.tar.zst`, or
`.tzst`. The content-detected compression must agree with that family after
generation. The operation inputs are:

1. the exact resolved `SourceFile` entry;
2. the snapshotted executable script bytes, script semantic identity, and
   script SHA-256;
3. the exact target architecture and RFC 3339 UTC evaluation instant;
4. exact bytes and verified identities for every declared `origin.inputs`
   filename, with no forward or undeclared input;
5. the ordered unique `origin.mock-packages` names and the selected
   implementation's availability result for those packages; and
6. all resource limits required by
   [Security](./security.md#resource-limits-and-denial-of-service).

The selected implementation supplies the
[custom generator isolation](./security.md#custom-generator-isolation).
Before the script starts, every declared input filename and declared mock
package MUST be available, no undeclared artifact may be exposed as an input,
and an unavailable declared package produces `unsupported-operation`.
Revision `0.1` does not define execution-root bytes, installed-package payload
closure, kernel-observation interfaces, or a portable proof that two
implementations constructed the same sandbox.

The script is invoked once with no arguments after the declared packages and
inputs are present. Failure, signal termination, timeout, resource exhaustion,
attempted credential access, undeclared input access, or forbidden network
access aborts the complete component attempt.

The script writes only beneath `/azldev-gen/output/`. That directory is the
semantic archive root; it is not itself emitted as a wrapper entry. After the
script exits successfully, the producer:

1. walks the output root without following a symbolic link as a directory;
2. applies the archive namespace, permitted entry-type, portable symlink,
   implicit-ancestor, mode, UTF-8, and resource-limit rules from
   [Archive extraction and batching](./overlays.md#archive-extraction-and-batching);
3. requires at least one semantic entry and rejects every entry outside the
   output root or any additional writable output;
4. packages that semantic tree using the compression family named by
   `filename`;
5. hashes the exact emitted archive bytes with the configured `hash-type`; and
6. accepts exactly one artifact only when the digest equals the configured
   `hash`.

The semantic archive result and configured output hash are exact. Revision
`0.1` does not standardize the generator execution environment or a portable
tar/xz/zstd encoder. A producer unable to emit bytes matching the configured
hash fails; it does not substitute a different archive.

On success the resulting artifact enters the ordinary configured-artifact
collision, manifest, placement, provenance, and materialization rules. A
compatibility producer MAY persist those exact bytes and replace the entry
with `custom-result`; core then reads and verifies the supplied file without
executing the generator.

The tracked [custom generator fixture](./examples/custom-generator/README.md)
covers declared input filenames, declared mock-package availability,
implementation-specific isolation availability, default-deny network behavior,
semantic output validation, undeclared input rejection, and configured
output-hash behavior.

No conformance class is enabled or claimed for this profile. This contract
prevents hidden ambient inputs and divergent accepted bytes, but it does not
define a canonical archive encoder or claim final RPM, image, or cross-tool
archive-byte reproducibility.

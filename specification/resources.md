[Return to index](./index.md)

# Resources and repository inputs

This chapter defines the complete RPM repository data model. These fields are
shared optional data for the RPM-build and image profiles. Their presence
activates no operation and requires no repository access. Every processor
accepting them still validates and preserves their canonical typed data under
[the common profile rules](./profiles.md).

`resources` is a closed table containing three name-keyed maps. Resource names
follow the common name rule and additionally MUST contain only ASCII letters,
digits, `.`, `_`, or `-`, and MUST begin and end with an alphanumeric
character.

## RPM repository resources

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `resources.rpm-repos` | Name-keyed map of closed repository tables | Optional; default empty map. | Resource-name constraints above. | Map by key; duplicate entry tables compose recursively. | N/A. | Every selected reference must resolve. | Shared RPM-build/image profile data |
| `resources.rpm-repos.<repo>.description` | string | Optional; no default. | No C0 controls; path base N/A. | Scalar replace. | N/A. | Diagnostic only. | Shared RPM-build/image profile data |
| `resources.rpm-repos.<repo>.type` | string enum | Optional; default `rpm-md`. | Exactly `rpm-md`; path base N/A. | Scalar replace. | N/A. | Other values are errors. | Shared RPM-build/image profile data |
| `resources.rpm-repos.<repo>.base-uri` | URI template string | Conditionally required. | Uses the exact [repository URI-template algorithm](#repository-uri-templates); `$basearch`, `$releasever`, and `$$` are permitted. | Scalar replace. | N/A. | Exactly one of `base-uri` and `metalink` is required. Expansion and final validation occur before repository access. | Shared RPM-build/image profile data |
| `resources.rpm-repos.<repo>.metalink` | URI template string | Conditionally required. | Uses the same [repository URI-template algorithm](#repository-uri-templates) and tokens. | Scalar replace. | N/A. | Exactly one of `base-uri` and `metalink` is required. Expansion and final validation occur before repository access. | Shared RPM-build/image profile data |
| `resources.rpm-repos.<repo>.disable-gpg-check` | boolean | Optional; default `false`. | Path base N/A. | Scalar replace, including `false`. | N/A. | When false, `gpg-key` is required. | Shared RPM-build/image profile data |
| `resources.rpm-repos.<repo>.gpg-key` | URI or string path | Conditionally required. | Absolute `https` URI, or the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from the defining document with a regular-file target. | Scalar replace retaining path provenance. | N/A. | Required unless GPG checking is explicitly disabled. This field is a locator only; a selected operation MUST supply the immutable [GPG-key binding](#repository-gpg-key-bindings). `http` and invalid portable paths are errors. | Shared RPM-build/image profile data |
| `resources.rpm-repos.<repo>.arches` | Array of strings | Optional; empty means all operation-supported architectures. | Unique non-empty architecture tokens; path base N/A. | Array replace. | N/A. | A selected architecture not in a non-empty list excludes the repo. | Shared RPM-build/image profile data |

## Repository URI templates

The exact repository URI-template algorithm in this section applies to
`resources.rpm-repos.<repo>.base-uri`,
`resources.rpm-repos.<repo>.metalink`, and legacy
`distros.<distro>.repos[].base-uri`. It is separate from repository-set
subpath expansion.

A repository URI template MUST be ASCII and parse as an absolute URI with
lowercase scheme `https`, a non-empty ASCII host, and no userinfo, fragment,
backslash, space, control character, or empty authority. Tokens may occur only
in the path or query. The direct resource fields permit `$basearch`,
`$releasever`, and `$$`; the legacy distro field permits `$releasever` and
`$$` and forbids `$basearch`. Each permitted value token may occur zero or
more times. `$$` is a literal dollar token. Any other `$` sequence is an
unknown-placeholder error.

Every existing percent escape in the path or query MUST have exactly two
uppercase hexadecimal digits and MUST NOT encode an RFC 3986 unreserved byte
(`ALPHA`, `DIGIT`, `-`, `.`, `_`, or `~`). A path escape MUST NOT encode `/`
or `\`. The path MUST NOT contain an empty interior segment or a literal or
percent-decoded `.` or `..` segment. Existing valid escapes are retained
byte-for-byte; they are not decoded and re-encoded.

All substitutions are computed simultaneously from the original template.
Text introduced by a value is never rescanned as a token. `$releasever` is the
exact `release-ver` of the distro version whose selected operation references
the repository. `$basearch` is the exact non-empty architecture value supplied
as an explicit input to that selected operation. The
[environmental-input contract](./determinism.md#architecture-platform-and-concurrency)
prohibits a host-derived source. There is no default.

For each value-token occurrence, UTF-8 encode the complete value and emit a
byte unchanged only when it is an RFC 3986 unreserved byte. Emit every other
byte as `%` followed by two uppercase hexadecimal digits. `$$` emits `%24`.
Thus reserved characters in either a path or query value remain data rather
than creating URI syntax.

After substitution, validate the complete result again by the structural URI
rules, now with no `$` token permitted. Canonical escapes emitted by
substitution are permitted to encode reserved value bytes, including `/` and
`\`; the prohibition on encoded path separators applies to escapes already
present in the template. The result MUST remain an absolute HTTPS URI, and a
value that makes a complete path segment `.` or `..` is an error. An invalid
template is a model error. Every selected operation that would access a direct
resource or legacy distro repository MUST supply `basearch` as an explicit
non-empty input, whether or not its template contains `$basearch`. When that
input is absent, the result is `unsupported-operation` before any repository
or metalink access. This operation-time input requirement does not activate
either optional consuming profile.

The [resource URI fixture](./examples/resource-uri/README.md) defines exact
direct and legacy outputs, including simultaneous substitution, existing
escapes, literal dollars, reserved and non-ASCII input bytes, and absent
`basearch`.

## Repository-set templates

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `resources.rpm-repo-set-templates` | Name-keyed map of closed template tables | Optional; default empty map. | Resource-name constraints. | Map by key and recursive table. | N/A. | Referenced templates must exist. | Shared RPM-build/image profile data |
| `resources.rpm-repo-set-templates.<template>.description` | string | Optional; no default. | Diagnostic UTF-8; path base N/A. | Scalar replace. | N/A. | No processing semantics. | Shared RPM-build/image profile data |
| `resources.rpm-repo-set-templates.<template>.subrepos` | Array of closed subrepo tables | Required; no default. | Non-empty ordered array. | Array replace. | N/A. | Subrepo `name` values must be unique. | Shared RPM-build/image profile data |
| `...subrepos[].name` | string | Required; no default. | Resource-name grammar; path base N/A. | Entry scalar. | N/A. | Used to select and synthesize an output repo name. | Shared RPM-build/image profile data |
| `...subrepos[].subpath` | string | Required; no default. | Relative URI path with `/`, no empty, `.`, `..`, query, or fragment. `$basearch` and `$releasever` are the only placeholders. | Entry scalar. | N/A. | Expansion uses the exact algorithm below and must remain below the set `base-uri`. | Shared RPM-build/image profile data |
| `...subrepos[].kind` | string enum | Optional; default `binary`. | Exactly `binary`, `debug`, or `source`; path base N/A. | Entry scalar. | N/A. | Metadata only; unsupported value is an error. | Shared RPM-build/image profile data |

## Repository sets

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `resources.rpm-repo-sets` | Name-keyed map of closed set tables | Optional; default empty map. | Resource-name constraints. | Map by key and recursive table. | N/A. | Expanded names must not collide with explicit repos or another set. | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.description` | string | Optional; no default. | Diagnostic UTF-8; path base N/A. | Scalar replace. | N/A. | No processing semantics. | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.template` | string reference | Required; no default. | Exact template name; path base N/A. | Scalar replace. | N/A. | Missing template is an error. | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.base-uri` | URI string | Required; no default. | Canonical absolute `https` directory URI as defined below; no placeholders. | Scalar replace. | N/A. | Each substituted and encoded subpath is appended below it. | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.name-prefix` | string | Optional; default `<set>-`. | Resource-name characters; path base N/A. | Scalar replace. | N/A. | Prefix plus subrepo name must satisfy the resource-name grammar. | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.gpg-key` | URI or string path | Conditionally required. | Absolute `https` URI, or the [portable relative model-path grammar](./loading.md#portable-relative-model-paths) from the defining document with a regular-file target. | Scalar replace retaining path provenance. | N/A. | Required unless GPG checking is disabled. This field is a locator only; every expanded selected repository receives the same immutable [GPG-key binding](#repository-gpg-key-bindings). | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.disable-gpg-check` | boolean | Optional; default `false`. | N/A. | Scalar replace. | N/A. | Same GPG invariant as repository. | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.disable-ssl-verify` | boolean | Optional; default `false`. | N/A. | Scalar replace. | N/A. | The field is preserved for compatibility data, but `true` is forbidden operationally: every selected operation that references the set fails with `security-policy` at `V-OPERATION` before repository expansion, credentials, DNS, TLS, or other access. Certificate and hostname verification are never weakened. | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.arches` | Array of strings | Optional; empty means all. | Unique architecture tokens. | Array replace. | N/A. | Filters every expanded subrepo. | Shared RPM-build/image profile data |
| `resources.rpm-repo-sets.<set>.subrepos` | Array of strings | Optional; default is every template subrepo in template order. | Unique exact subrepo names. | Array replace. | N/A. | Unknown names are errors; explicit order controls expansion order. | Shared RPM-build/image profile data |

Before set expansion, `disable-ssl-verify = true` fails under the rule above.
Set expansion therefore produces repositories only from sets whose effective
value is `false`. It produces one effective `rpm-md` repository per selected
subrepo. The name is `name-prefix + subrepo.name`; shared GPG and architecture
values are copied. No expanded repository contains a TLS-verification opt-out.

A set `base-uri` is canonical only when the complete URI is ASCII, it has
scheme `https`, a non-empty ASCII host, no userinfo, query, or fragment, no dot
segment or percent-encoded path separator, uppercase hexadecimal in every
percent escape, no percent escape for an RFC 3986 unreserved byte, and a path
ending in `/`. Dot-segment checking occurs after percent decoding each segment.
It contains no `$` placeholder.

The final URI is constructed when a distro-version input reference and an
operation-supplied architecture are both known:

1. split `subpath` on literal `/`;
2. in each segment, replace every `$basearch` with the selected architecture
   and every `$releasever` with the referenced
   `distros.<d>.versions.<v>.release-ver`;
3. reject any remaining `$` placeholder or an empty, `.` or `..` substituted
   segment;
4. UTF-8 encode each substituted segment and percent-encode every byte except
   RFC 3986 unreserved bytes `ALPHA`, `DIGIT`, `-`, `.`, `_`, and `~`, using
   uppercase hexadecimal; and
5. append the encoded segments, separated by `/`, after the base URI's existing
   trailing `/`.

This is path-segment append, not RFC relative-reference resolution: the final
directory segment of `base-uri` is never replaced. Placeholder values
containing `/`, `?`, `#`, `%`, or non-ASCII text remain one encoded segment.
The output has no unresolved placeholder. For
`https://mirror.example.invalid/fedora/43/`,
`Everything/$basearch/os`, architecture `x86_64`, and release version `43`,
the exact URI is
`https://mirror.example.invalid/fedora/43/Everything/x86_64/os`.
The [resource URI fixture](./examples/resource-uri/README.md) includes this and
reserved-character cases.

## Repository GPG key bindings

The model `gpg-key` value identifies where key bytes originate; it is never
sufficient by itself for a selected `rpm-build` or `image-build` operation.
For every effective selected repository whose GPG checking is enabled, the
operation's immutable repository-input manifest MUST contain exactly one
closed key-binding record. The complete binding set is validated before any
repository metadata or package access. Each defect has one required portable
diagnostic:

The repository-key binding manifest is a narrow immutable input, not a
selected-operation request envelope. Its root contains exactly integer
`format-version = 1`, one `manifest-id` using the fixture-token grammar, and
one non-empty ordered `key-bindings` array. Unknown root keys, a missing root
key, another version, or a wrong TOML type is an error. The manifest carries no
target component, profile selection, macro/toolchain/package context, or
publication input.

| Defect | Diagnostic class | Validation phase | Requirement identity |
| --- | --- | --- | --- |
| A selected repository has no record | `unsupported-operation` | `V-OPERATION` | `REPO-GPG-BINDING-MISSING` |
| More than one record names one repository | `conflict` | `V-OPERATION` | `REPO-GPG-BINDING-DUPLICATE` |
| A record names no selected repository | `integrity` | `V-OPERATION` | `REPO-GPG-BINDING-UNSELECTED` |
| `source`, `locator`, canonical URI, or local provenance does not equal the effective repository input | `integrity` | `V-OPERATION` | `REPO-GPG-BINDING-LOCATOR` |
| The selected package-manager implementation rejects the snapshotted key material | `integrity` | `V-ACQUIRE` | `REPO-GPG-KEY-FORMAT` |
| The SHA-256 of snapshotted bytes does not equal the binding | `integrity` | `V-ACQUIRE` | `REPO-GPG-KEY-DIGEST` |

The diagnostic processing phase is the selected `rpm-build` or `image-build`
operation. Its subject is the effective repository identity; duplicate and
unselected diagnostics list the conflicting binding identities in `related`.
The first four defects stop at operation preflight. The byte-format and digest
checks occur after acquisition or local snapshot but before repository use.

Every record contains these common keys:

| Key | Requirement |
| --- | --- |
| `repository` | Exact effective repository identity, unique in the manifest. |
| `source` | Exactly `remote` or `local`; it discriminates the closed union below. |
| `locator` | Exact effective `gpg-key` string after composition and set expansion. |
| `sha256` | Lowercase 64-digit SHA-256 of the complete snapshotted key-file bytes. |

A `remote` record additionally contains exactly `canonical-uri`, equal to the
canonical request URI derived from `locator` by
[Sources](./sources.md#canonical-https-authority-and-origin). It forbids local
provenance keys. The processor fetches the key with the exact
[HTTPS artifact-fetch state machine](./sources.md#https-artifact-fetch-and-source-selection):
the operation's network context supplies trust, evaluation instant, proxy,
deadlines, and any credential reference scoped to
repository-key acquisition. Redirect request reconstruction, canonical-origin
credential stripping, loop identity, the complete logical request, response
framing, and single-attempt rules are unchanged. `hit(bytes)` with the record's exact
SHA-256 is the only accepted outcome; `not-found` does not select another key.

A `local` record additionally contains exactly `defining-document` and
`portable-path`. `defining-document` is the relocatable semantic identity of
the document that supplied the effective path, and `portable-path` is that
exact retained model path. It forbids `canonical-uri`. The processor resolves
the path from its defining document, verifies containment and regular-file
kind, snapshots the complete bytes and provenance, and compares `sha256`
before repository metadata or package access. A later file replacement cannot
change the frozen snapshot.

After either variant succeeds, the selected RPM or image package-manager
implementation owns key-format acceptance and signature verification. The
specification does not define an OpenPGP packet grammar, accepted fixture
vector, or package-manager-independent parser. Ambient keyrings, previously
imported keys, package-manager defaults, trust-on-first-use, live mutable key
bytes, and a key obtained from another repository are forbidden. Configured
material rejected by the selected implementation and digest mismatch use the
exact diagnostics above. A signature that does not validate under exactly the
accepted key set is `integrity` at `V-ACQUIRE` with requirement identity
`REPO-GPG-SIGNATURE`, and any attempt to use repository metadata or packages
before key and signature verification is `security-policy` at `V-ACQUIRE`
with requirement identity `REPO-GPG-VERIFY-BEFORE-USE`. None produces an
accepted repository input.

Set expansion copies the locator but does not copy an unverified live key:
every expanded effective repository identity has its own manifest binding,
which may intentionally name the same digest. The operation validates the
complete binding set before any repository metadata or package access. This
immutable input rule is inside the selected operation's deterministic
preflight and does not claim final RPM/image bytes.

The tracked
[repository-key fixture](./examples/repository-gpg-keys/README.md) covers a
redirected remote key, a local project-contained key snapshot, and the
binding/key diagnostic rows above.

## Distro-version inputs

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `distros.<d>.versions.<v>.inputs.rpm-build` | Ordered array of closed reference tables | Optional; default `[]`. | Path base N/A. | Array replace. | N/A. | Each entry sets exactly one of `repo` or `set`. | RPM-build profile |
| `distros.<d>.versions.<v>.inputs.image-build` | Ordered array of closed reference tables | Optional; default `[]`. | Path base N/A. | Array replace. | N/A. | Each entry sets exactly one of `repo` or `set`. | Image profile |
| `...inputs.<use-case>[].repo` | string reference | Conditionally required. | Exact effective repo name; path base N/A. | Entry scalar. | N/A. | Mutually exclusive with `set`; missing target is an error. | Owning use-case profile |
| `...inputs.<use-case>[].set` | string reference | Conditionally required. | Exact repo-set name; path base N/A. | Entry scalar. | N/A. | Mutually exclusive with `repo`; missing target is an error. | Owning use-case profile |

References expand in array order; a set expands in its selected subrepo order.
An effective repo name appearing more than once is an error. Order is retained
for serialization but does not imply package-manager priority.

The legacy `distros.<distro>.repos[]` RPM-build field is a replacing array of
closed tables:

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `distros.<distro>.repos` | Array of closed repository tables | Optional; default `[]`. | Path base N/A. | Array replace. | N/A. | Cannot be combined with `inputs.rpm-build`. | RPM-build profile |
| `distros.<distro>.repos[].base-uri` | URI template string | Required; no default. | Uses the exact [repository URI-template algorithm](#repository-uri-templates); `$releasever` and `$$` are permitted, while `$basearch` is forbidden. | Entry scalar. | N/A. | Unknown placeholders, invalid templates, or invalid final URIs are errors before repository access. | RPM-build profile |

This legacy field is accepted by the RPM-build profile but cannot be combined
with `inputs.rpm-build`; using both is an error.

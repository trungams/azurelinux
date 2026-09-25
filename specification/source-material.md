[Previous: Project model](./project-model.md) ·
[Overview](./index.md) ·
[Next: Transformations](./transformations.md)

# Source material

> **Non-normative reading guide.** This page explains the normal source
> workflow. The primary normative owner is
> [Source identity and acquisition](./sources.md); cross-cutting owners are
> linked where used. Follow the exact links for field, grammar, ordering,
> network, hash, transformation, and security requirements.

A component starts from either a local dist-git directory or an exact upstream
dist-git commit. It may also declare source artifacts such as tarballs, patches,
or generated archives. Each required identity or digest is verified at the
origin-specific acceptance boundary before the corresponding source result is
accepted.

**First-pass takeaways:**

- a component has one verified local or exact-commit upstream source;
- each source artifact is accepted only at its origin-specific hash boundary;
  and
- for an upstream component's configured download, the distro-hosted,
  hash-addressed lookaside is tried first and only `not-found` can select an
  enabled configured origin; a local component fetches its declared
  `origin.uri` directly.

## Choose the component source

For a local component, point `spec.path` at the RPM spec file:

```toml
[components.hello.spec]
type = "local"
path = "pkg/hello.spec"
```

The spec file's parent is the local source root. The local source identity
includes the normalized project-relative source root, each regular file's
path, bytes, and executable classification, and each portable symbolic link's
path and exact target. Empty directories and non-semantic host metadata do not
participate.

For an upstream component, name the source distro and version and pin the
repository to a full commit:

```toml
[components.zlib.spec]
type = "upstream"
upstream-name = "zlib"
upstream-commit = "0123456789abcdef0123456789abcdef01234567"

[components.zlib.spec.upstream-distro]
name = "azurelinux"
version = "4.0"
```

`upstream-name` defaults to the component name. The exact local and upstream
variants, forbidden fields, and defaults are owned by
[Upstream component source](./sources.md#upstream-component-source) and
[Local component source](./sources.md#local-component-source).

## Pin upstream content before acquisition

Branches and snapshots help a producer choose a commit; they are not final
source identities. The effective `upstream-commit` is the portable source
truth and is verified as a full commit in the selected repository before
checkout. Within one composed provider (one inheritance-layer partial
configuration after its source documents have been composed), document
loading and scalar composition decide among pin contributions. The effective
component then applies the fixed
[component inheritance order](./resolution.md#component-inheritance-order).
Overlapping group pin contributions are errors rather than choosing a winner.

The distro's `dist-git-base-uri` identifies the repository. Source URI
templates encode placeholder values as path data and are validated before
network access. See [Source URI templates](./sources.md#source-uri-templates)
for the exact tokens, cardinalities, encoding, and rejection rules.

## Add source artifacts

`source-files` adds or intentionally replaces top-level source artifacts. Each
entry declares an exact filename, digest, origin, and replacement policy:

```toml
[[components.hello.source-files]]
filename = "hello-1.0.tar.gz"
hash-type = "SHA256"
hash = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"

[components.hello.source-files.origin]
type = "local"
path = "files/hello-1.0.tar.gz"
```

For this local-origin example, the digest must match the exact bytes of that
local file. Available origins are direct HTTPS download, a project-contained
local file, a `custom-result` file carrying producer-asserted generated
provenance that core reads and hash-verifies, an archive transformed by
overlays, or optional custom generation. The exact closed variants are in
[Source artifact field contracts](./sources.md#source-artifact-field-contracts).

An upstream dist-git may also contain a top-level `sources` manifest. Its
records are parsed in order, filenames are unique, and artifact bytes must
match the declared digest. See
[Upstream `sources` manifest](./sources.md#upstream-sources-manifest).

## Acquire, verify, and resolve collisions

The normal order is: validate filenames, verify the upstream commit when
applicable, parse the upstream manifest, acquire and hash upstream artifacts,
then process configured `source-files` in array order. Additions and explicit
replacements produce one effective artifact set; an unexpected collision
fails the component attempt.

The exact order and failure boundary are in
[Acquisition and collision order](./sources.md#acquisition-and-collision-order).
Remote artifacts are fetched over HTTPS. A configured-origin fallback is
available only for an upstream component's configured download, after the
specified lookaside `not-found` outcome and only when origins are enabled. A
local component's configured download fetches its declared `origin.uri`
directly without a lookaside attempt. The exact canonical authority/origin
model and single-attempt fetch state machine are defined in
[Canonical HTTPS authority and origin](./sources.md#canonical-https-authority-and-origin)
and
[HTTPS artifact fetch and source selection](./sources.md#https-artifact-fetch-and-source-selection).
The focused
[lookaside outcome fixture](./examples/lookaside-outcomes/README.md) is the
quickest evidence for that one permitted fallback transition.

## Transform an upstream archive

An archive changed by archive-scoped overlays has a matching
`origin.type = "overlay"` entry. That entry replaces exactly one upstream
artifact and records the required post-overlay hash. The association is
defined in
[Overlay-origin association](./sources.md#overlay-origin-association); archive
batching, the semantic extracted tree, repacking, and hash verification are
owned by
[Overlay transformations](./overlays.md#archive-extraction-and-batching).

## Optional custom generation

`origin.type = "custom"` describes an optional
`custom-source-generate` operation. Core validates and preserves the fields but
does not execute the script. Generation occurs only through a separately and
explicitly selected `custom-source-generate` operation. When selected, the
implementation exposes only declared artifact inputs, makes the declared
package names available, and isolates the invocation. It validates the
semantic output tree, emits one supported archive, and accepts it only when
its configured hash matches.

The exact operation boundary and explicit non-goals are in
[Custom-source-generation profile](./sources.md#custom-source-generation-profile);
the isolation boundary is in
[Custom generator isolation](./security.md#custom-generator-isolation).
A producer may instead supply a `custom-result` file. That variant asserts
generated provenance; core only reads and hash-verifies the supplied bytes and
does not verify a generation history.

## Further reading

- [Source identity fixture](./examples/source-identity/README.md)
- [Local source identity fixture](./examples/local-source-identity/README.md)

[Continue: Transformations](./transformations.md)

[Previous: Project model](./project-model.md) ·
[Overview](./index.md) ·
[Next: Transformations](./transformations.md)

# Source material

This page shows how a component chooses its source, acquires declared
artifacts, and verifies them before transformation. The binding rules are in
[Source identity and acquisition](./sources.md); cross-cutting owners are
linked where they apply.

A local component starts from one dist-git directory. An upstream component
starts from one exact, verified commit. Both may add tarballs, patches, or
generated archives through `source-files`. The processor verifies each
identity or digest when its origin supplies the result. Only then can that
result enter the component's source set.

Downloads differ by component source. For an upstream component, a configured
download first checks the distro's hash-addressed lookaside. Only `not-found`
may select the configured `origin.uri`, and only when origins are enabled. A
local component skips the lookaside and fetches its declared `origin.uri`
directly.

## Choose the component source

For a local component, point `spec.path` at the RPM spec file:

```toml
[components.hello.spec]
type = "local"
path = "pkg/hello.spec"
```

The directory containing the spec is the local source root. Its identity
includes the normalized project-relative root path. It also includes every
regular file's path, bytes, and executable classification, plus every portable
symbolic link's path and exact target. Empty directories and non-semantic host
metadata don't participate.

For an upstream component, name the source distro and version and pin the
repository to one full commit:

```toml
[components.zlib.spec]
type = "upstream"
upstream-name = "zlib"
upstream-commit = "0123456789abcdef0123456789abcdef01234567"

[components.zlib.spec.upstream-distro]
name = "azurelinux"
version = "4.0"
```

`upstream-name` defaults to the component name. The local and upstream field
variants, including their forbidden fields and defaults, are defined in
[Upstream component source](./sources.md#upstream-component-source) and
[Local component source](./sources.md#local-component-source).

## Pin upstream content before acquisition

Branches and snapshots may help a producer choose a commit, but the processor
identifies the upstream source by the effective `upstream-commit`. Before
checkout, it verifies that the selected repository contains that full commit.

Pin contributions follow ordinary composition and inheritance. Within one
inheritance layer, several source documents may contribute a pin. Those
documents are composed first, so composition determines whether that layer
contributes a pin and, if so, its value. The component then applies the four
layers in the fixed
[component inheritance order](./resolution.md#component-inheritance-order).
Two groups cannot set the same pin; that overlap is an error.

The distro's `dist-git-base-uri` identifies the repository. Before network
access, the processor expands and validates its placeholders as path data.
The [Source URI templates](./sources.md#source-uri-templates) section defines
the allowed tokens, their cardinalities, encoding, and rejection rules.

## Add source artifacts

Use `source-files` to add or intentionally replace top-level source artifacts.
Each entry gives the filename, digest, origin, and replacement policy:

```toml
[[components.hello.source-files]]
filename = "hello-1.0.tar.gz"
hash-type = "SHA256"
hash = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"

[components.hello.source-files.origin]
type = "local"
path = "files/hello-1.0.tar.gz"
```

For this local origin, the digest must match the file's exact bytes. Other
origins cover a direct HTTPS download, a project-contained file, a transformed
upstream archive, and optional custom generation. A `custom-result` is a
supplied file that the producer identifies as generated. Core reads the file
and verifies its hash. The
[Source artifact field contracts](./sources.md#source-artifact-field-contracts)
section defines all closed variants.

An upstream dist-git may also contain a top-level `sources` manifest. The
processor parses its records in order and rejects duplicate filenames. It
accepts each artifact only when the acquired bytes match the recorded digest.
See [Upstream `sources` manifest](./sources.md#upstream-sources-manifest).

## Acquire, verify, and resolve collisions

The processor validates configured filenames before it reads source data. For
an upstream component, it then verifies the commit, parses the upstream
manifest, and acquires and hashes those artifacts. Configured `source-files`
are processed afterward in array order.

Additions and explicit replacements produce one effective artifact set. An
unexpected collision, a missing replacement target, or a hash failure aborts
the complete component attempt. [Acquisition and collision order](./sources.md#acquisition-and-collision-order)
defines the full sequence.

Remote artifacts use HTTPS. For an upstream component's configured download,
the processor checks the lookaside first. Only `not-found` can select
`origin.uri`, and only when origins are enabled. Once the origin is tried, any
failure ends acquisition. A local component skips the lookaside and uses
`origin.uri` directly.

[Canonical HTTPS authority and origin](./sources.md#canonical-https-authority-and-origin)
and [HTTPS artifact fetch and source selection](./sources.md#https-artifact-fetch-and-source-selection)
define the request and one-attempt behavior. The
[lookaside outcome fixture](./examples/lookaside-outcomes/README.md) shows the
sole upstream fallback.

## Transform an upstream archive

When archive-scoped overlays change an upstream archive, a matching
`origin.type = "overlay"` entry must replace exactly that one artifact. The
entry records the hash required after repacking.
[Overlay-origin association](./sources.md#overlay-origin-association) defines
that link. Archive grouping, the semantic extracted tree, repacking, and hash
verification are defined in
[Overlay transformations](./overlays.md#archive-extraction-and-batching).

## Optional custom generation

`origin.type = "custom"` describes data for the optional
`custom-source-generate` operation. Core validates and preserves the fields,
but it never runs the script. Generation happens only when that operation is
selected separately and explicitly. The implementation isolates the script,
exposes only its declared inputs, validates the output tree, and accepts the
archive only after its exact bytes match the configured hash.

A producer may instead supply a `custom-result` file. Core reads and
hash-verifies that file without running the generator or verifying its
history.

[Custom-source-generation profile](./sources.md#custom-source-generation-profile)
defines operation selection, output checks, and required resource-limit
inputs.
[Custom generator isolation](./security.md#custom-generator-isolation)
defines the sandbox, default-deny network access, and credential boundary.

## Further reading

- [Source identity fixture](./examples/source-identity/README.md)
- [Local source identity fixture](./examples/local-source-identity/README.md)

With the source inputs verified, the processor can assemble a private
candidate and run its overlays once. The next page explains that overlay work
before final validation and publication.
[Continue: Transformations](./transformations.md)

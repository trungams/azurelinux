[Previous: Materialized dist-git](./materialized-dist-git.md) ·
[Overview](./index.md) ·
[Next: Errors, determinism, and security](./errors-determinism-security.md)

# Optional workflows

Declaring profile data does not run any work. A processor still validates and
preserves that data, even when it cannot perform the related operation.

Work starts only when a caller selects one supported operation and supplies
all of its required inputs. Before network access, credential loading,
subprocess execution, build-root creation, or destination mutation, the
processor checks its capabilities and completes `V-OPERATION` preflight. If
the operation or an input is unsupported, it returns
`unsupported-operation` without a side effect.

## Declare profile data without executing it

This example attaches RPM-build settings and a pytest definition to a
component:

```toml
[components.hello.release]
calculation = "auto"

[components.hello.build]
with = ["feature"]
defines = { downstream = "1" }

[tests.smoke]
type = "pytest"
kind = "functional"

[tests.smoke.pytest]
working-dir = "tests/smoke"
test-paths = ["test_hello.py"]

[components.hello.tests]
tests = [{ name = "smoke" }]
```

The referenced project paths must exist under their owning contracts. Loading
this data does not build `hello` or run `smoke`. Execution requires a separate
operation request.

## Select one exact operation

The optional operation namespace contains exactly six identifiers:

- `rpm-build`;
- `publish-packages`;
- `image-build`;
- `publish-image`;
- `test`; and
- `custom-source-generate`.

Core loading, resolution, source acquisition, overlays, and materialized
dist-git use no optional operation identifier. Selecting one operation never
selects another: an RPM build does not publish packages, an image build does
not publish an image, and materialization does not run tests.

Each request names its target object or artifact and binds the required
distro, architecture, environment, security context, and observable result.
Those inputs are immutable for the attempt; the host cannot fill a missing
value later.

Details: [Optional profile data and operations](./profiles.md),
[Selected operation contracts](./profiles.md#selected-operation-contracts),
and the
[environmental-input record](./determinism.md#environmental-input-record).

## Build RPMs with explicit context

The RPM-build profile carries release calculation, build conditionals, mock
configuration, macros, and repository references. Release calculation is part
of `rpm-build`, not a seventh operation.

An `rpm-build` request supplies a complete RPM macro context, immutable
toolchain and repository/package manifests, and any repository credentials.
It cannot fall back to host RPM configuration, environment-derived macros,
the host architecture, or an undeclared repository. RPM and image builds can
share repository definitions, but declaring a repository starts neither
operation.

When GPG checking is enabled, every selected repository has an immutable key
binding. The key bytes and digest are verified before repository metadata or
packages are trusted. Package-manager-specific key-format and signature
acceptance remain with the selected package-manager implementation.

See [Build and release configuration](./build.md), especially
[Build configuration](./build.md#build-configuration). Repository expansion
and trust are in [Resources and repository inputs](./resources.md),
[RPM repository resources](./resources.md#rpm-repository-resources), and
[Repository GPG key bindings](./resources.md#repository-gpg-key-bindings).
The macro and toolchain boundary is in
[RPM macro and external tool context](./determinism.md#rpm-macro-and-external-tool-context).
Final RPM and SRPM bytes are outside the revision `0.1` conformance output.

## Build and publish images separately

`image-build` receives one selected image and one of its listed architectures,
plus the immutable repository, package, and toolchain inputs for that image.
`publish-image` is a separate request over the built image's identity and
digest. It preserves the declared channel order and uses only credentials
scoped to that route.

Each image entry names one supported `kiwi` definition, its allowed
architectures, capabilities, tests, and publication channels. A required
capability is satisfied only when it is explicitly `true`; unknown and explicit
`false` both fail the requirement. Profile field details are in
[Image profile](./images.md).

Final image bytes and cross-tool image reproducibility are not standardized.
Remote image publication has the same no-replay and no-rollback boundary as
package publication.

See the [image build and publishing rules](./images.md) and the image rows in
[Selected operation contracts](./profiles.md#selected-operation-contracts).

## Define and run tests explicitly

The test profile supports the defined portable subsets of pytest, LISA, and
TMT. Test definitions, groups, component references, and image references are
validated as model data. Group expansion preserves declared order and cannot
nest groups.

An explicit `test` operation names the test or group and the component or
image under test. The request binds required capabilities, an immutable
runner/toolchain manifest, network context, resource limits, permitted
credential references, and architecture only when the selected runner needs
one. For TMT, the plan is an exact absolute identity beginning with `/` and is
passed unchanged to the pinned runner.

Runner timing, performance measurements, external service state, and a
universal test-result byte format are not portable outputs. See
[Test profile](./tests.md) and
[Test definitions](./tests.md#test-definitions).

## Resolve package routes before publishing

Before publishing, the processor resolves a route for each package in provider
order. It uses the evaluated package identity; it does not infer the package
kind from a filename suffix. An absent route, or one set to `none`, skips that
artifact. Profile field details are in
[Packages and publishing profile](./packages.md).

`publish-packages` preserves the supplied artifact order after removing
unrouted artifacts. Each remaining artifact has a digest, evaluated route,
destination, and credential reference scoped under
[Credentials and sensitive values](./security.md#credentials-and-sensitive-values).

Revision `0.1` does not standardize a publication wire protocol, replay,
retry, receipt, remote transaction, or rollback. A remote error produces no
portable success result, but it does not prove that a remote system reversed
an earlier effect.

See the [package routing and publishing rules](./packages.md) and
[Publishing boundary](./profiles.md#publishing-boundary).

## Run custom source generation only when selected

A `source-files` entry with `origin.type = "custom"` declares optional
executable generation. Core validates and preserves it but never runs the
script. Only `custom-source-generate` may execute the snapshotted script, and
only after its architecture, evaluation instant, declared artifact inputs,
declared package names, isolation support, and limits have passed preflight.

The selected implementation exposes only declared artifact inputs and makes
the declared package names available. It receives no credentials in revision
`0.1`, and network access is denied by default. It consumes only the semantic
output beneath `/azldev-gen/output/`. The resulting archive is accepted only
after semantic validation and exact configured-hash verification.

Execution-root bytes, installed-package closure, kernel-observation proof, and
a universal archive encoder are deferred. A producer can instead supply a
`custom-result` file with producer-asserted generated provenance; core reads
and hash-verifies that file without executing a generator.

Details:
[Custom-source generation](./sources.md#custom-source-generation-profile) and
[Custom generator isolation](./security.md#custom-generator-isolation).

## Further reading

- [Optional profile behavior fixtures](./examples/profile-behavior/README.md)
- [Field/profile matrix](./profile-matrix.tsv)
- [Network and external validity context](./determinism.md#network-and-external-validity-context)

The next page explains the failure, determinism, and security rules shared by
these operations.

[Continue: Errors, determinism, and security](./errors-determinism-security.md)

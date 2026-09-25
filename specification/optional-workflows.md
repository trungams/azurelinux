[Previous: Materialized dist-git](./materialized-dist-git.md) ·
[Overview](./index.md) ·
[Next: Errors, determinism, and security](./errors-determinism-security.md)

# Optional workflows

> **Non-normative reading guide.** This page explains how optional data and
> explicit operations relate to core processing. The primary normative owners
> are [Optional profile data and operations](./profiles.md),
> [Build and release configuration](./build.md),
> [Resources and repository inputs](./resources.md),
> [Packages and publishing](./packages.md), [Image profile](./images.md),
> [Test profile](./tests.md), and
> [Custom-source generation](./sources.md#custom-source-generation-profile);
> cross-cutting determinism and security owners are linked where used.

Optional fields are still portable configuration. A processor validates and
preserves them even when it does not implement the associated operation.
Their presence never starts a build, publication, image, test, or generator.

**First-pass takeaways:**

- profile data is always validated and preserved;
- work begins only after one explicit operation is selected and preflighted;
  and
- final RPM/image reproducibility and remote publication rollback remain
  outside revision `0.1`.

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
explicit operation request with all operation-time inputs.

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

Before side effects, the processor checks capability support and all required
operation inputs. Missing support or an incomplete explicit environment
returns `unsupported-operation` before network access, credential loading,
subprocess execution, build-root creation, or destination mutation. See
[Selected operation contracts](./profiles.md#selected-operation-contracts)
and the immutable
[environmental-input record](./determinism.md#environmental-input-record).

## Build RPMs with explicit context

The RPM-build profile includes release calculation, build conditionals and
macros, selected mock configuration, and repository inputs. The selected
operation supplies the exact resolved component, materialized tree identity,
target distro/version, target architecture, complete RPM macro context,
repositories and immutable manifests, toolchain, evaluation instant, network
context, resource limits, and any scoped credential references.

Host RPM configuration, environment-derived macros, implicit architecture,
and undeclared repositories cannot fill a missing input. Repository resources
are shared optional data for RPM and image builds; merely declaring them
activates neither operation.

When GPG checking is enabled, every selected repository has an immutable key
binding. The key bytes and digest are verified before repository metadata or
packages are trusted. Package-manager-specific key-format and signature
acceptance remain owned by the selected package-manager implementation.

Exact fields and resource expansion are in
[Build configuration](./build.md#build-configuration),
[RPM repository resources](./resources.md#rpm-repository-resources), and
[Repository GPG key bindings](./resources.md#repository-gpg-key-bindings).
The complete macro and toolchain boundary is in
[RPM macro and external tool context](./determinism.md#rpm-macro-and-external-tool-context).
Final RPM and SRPM bytes are outside the revision `0.1` conformance output.

## Build and publish images separately

An image definition names one supported `kiwi` definition, allowed
architectures, optional capabilities, tests, and publication channels.
Capabilities have no implicit false default: a required capability is
satisfied only by explicit `true`.

`image-build` receives an explicit architecture and the complete selected
repository, package, toolchain, network, and resource context. `publish-image`
is a separate operation over an exact image identity and digest plus ordered
channels, destination, network context, limits, and route-scoped credentials.
Final image bytes and cross-tool image reproducibility are not standardized.

See [Image profile](./images.md) and the image rows in
[Selected operation contracts](./profiles.md#selected-operation-contracts).

## Define and run tests explicitly

The test profile supports the defined portable subsets of pytest, LISA, and
TMT. Test definitions, groups, component references, and image references are
validated as model data. Group expansion preserves declared order and cannot
nest groups.

An explicit `test` operation selects the exact test or group, resolved
component or image, required capabilities, runner/toolchain manifest, any
needed architecture, network context, limits, and permitted credentials.
For TMT, the plan is an exact absolute identity beginning with `/` and is
passed unchanged to the pinned runner.

Runner timing, performance measurements, external service state, and a
universal test-result byte format are not portable outputs. Exact framework
fields are in [Test definitions](./tests.md#test-definitions).

## Resolve package routes before publishing

Publishing data resolves package channels through its own fixed provider
order. It distinguishes ordinary, debuginfo, debugsource, and source packages
and uses exact evaluated package identities rather than suffix guessing.
Absent or `none` routes do not publish an artifact.

`publish-packages` receives an exact ordered artifact list, digests, evaluated
routes, destination identities, network context, resource limits, and
route-scoped credential references under
[Credentials and sensitive values](./security.md#credentials-and-sensitive-values).
The implementation preserves that order through preflight and invocation.

Revision `0.1` does not standardize a publication wire protocol, replay,
retry, receipt, remote transaction, or rollback. A remote error produces no
portable success result, but it does not prove that a remote system reversed
an earlier effect. See [Packages and publishing profile](./packages.md) and
[Publishing boundary](./profiles.md#publishing-boundary).

## Run custom source generation only when selected

A `source-files` entry with `origin.type = "custom"` declares optional
executable generation. Core validates and preserves it but never runs the
script. `custom-source-generate` receives the exact source entry, snapshotted
script, explicit architecture and evaluation instant, declared input artifact
bytes, declared package names, isolation availability, and resource limits.

The selected implementation exposes only declared artifact inputs, makes the
declared package names available, receives no credentials in revision `0.1`,
has network access denied by default, and consumes only the semantic output
beneath `/azldev-gen/output/`. The resulting archive is accepted only after
semantic validation and exact configured-hash verification. The exact
boundary is [Custom generator isolation](./security.md#custom-generator-isolation).

Execution-root bytes, installed-package closure, kernel-observation proof, and
a universal archive encoder are deferred. A producer can instead supply a
`custom-result` file with producer-asserted generated provenance; core reads
and hash-verifies that file without executing a generator.

## Further reading

- [Profile behavior fixture](./examples/profile-behavior/README.md)
- [Field/profile matrix](./profile-matrix.tsv)
- [Network and external validity context](./determinism.md#network-and-external-validity-context)

[Continue: Errors, determinism, and security](./errors-determinism-security.md)

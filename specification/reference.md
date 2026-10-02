[Previous narrative guide: Errors, determinism, and security](./errors-determinism-security.md) ·
[Overview](./index.md)

# Reference

Use this page to find the detailed chapter, matrix, inventory, or fixture for
an implementation task. It adds no requirements.

Binding prose lives in the linked normative chapters. A matrix is authoritative
only where its owner says so. Inventories and fixtures support lookup,
traceability, and validation; they do not replace those rules.

Read the narrative guides first. Return here when implementing a field,
operation, validator, diagnostic, or fixture.

## Find by task

- **Field lookup:** start with the
  [top-level object model](./objects.md), then use the
  [field disposition inventory](./field-disposition.tsv) or
  [opaque framework inventory](./opaque-field-disposition.tsv) to find its
  prose owner.
- **Document loading, includes, or project-side model paths:** use
  [Document format](./document.md) and
  [Document loading and composition](./loading.md).
- **Inheritance or reference resolution:** use
  [Resolution](./resolution.md) and
  [Component groups and discovery](./component_groups.md).
- **Source acquisition:** use
  [Source identity and acquisition](./sources.md).
- **Overlay operations:** use
  [Overlay transformations](./overlays.md) with the normative
  [operation matrix](./overlay-operation-matrix.tsv).
- **Materialized tree, `sources`, provenance, or local publication:** use
  [Materialized dist-git artifacts](./artifacts.md).
- **Selected optional operation or preflight:** use
  [Selected operation contracts](./profiles.md#selected-operation-contracts).
- **Diagnostics, determinism, or security:** use
  [Validation](./validation.md), [Determinism](./determinism.md), and
  [Security](./security.md).
- **Fixtures:** choose a group under [Focused fixtures](#focused-fixtures),
  then read its README for the represented success or failure.

## Documents, loading, and resolution

- [Document format](./document.md): UTF-8 TOML, root and fragment identity,
  strict keys, TOML types, equality, and source-model validation.
- [Document loading and composition](./loading.md): project roots, portable
  paths and patterns, includes, traversal, repeated reaches, provenance, and
  composition.
- [Resolution](./resolution.md): model stages, phase order, target selection,
  inheritance, references, source identities, and resolved-model behavior.
- [Parsing and processing entry points](./parsing.md): compatibility
  route for older chapter links; it adds no requirement.

## Object and field owners

- [Top-level object model](./objects.md): root vocabulary, contract notation,
  names, and profile dispositions.
- [Project](./project.md): project description and default source distro.
- [Distros and versions](./distros.md): source URI bases, versions, release
  values, distro defaults, and profile inputs.
- [Components](./components.md): reusable `ComponentConfig`, overlays, source
  files, build, publishing, and test participation.
- [Component groups and discovery](./component_groups.md): membership,
  discovery patterns, synthesized components, and group defaults.
- [Source identity and acquisition](./sources.md): local/upstream source
  identity, exact commits, URI templates, manifests, artifacts, HTTPS
  acquisition, collisions, overlay origins, and custom generation.
- [Build and release configuration](./build.md): RPM-build data and release
  calculation.
- [Resources and repository inputs](./resources.md): repositories, templates,
  sets, URI expansion, GPG-key bindings, and distro-version references.
- [Packages and publishing profile](./packages.md): package identities,
  inheritance, routes, and package publication inputs.
- [Image profile](./images.md): definitions, architectures, capabilities,
  tests, and channels.
- [Test profile](./tests.md): pytest, LISA, TMT, references, and groups.
- [Optional profile data and operations](./profiles.md): core/profile split,
  six operation identifiers, preflight, publishing boundaries, and excluded
  outputs.

## Transformations and artifacts

- [Overlay transformations](./overlays.md): common fields and pipeline,
  matching, bounded spec parsing, archive semantics, all 17 operations,
  metadata, and claim status.
- [Overlay operation matrix](./overlay-operation-matrix.tsv): the normative
  required, optional, or forbidden common fields for each operation.
- [Materialized dist-git artifacts](./artifacts.md): candidate assembly,
  namespace, placement, `sources` bytes, behavioral identity, provenance,
  atomic publication, and transformed-archive byte boundary.

## Validation, determinism, and security

- [Determinism and environmental inputs](./determinism.md): immutable
  environmental records, ambient-state exclusions, network context, RPM
  macros, tools, randomness, and limits.
- [Environmental-input matrix](./environmental-input-matrix.tsv): the
  normative classification of every tracked influence.
- [Validation and diagnostics](./validation.md): nine validation phases,
  diagnostic fields and classes, redaction, failure, and rollback.
- [Security and authorization](./security.md): credentials, HTTPS, paths,
  publication, archives, script isolation, and denial-of-service limits.
- [Conformance examples and fixture format](./conformance.md): class status,
  boundary descriptions, ordinary fixture format, execution guidance,
  traceability, and explicit non-goals.

## Inventories and matrices

- [Field disposition inventory](./field-disposition.tsv): schema paths and
  their owners.
- [Opaque framework disposition inventory](./opaque-field-disposition.tsv):
  fields beneath schema-opaque test framework tables.
- [Field/profile matrix](./profile-matrix.tsv): structural participation and
  core/profile ownership; it does not select an operation.
- [Overlay operation matrix](./overlay-operation-matrix.tsv): required,
  optional, or forbidden common-field classifications for every operation.
- [Environmental-input matrix](./environmental-input-matrix.tsv): the
  normative explicit, fixed, prohibited, irrelevant, and external-failure
  classifications.

The overlay and environmental matrices carry the classifications that their
owners mark normative. The field inventories and field/profile matrix support
traceability and ownership lookup. None of them supersedes the prose owner.

## Focused fixtures

### Documents, paths, composition, and resolution

Use these fixtures for document roles, includes, paths, composition, target
selection, inheritance, discovery, or provenance.

- [Document loading fixtures](./examples/loading/README.md)
- [Include lexical grammar](./examples/include-lexical/README.md)
- [Repeated canonical reaches](./examples/repeated-reaches/README.md)
- [Relocatable provenance](./examples/provenance/README.md)
- [Portable paths](./examples/portable-paths/README.md)
- [TOML equality](./examples/toml-equality/README.md)
- [Document-control key paths](./examples/control-key-paths/README.md)
- [Source-model validation](./examples/source-model-validation/README.md)
- [Top-level object fixtures](./examples/object-model/README.md)
- [Target distro selection](./examples/target-distro/README.md)
- [Inheritance overlap](./examples/inheritance-overlap/README.md)
- [Component discovery](./examples/component-discovery/README.md)
- [Distro fallback](./examples/distro-fallback/README.md)
- [Pin order](./examples/pin-order/README.md)

### Sources and acquisition

Use these fixtures for source identity, URI expansion, manifests, lookaside
selection, repository keys, or custom-source inputs.

- [Source URI templates](./examples/source-uri-template/README.md)
- [Source identity fixtures](./examples/source-identity/README.md)
- [Local source identity](./examples/local-source-identity/README.md)
- [Upstream `sources` manifests](./examples/sources-manifest/README.md)
- [Lookaside outcomes](./examples/lookaside-outcomes/README.md)
- [Resource URI expansion](./examples/resource-uri/README.md)
- [Repository GPG keys](./examples/repository-gpg-keys/README.md)
- [Custom generator](./examples/custom-generator/README.md)

### Transformations and artifacts

Use these fixtures for overlay results and failures, archive batching,
provenance, final-tree identity, or atomic publication.

- [Overlay operation fixtures](./examples/overlay-operations/README.md)
- [Overlay provenance](./examples/overlay-provenance/README.md)
- [Materialized artifact](./examples/materialized-artifact/README.md)

### Profiles, validation, and conformance

Use these fixtures for optional-operation preflight, framework fields,
publishing precedence, environmental influence, or ordinary fixture format.

- [Framework fields](./examples/framework-fields/README.md)
- [Optional profile behavior fixtures](./examples/profile-behavior/README.md)
- [Publishing precedence](./examples/publishing-precedence/README.md)
- [Locale, timezone, and wall-clock fixture](./examples/determinism/README.md)
- [Conformance manifest](./examples/conformance/README.md)

Fixtures show representative successes and failures and support validation.
They are not a suite registry, class claim, canonical serialization, or
replacement for an owner.

## Exclusions and deferred work

- [Excluded, deferred, and compatibility fields](./compatibility.md) is the
  binding disposition for characterized keys that `0.1` does not accept, plus
  the one accepted deprecated publishing alias.
- [Decision status and open decisions](./open-decisions.md) is non-normative
  history for resolved choices and future-version topics.
- [Conformance deferrals and non-goals](./conformance.md#explicit-deferrals-and-non-goals)
  collects the revision `0.1` non-goals. They cover canonical serialization,
  replay and claim infrastructure, portable generator closure, OpenPGP
  internals, canonical archive encoding, final RPM/image reproducibility,
  long-term version policy, and future semantic overlays.

## Return to the narrative path

Return to the [Overview](./index.md#read-next) for the seven-guide narrative
path.

[Previous narrative guide: Errors, determinism, and security](./errors-determinism-security.md) ·
[Overview](./index.md)

# Reference

> **Non-normative navigation guide.** This page groups the exhaustive owners,
> matrices, fixtures, compatibility rules, and deferred work. It adds no field
> or processing requirement. Only normative chapters and matrices explicitly
> identified by their owners as normative are authoritative. Inventories and
> fixtures are traceability or evidence and do not replace prose contracts.

Use the narrative path for a first reading. Use this page when implementing a
field, operation, validator, diagnostic, or fixture.

## Find by task

- **Field lookup:** start with the
  [top-level object model](./objects.md), then use the
  [field disposition inventory](./field-disposition.tsv) or
  [opaque framework inventory](./opaque-field-disposition.tsv) to locate the
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
  then read its README for the represented success/failure boundary.

## Documents, loading, and resolution

- [Document format](./document.md): UTF-8 TOML, root and fragment identity,
  strict keys, TOML types, equality, and source-model validation.
- [Document loading and composition](./loading.md): root discovery, portable
  paths and patterns, includes, traversal, repeated reaches, provenance, and
  composition.
- [Resolution](./resolution.md): source/composed/resolved models, phase order,
  target selection, inheritance, references, source identities, and resolved
  behavioral identity.
- [Parsing and processing entry points](./parsing.md): compatibility
  navigation for earlier chapter links; it defines no additional requirement.

## Object and field owners

- [Top-level object model](./objects.md): closed root vocabulary, contract
  notation, names, and profile dispositions.
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
- [Build and release configuration](./build.md): RPM-build fields and release
  behavior.
- [Resources and repository inputs](./resources.md): repositories, templates,
  sets, URI expansion, GPG-key bindings, and distro-version references.
- [Packages and publishing profile](./packages.md): package identities,
  inheritance, routes, and package publication inputs.
- [Image profile](./images.md): definitions, architectures, capabilities,
  tests, and channels.
- [Test profile](./tests.md): pytest, LISA, TMT, references, and groups.
- [Optional profile data and operations](./profiles.md): core/profile split,
  the six operation identifiers, preflight, publishing boundary, and output
  exclusions.

## Transformations and artifacts

- [Overlay transformations](./overlays.md): common fields and pipeline,
  matching, bounded spec parsing, archive semantics, all 17 operations,
  metadata, and claim status.
- [Overlay operation matrix](./overlay-operation-matrix.tsv): one row per
  operation and every required, optional, or forbidden common-field cell.
- [Materialized dist-git artifacts](./artifacts.md): candidate assembly,
  namespace, placement, `sources` bytes, behavioral identity, provenance,
  atomic publication, and transformed-archive byte boundary.

## Validation, determinism, and security

- [Determinism and environmental inputs](./determinism.md): immutable
  environmental records, ambient-state exclusions, network context, RPM
  macros, tools, randomness, and limits.
- [Environmental-input matrix](./environmental-input-matrix.tsv): normative
  classification of every tracked influence.
- [Validation and diagnostics](./validation.md): nine validation phases,
  diagnostic fields and classes, redaction, failure, and rollback.
- [Security and authorization](./security.md): credentials, HTTPS, paths,
  publication, archives, script isolation, and denial-of-service limits.
- [Conformance examples and fixture format](./conformance.md): current class
  status, detailed boundaries, strict ordinary fixture format, execution
  guidance, traceability, and explicit non-goals.

## Inventories and matrices

- [Field disposition inventory](./field-disposition.tsv): all 508
  characterized schema paths and their owners.
- [Opaque framework disposition inventory](./opaque-field-disposition.tsv):
  the 52 fields beneath schema-opaque test framework tables.
- [Field/profile matrix](./profile-matrix.tsv): structural applicability and
  core/profile ownership.
- [Overlay operation matrix](./overlay-operation-matrix.tsv): 17 operations
  and 221 field-classification cells.
- [Environmental-input matrix](./environmental-input-matrix.tsv): explicit,
  fixed, prohibited, irrelevant, and external-failure influences.

These files support traceability and validation; they do not replace the prose
contracts.

## Focused fixtures

### Documents, paths, composition, and resolution

Choose this group for root/fragment roles, include traversal, portable paths,
composition, target selection, inheritance, discovery, or provenance.

- [Loading](./examples/loading/README.md)
- [Include lexical grammar](./examples/include-lexical/README.md)
- [Repeated canonical reaches](./examples/repeated-reaches/README.md)
- [Relocatable provenance](./examples/provenance/README.md)
- [Portable paths](./examples/portable-paths/README.md)
- [TOML equality](./examples/toml-equality/README.md)
- [Document-control key paths](./examples/control-key-paths/README.md)
- [Source-model validation](./examples/source-model-validation/README.md)
- [Object model](./examples/object-model/README.md)
- [Target distro selection](./examples/target-distro/README.md)
- [Inheritance overlap](./examples/inheritance-overlap/README.md)
- [Component discovery](./examples/component-discovery/README.md)
- [Distro fallback](./examples/distro-fallback/README.md)
- [Pin order](./examples/pin-order/README.md)

### Sources and acquisition

Choose this group for commit/source identity, URI expansion, manifest parsing,
lookaside selection, repository keys, or custom-source inputs.

- [Source URI templates](./examples/source-uri-template/README.md)
- [Source identity](./examples/source-identity/README.md)
- [Local source identity](./examples/local-source-identity/README.md)
- [Upstream `sources` manifests](./examples/sources-manifest/README.md)
- [Lookaside outcomes](./examples/lookaside-outcomes/README.md)
- [Resource URI expansion](./examples/resource-uri/README.md)
- [Repository GPG keys](./examples/repository-gpg-keys/README.md)
- [Custom generator](./examples/custom-generator/README.md)

### Transformations and artifacts

Choose this group for operation effects and failures, archive batching,
overlay provenance, final-tree identity, or atomic publication.

- [Overlay operations](./examples/overlay-operations/README.md)
- [Overlay provenance](./examples/overlay-provenance/README.md)
- [Materialized artifact](./examples/materialized-artifact/README.md)

### Profiles, validation, and conformance

Choose this group for optional-operation preflight, framework fields,
publishing precedence, environmental influence, or strict ordinary fixture
format.

- [Framework fields](./examples/framework-fields/README.md)
- [Profile behavior](./examples/profile-behavior/README.md)
- [Publishing precedence](./examples/publishing-precedence/README.md)
- [Determinism](./examples/determinism/README.md)
- [Conformance manifest](./examples/conformance/README.md)

Fixtures are representative falsifiers and evidence. They are not a suite
registry, class claim, canonical serialization, or replacement for an owner.

## Exclusions and deferred work

- [Excluded, deferred, and compatibility fields](./compatibility.md) is the
  normative disposition owner for characterized keys that conforming `0.1`
  documents do not accept, plus the one accepted deprecated publishing alias.
- [Decision status and open decisions](./open-decisions.md) is non-normative
  history for resolved choices and selected future-version topics.
- [Conformance deferrals and non-goals](./conformance.md#explicit-deferrals-and-non-goals)
  lists canonical serialization, replay, claim infrastructure, portable
  generator closure, OpenPGP internals, canonical archive encoding, final
  RPM/image reproducibility, long-term version policy, and future semantic
  overlays that revision `0.1` does not define.

## Return to the narrative path

Return to the [Overview](./index.md#read-next) for the seven-guide narrative
path.

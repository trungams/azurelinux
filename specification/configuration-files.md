[Overview](./index.md) ·
[Next: Project model](./project-model.md)

# Configuration files

> **Non-normative reading guide.** This page explains the normal file-loading
> workflow. The primary normative owners are
> [Document format](./document.md) and
> [Document loading and composition](./loading.md); cross-cutting owners are
> linked where used.

A project starts with one file named `azldev.toml`. Small projects can keep
their complete configuration there. Larger projects can split configuration
into fragments and list those fragments with `includes`.

## Start with the root file

The root file selects this specification revision and can immediately declare
project objects:

```toml
spec-version = "0.1"
includes = [
  "configuration/defaults.toml",
  "configuration/components/*.toml",
]

[project]
description = "Example distribution configuration"

[components.hello.spec]
type = "local"
path = "pkg/hello.spec"
```

The root-table `spec-version` document-control key appears only in the root
file. An included fragment omits that root-table key; the same string at
another path follows its owning field contract. A fragment may contain its own
root-table `includes` array. All files are strict UTF-8 TOML: invalid TOML,
duplicate assignments, unknown keys in closed tables, wrong types, and
noncanonical key spellings are errors. The exact root and fragment rules are
in [Root document identity](./document.md#root-document-identity),
[Included fragments](./document.md#included-fragments), and [Strict key
vocabulary](./document.md#strict-key-vocabulary).

The example's literal `configuration/defaults.toml` must exist. The glob may
match no files without failing. `path = "pkg/hello.spec"` remains relative to
the file that supplied that value, even if another file later contributes to
the same component.

## Resolve includes from the declaring file

Each include entry is relative to the directory of the file that declares it,
not always to the project root. `/` is the only separator. Absolute paths,
backslashes, URI-like paths, drive spellings, invalid glob syntax, and an
attempt to navigate above the project root are errors before a fragment is
loaded.

Literal segments and glob matches use exact Unicode spelling. Implementations
do not case-fold or normalize names. Symbolic links are resolved for
containment, and every selected fragment must remain inside the canonical
project root and resolve to a regular file. A missing literal is an error. A
valid glob with no matches contributes nothing.

These rules make a configuration portable across checkout locations while
rejecting host-specific path behavior. The exact grammar and failure cases are
in [The `includes` directive](./loading.md#the-includes-directive),
[Include-entry and glob grammar](./loading.md#include-entry-and-glob-grammar),
and [Match handling and containment](./loading.md#match-handling-and-containment).

## Follow one deterministic order

Loading begins with the root body. Include entries are then processed in array
order. Matches for one glob are sorted by normalized project-relative path
UTF-8 bytes, and each matched fragment's include subtree is completed
depth-first before loading the next match.

The declaring document therefore precedes everything it includes. Later
document bodies have higher precedence only where the owning composition rule
allows replacement. The physical position of `includes` inside a file does
not change the order.

A canonical document reached again outside the active recursion chain is not
applied twice. Its first deterministic reach supplies its body and traverses
its includes; later reaches add graph provenance only. Reaching a document
that is still active is a cycle and fails. See
[Traversal order](./loading.md#traversal-order) and
[Cycles and repeated reaches](./loading.md#cycles-and-repeated-reaches).

## Validate before merging

Every newly reached file is parsed and checked independently before its body
can participate in composition. A bad value in an earlier fragment cannot be
hidden by a valid value in a later file. Required values that may be supplied
elsewhere can remain absent from an individual fragment, but every value that
is present must already have a valid key, TOML type, nested shape, and
source-local constraints.

After every reached document is valid, composition processes bodies from
earliest to latest:

- tables and name-keyed maps merge recursively;
- a later scalar of the same TOML type replaces the earlier scalar;
- a later array replaces the complete earlier array, including with `[]`;
- different TOML value types conflict and fail; and
- `ComponentConfig.overlays` is the defined append-composed array exception.

Field chapters can define another exception only explicitly. For example,
`overlay-files` is a replacing array, while `overlays` appends in document
reach order. Composition is atomic: a load, type, or merge error produces no
composed model. The complete rules are in
[Composition sequence](./loading.md#composition-sequence) and
[Explicit composition exceptions](./loading.md#explicit-composition-exceptions).

Quick result sketch: if an earlier fragment supplies
`project.description = "Base"` and a later fragment supplies
`project.description = "Component"`, the later scalar wins. Two `overlays`
arrays append, while a later replacing array set to `[]` clears the earlier
array. An invalid value in the earlier fragment fails before any of those
composition results can hide it; the focused
[source-model validation falsifier](./examples/source-model-validation/README.md)
shows that boundary.

## Keep each value's origin

The composed model retains the source document that supplied each effective
leaf. Relative path values also retain the defining file's path base. Moving
the complete project to another absolute checkout directory does not change
that semantic provenance.

This distinction matters when a fragment supplies a component spec path, an
overlay source, a test path, or another project-side file. Later composition
and inheritance do not silently rebase the path to the root document or to the
file that refers to the resulting object. Exact provenance behavior is in
[Path provenance](./loading.md#path-provenance), and the composed result is
defined in [Composed model](./resolution.md#composed-model).

## Further reading

- [Portable relative model paths](./loading.md#portable-relative-model-paths)
- [Processing and validation order](./resolution.md#processing-and-validation-order)

[Continue: Project model](./project-model.md)

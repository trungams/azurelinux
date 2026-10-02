[Overview](./index.md) ·
[Next: Project model](./project-model.md)

# Configuration files

This guide follows the loader from `azldev.toml` through included fragments to
one composed model. [Document format](./document.md) and
[Document loading and composition](./loading.md) contain the binding rules.

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

`spec-version` is a document-control key only at the root of a document. The
root document must contain it, while included fragments must omit it. A
fragment may still contain `includes`, and a nested field named `spec-version`
follows that field's own definition.

Every file must be strict UTF-8 TOML. Invalid TOML, duplicate assignments,
unknown keys in closed tables, wrong types, and noncanonical key spellings are
errors.

[Root document identity](./document.md#root-document-identity) and
[Included fragments](./document.md#included-fragments) define the two document
roles.

[Strict key vocabulary](./document.md#strict-key-vocabulary) defines accepted
field spellings.

The example's literal `configuration/defaults.toml` must exist. The glob may
match no files without failing. `path = "pkg/hello.spec"` remains relative to
the file that supplied that value, even if another file later contributes to
the same component.

## Resolve includes from the declaring file

Each include path is `/`-separated, relative to the file that declares it, and
confined to the project root. Host-specific or malformed paths fail before
loading. Matching uses exact Unicode spelling: a missing literal fails, while
a valid glob with no matches contributes nothing. The loader resolves the
include path and any symlinks, then requires the resulting target to be a
regular file inside the canonical project root.

These rules keep configuration portable across checkout locations and reject
host-specific path behavior.

Details: [The `includes` directive](./loading.md#the-includes-directive),
[Include-entry and glob grammar](./loading.md#include-entry-and-glob-grammar),
and [Match handling and containment](./loading.md#match-handling-and-containment).

## Follow one deterministic order

Loading begins with the root body. The loader processes include entries in
array order. It sorts one glob's matches by the UTF-8 bytes of their normalized
project-relative paths. It finishes each matched fragment's include subtree
depth-first before loading the next match.

The declaring document therefore precedes everything it includes. Later
document bodies have higher precedence only where that field's composition
rules allow replacement. The physical position of `includes` inside a file
does not change the order.

If the loader encounters the same file again after finishing its first
traversal, it records the additional include relationship but does not apply
the file or follow its includes again. Encountering a file that is still being
loaded is a cycle and fails.

Details: [Traversal order](./loading.md#traversal-order) and
[Cycles and repeated reaches](./loading.md#cycles-and-repeated-reaches).

## Validate before merging

The loader parses and checks every newly reached file before its body can
participate in composition. A valid value in a later file cannot hide a bad
value in an earlier fragment. A fragment may omit a required value when
another file can supply it, but every value that is present must already have
a valid key, TOML type, nested shape, and source-local constraints.

After every reached document is valid, composition processes bodies from
earliest to latest:

- Tables and name-keyed maps merge recursively.
- A later scalar of the same TOML type replaces the earlier scalar.
- A later array replaces the complete earlier array, including with `[]`.
- Different TOML value types conflict and fail.
- `ComponentConfig.overlays` is the defined append-composed array exception.

A field follows these composition rules unless its own definition states an
exception. For example,
`overlay-files` is a replacing array, while `overlays` appends in document
reach order. Composition is atomic. A load, type, or merge error produces no
composed model.

Details: [Composition sequence](./loading.md#composition-sequence) and
[Explicit composition exceptions](./loading.md#explicit-composition-exceptions).

For example, suppose an earlier fragment supplies
`project.description = "Base"` and a later fragment supplies
`project.description = "Component"`. The later scalar wins. Two `overlays`
arrays append. A later replacing array set to `[]` clears the earlier array.
An invalid value in the earlier fragment fails before any of those composition
results can hide it. The focused
[source-model validation example](./examples/source-model-validation/README.md)
shows that boundary.

## Keep each value's origin

The composed model remembers which source document supplied each leaf.
Relative path values also retain the defining file's path base. Moving the
complete project to another absolute checkout directory does not change
either fact.

This distinction matters when a fragment supplies a component spec path, an
overlay source, a test path, or another project-side file. Later composition
and inheritance do not silently rebase that path to the root document or to
the file that refers to the resulting object.

Details: [Path provenance](./loading.md#path-provenance) and
[Composed model](./resolution.md#composed-model).

## Further reading

- [Portable relative model paths](./loading.md#portable-relative-model-paths)
- [Processing and validation order](./resolution.md#processing-and-validation-order)

[Continue: Project model](./project-model.md)

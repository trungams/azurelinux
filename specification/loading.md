[Return to index](./index.md)

# Document loading and composition

This chapter defines the forward processing architecture by which source
documents become one composed model. Document syntax and identity are defined in
[Document format](./document.md).

## Host filesystem name adapter

Document names and include matching operate on Unicode scalar-value sequences,
not on implementation-specific path strings. Whenever a processor enumerates a
directory for root discovery, literal-segment lookup, or glob expansion, it
MUST convert every returned host entry name to a Unicode scalar-value sequence
before comparing or matching names.

On a byte-oriented filesystem, this conversion MUST decode the original name
bytes as UTF-8 with strict error handling. On a filesystem whose native name
API is Unicode, every returned name MUST consist only of Unicode scalar values.
A processor MUST NOT synthesize replacement characters, expose surrogate code
points, or use another locale-dependent decoding. U+FFFD is an ordinary scalar
only when that scalar is present in the original host name.

If any entry returned by a directory enumeration cannot be converted under
this rule, that root discovery, literal-segment lookup, or pattern expansion is
an error. The processor MUST NOT ignore the entry as unmatchable. This
deliberately means that an otherwise unrelated undecodable name in a directory
that must be completely enumerated makes the operation fail rather than making
its result host-API-dependent.

After conversion, every directory enumeration and glob candidate set MUST
exclude entries whose complete name is `.` or `..`, whether or not the host API
returns those pseudoentries. Real names beginning with `.` and containing one
or more additional scalar values remain ordinary candidates.

All other comparisons after conversion use the exact Unicode scalar-value
sequence. A processor MUST NOT apply case folding, locale collation, Unicode
normalization, or host-equivalent-name lookup.

## Root discovery and project root

The root source document is named `azldev.toml`. Processing begins with either
that file or an implementation-supplied project directory containing that file.
Automatic ancestor-directory search is not part of this specification.

Before loading includes, a processor MUST:

1. enumerate the supplied project directory, or the parent directory of an
   explicitly supplied root path, through the
   [host filesystem name adapter](#host-filesystem-name-adapter);
2. select an entry whose scalar-value sequence is exactly `azldev.toml`;
3. reject a missing exact spelling, including a host-equivalent case-folded or
   normalized spelling;
4. require the selected entry to resolve to a regular file;
5. resolve symbolic links in the root document path;
6. define the **project root** as the canonical parent directory of that file;
   and
7. parse and validate the root document as defined by
   [Document-layer validation](./document.md#document-layer-validation).

When an explicit root path is supplied, the exact-name enumeration MUST select
that same directory entry; the processor MUST NOT substitute an exact-spelling
sibling for a differently spelled supplied path.

The canonical root document path is also the operational node identity of the
include-graph root.

## Path provenance

Every loaded source document has two distinct path identities:

- its **operational host path**, the host-canonical absolute path used for file
  access, containment, symlink resolution, and diagnostics; and
- its **semantic source-document identity**, the UTF-8 project-relative path
  from the canonical project root to the canonical target, using `/`
  separators and containing no empty, `.` or `..` segments.

The root document's semantic identity is always the logical name
`azldev.toml`. This root-specific rule is an explicit exception to deriving a
source-document identity from the canonical target path: resolving a root
symlink to a differently named target does not change the root semantic
identity. The semantic **defining path base** is the parent of the
source-document identity, using `.` for the project root. These semantic values
use exact Unicode scalar values without normalization or case folding.

Every value read from TOML MUST retain at least its semantic source-document
identity. Every relative path value MUST also retain its semantic defining path
base. A processor MAY retain the operational host path and directory for
diagnostics and file access, but those absolute paths are not part of semantic
model equality.

A relative path value is interpreted relative to the file containing that
value, not relative to the root document, the current working directory, or the
file that later overrides or inherits it.

Composition MUST preserve the semantic provenance of the winning scalar or
array. A recursively merged table or map MUST preserve semantic provenance at
the leaves. Later resolution MAY also retain table-level operational
provenance for diagnostics.

Two byte-identical project trees loaded from different checkout roots have
equal source provenance when their source-document identities and defining path
bases are equal. Converting a path for host operations MUST NOT replace or
discard that relocatable semantic provenance.

## Portable relative model paths

Every ordinary path-valued model field that cites this section uses the same
host-independent grammar. The owning field supplies only its path base and
target-kind requirement, such as regular file, executable regular file, or
directory.

```text
model-path = model-segment ("/" model-segment)*
model-segment = model-character+
```

A `model-character` is one Unicode scalar value other than U+0000, `/`, `\`,
`:`, a C0 control U+0001 through U+001F, or U+007F. `/` is the only separator.
The decoded value MUST be non-empty and cannot contain an empty segment, so a
leading `/`, trailing `/`, or `//` is an error. A backslash is neither a
separator nor a filename character and is always an error.

Before segment processing, a processor MUST explicitly reject POSIX-rooted
forms, strings beginning with `\` including UNC-like forms, an ASCII letter
followed by `:` including drive-relative and drive-qualified forms, and a
URI-like prefix matching `[A-Za-z][A-Za-z0-9+.-]*:`. The general prohibition on
`:` also rejects such spellings elsewhere rather than treating them
host-specifically. These checks are lexical and occur before any host lookup.

A complete segment `.` or `..` is structural navigation, not a host entry
name. Starting from the field's normalized semantic path base, process
segments left to right: `.` leaves the current path unchanged, `..` removes
one existing segment, and another segment appends its exact scalar sequence.
Attempting `..` when no project-relative segment remains is a lexical
containment error, even if later segments would return beneath the project
root. The resulting **normalized lexical form** is `.` for the project root or
the remaining segments joined by `/`; it contains no empty, `.`, or `..`
segment.

For a project-file lookup, every ordinary literal segment MUST be selected by
completely enumerating its containing directory through the
[host filesystem name adapter](#host-filesystem-name-adapter). The processor
MUST convert every entry returned by that complete enumeration before
selection; one unconvertible entry is fatal even when it is unrelated to the
requested segment. Only after the complete enumeration succeeds may the
processor select the one entry whose scalar-value sequence exactly equals the
requested segment. A direct by-name host lookup that avoids observing sibling
entries is not conforming.

After each selected segment, resolve every traversed symbolic link and require
every canonicalized prefix and the final target to remain within the canonical
project root by path-boundary comparison. Structural `.` and `..` perform the
lexical navigation above rather than entry lookup, but the resulting prefix is
still canonicalized and checked before the next ordinary segment. The final
target MUST have the kind required by the owning field. A missing entry, wrong
target kind, symlink loop, decoding failure in any complete containing-directory
enumeration, permission failure, I/O failure, or lexical or canonical escape is
an error.

The **normalized semantic form** of a successful project-file path is the
canonical target's project-relative scalar-value path with `/` separators, or
`.` when the target is the project root. It contains no empty, `.`, or `..`
segment and is compared without Unicode normalization, case folding, locale
collation, or host-equivalent spelling. The original value, normalized
semantic form, defining source-document identity, and defining path base remain
available as provenance. A field interpreted in a later non-project namespace,
such as a materialized-tree path, uses the same lexical normalization but its
own chapter defines target lookup and canonical containment.

The [portable path fixture](./examples/portable-paths/README.md) contains exact
positive normalizations and lexical rejection cases.

### Portable relative-path pattern syntax

A field declared as a relative path pattern extends the model-path grammar
with the `*`, `?`, and `character-class` tokens and same-category ASCII range
rules from [Include-entry and glob grammar](#include-entry-and-glob-grammar).
A complete segment spelled exactly `**` is additionally allowed and matches
zero or more complete directory segments. `**` inside another segment and any
run of three or more `*` are errors. Complete literal `.` and `..` segments
remain structural navigation and are normalized by the model-path rules before
any namespace lookup.

This section defines namespace-neutral lexical syntax and normalization only.
It does not select a filesystem namespace, require that a target currently
exist, enumerate a tree, follow or reject a symbolic link, or define canonical
containment. An owning field that operates in the project tree MUST also import
the traversal contract below. A field in another namespace imports this
lexical contract alone until that namespace's owning chapter defines lookup.
This lexical contract does not impose a filename suffix.

### Project-tree pattern traversal

For an owning field that imports this section, pattern traversal starts from
the field's project-tree path base. It uses the host filesystem name adapter,
never follows a symbolic link as a directory, excludes `.` and `..`
pseudoentries, enforces lexical and canonical project-root containment at every
step, and sorts each successful match set by normalized semantic path UTF-8
bytes. Every directory used for a literal segment or pattern match is
completely enumerated, and an unconvertible entry makes the complete expansion
an error. Failures are typed errors and never partial matches.

The owning field defines the accepted final target kind and whether a
successful zero-match result is allowed. Project-tree traversal does not
impose a filename suffix.

## The `includes` directive

`includes` is an optional array of strings at the exact root-table TOML path
`includes` in a root document or included fragment. At that path it is a
document-control key and is not part of the composed model. The string
`includes` at any nested path has no document-control meaning and is
interpreted by the owning closed-table or name-keyed-map contract.

Each decoded string MUST first pass the host-independent lexical grammar below.
Only then is it classified as a relative literal path or relative glob pattern.
The base directory is the canonical directory of the document that declares
the entry.

Before parsing segments, a processor MUST recognize and reject:

- an empty string;
- a string beginning with `/`, including POSIX-rooted and `//` UNC-like forms;
- a string beginning with `\`, including host-rooted and UNC forms;
- an ASCII letter followed by `:`, whether drive-relative such as
  `C:child.toml` or drive-qualified such as `C:/child.toml`; and
- a URI-like string beginning with an ASCII scheme matching
  `[A-Za-z][A-Za-z0-9+.-]*:`, including `a:b.toml`.

These recognizers are normative and do not depend on whether the host treats
the string as rooted, drive-qualified, or a URI. A backslash is never an
include separator, filename character, or pattern escape; any backslash after
TOML string decoding is an error.

## Include-entry and glob grammar

The grammar operates on Unicode scalar values:

```text
entry           = segment ("/" segment)*
segment         = atom+
atom            = literal | "*" | "?" | character-class
character-class = "[" "!"? class-item+ "]"
class-item      = class-literal | class-range
class-range     = class-literal "-" class-literal
```

`literal` is one Unicode scalar value other than U+0000, `/`, `\`, `:`, `*`,
`?`, `[`, or a C0 control U+0001 through U+001F or U+007F.
`class-literal` is one Unicode scalar value other than U+0000, `/`, `\`, `:`,
`[`, `]`, `-`, a C0 control, or U+007F. A leading `!` immediately after `[` is
negation; `!` elsewhere is a class literal. When a class literal is immediately
followed by `-`, the three scalars MUST parse as a range and the following
class literal is required.

A class range is valid only when both endpoints belong to the same one of
these ASCII categories:

- decimal digits `0` through `9`;
- uppercase letters `A` through `Z`; or
- lowercase letters `a` through `z`.

A valid range includes every ASCII character from its first endpoint through
its second endpoint. The first endpoint MUST NOT be greater than the second.
Mixed-category ranges, non-ASCII ranges, and ranges with an endpoint outside
those categories are errors. Therefore `[2-7]`, `[B-F]`, and `[b-f]` are
valid, while `[Z-a]`, `[0-A]`, and `[Å-Ö]` are errors. Non-ASCII
`class-literal` values remain valid when written individually.

A character class must contain at least one class item after optional
negation. Hyphen has no literal form inside a class, closing bracket cannot be
a class member, and there is no escape syntax. Consequently `[a-]`, `[]a]`,
and `[!]]` are errors. Two consecutive `*` characters outside a class are an
error, so globstar and longer runs are not accepted.

After the complete entry validates, it is a glob when it contains a `*`, `?`,
or character-class token; otherwise it is a literal. `*` matches zero or more
Unicode scalar values within one segment, `?` matches exactly one, and a class
matches exactly one member scalar. Neither `/` nor a segment boundary can be
matched by a metacharacter.

Complete literal segments `.` and `..` are structural navigation components,
not host entry names. This applies whether or not another segment makes the
complete entry a glob. A segment with any other spelling, including `.name`,
`...`, or a glob pattern that can match a leading `.`, is not structural
navigation.

The complete segments are converted to host path operations only after this
lexical validation and classification. Processors MUST NOT pass the original
string to a host glob parser or reinterpret it using host separators, escaping,
case folding, locale collation, or Unicode normalization.

For each literal segment and each directory entry considered by a glob, the
processor MUST enumerate names through the
[host filesystem name adapter](#host-filesystem-name-adapter). On a
case-insensitive or normalization-insensitive filesystem, it MUST choose an
exact scalar-value match rather than accepting a host-equivalent spelling.

Glob expansion MUST be constrained to the project root. A processor MUST NOT
enumerate a directory whose canonical path is outside the project root while
expanding an include.

## Glob-expansion outcomes

Expansion of one validated include entry has exactly one of these typed
outcomes:

- **success(sequence)** contains the complete sequence of matched paths after
  every required directory enumeration and path check has succeeded; or
- **error(reason)** contains no usable match sequence.

An implementation MUST NOT expose or consume partial matches from an expansion
that ends in error.

Segments are processed from left to right. Before any literal-name lookup or
glob matching for a segment, a complete literal `.` segment keeps the current
directory and a complete literal `..` segment selects its parent. The
processor MUST NOT enumerate either spelling as an entry name. After each such
navigation step, it MUST canonicalize the resulting path and require it to be
contained by the canonical project root before processing the next segment.
Navigating above the project root is therefore an error even if a later segment
would navigate back into it.

Every other traversed literal prefix MUST resolve successfully to a directory
whose canonical path is contained by the project root before the next segment
is considered. A missing exact literal prefix is an error, even when a later
segment contains a glob token. A non-final entry selected by a glob segment is
likewise a traversal candidate and MUST resolve successfully to a contained
directory before expansion continues through it.

Every directory required by the expansion MUST be completely enumerated
through the host filesystem name adapter. Any of the following makes the
complete expansion an error:

- a path escape from the project root;
- a symbolic-link loop or other failure to canonicalize a required path;
- traversal through an entry that is not a directory;
- a host entry name that cannot be converted to Unicode scalar values;
- permission denial; or
- any other directory or path I/O failure.

A processor MUST NOT reinterpret one of these failures as an absent entry or an
empty match set. Only after all required enumeration, decoding,
canonicalization, containment, and target checks have completed successfully
may expansion return `success(sequence)`, including `success([])`.

## Match handling and containment

For every literal or matched path, the processor MUST:

1. use the path produced by the left-to-right structural processing of `.` and
   `..` components;
2. resolve every symbolic link needed to identify the target;
3. require the resulting target to be a regular file;
4. require the canonical target path to be within the canonical project root;
5. derive its normalized semantic source-document identity;
6. require that identity to be representable as UTF-8; and
7. use the operational canonical target path as the node identity for cycle and
   repeated-reach detection.

Containment is a path-boundary comparison, not a string-prefix comparison.
For example, `/project-other/file.toml` is not within `/project`.

A missing literal path is an error. A valid glob pattern whose expansion
returns `success([])` contributes no documents and is not an error. No failed
or incomplete expansion is an optional no-match.

Each glob's matches MUST be sorted by the ascending UTF-8 byte sequence of
their normalized project-relative paths. Normalized paths use `/` separators
and do not contain `.` or `..` segments. The `includes` array itself is
processed in declaration order.

## Traversal order

The composition sequence is constructed as follows:

```text
reach(candidate, expected-role):
    canonicalize the candidate and enforce project-root containment
    if its canonical identity is in the active chain: report a cycle
    if its canonical identity was evaluated before:
        record the incoming reach as additional provenance
        reuse the cached source model and return
    otherwise mark it reached and active
    perform byte, expected-role, document-control, and source-model validation
    cache the validated source model
    record its body once at this first depth-first position
    for include entry in declaration order:
        expand the entry
        sort matches by normalized project-relative path
        for match in sorted order:
            reach(match, fragment) depth-first
    remove the document from the active chain
```

After a candidate target has been canonicalized, active-chain and
previously-reached classification MUST occur before reading its bytes,
validating its root/fragment role, or validating its source model. Only a newly
reached canonical node enters those validation phases. In particular, a
self-include of the root is a cycle even though revalidating the same bytes as
a fragment would also find the forbidden root-table `spec-version`.

A previously reached target is classified under
[Cycles and repeated reaches](#cycles-and-repeated-reaches) without
revalidating it as a new source document, traversing its includes again, or
adding its body to the composition sequence again. The processor reuses the
cached validated source model and records the additional incoming reach only
as provenance.

The including document body precedes every body reached from its `includes`
array. Each include subtree is completed depth-first before the next match or
entry is processed. Consequently, later bodies have higher precedence where
the composition rules specify replacement.

The physical position of `includes` within a TOML document does not affect this
order. `includes` is removed from the body before composition.

## Cycles and repeated reaches

A canonical document path that appears again in the active recursion chain
forms a cycle. A cycle is an error, and the diagnostic MUST identify the
ordered canonical or project-relative path chain that closes the cycle.

A **repeated canonical-document reach** is every second or subsequent reach of
one operational canonical document path when that path is not in the active
recursion chain. It is not a cycle. This category includes:

- duplicate literal entries from one parent;
- overlap between a literal and a glob, or between two globs;
- reaches through different parents; and
- distinct lexical or symbolic-link aliases that resolve to one canonical
  document path.

A processor MUST distinguish an active-chain cycle from every repeated reach
and MUST NOT diagnose a repeated reach as cyclic.

Each canonical document MUST be evaluated exactly once, at its first
deterministic depth-first traversal position. That evaluation includes byte
loading, source-document validation, one composition contribution, and one
depth-first traversal of its outgoing includes. Every later non-active-chain
reach MUST reuse that cached source model, MUST NOT contribute the document
body or its outgoing include subtree again, and MUST add the incoming parent,
include entry, matched spelling, and canonical target identity to the
document's reach provenance.

The first reach remains the supplier position for every value contributed by
the document. Later reach provenance records graph connectivity only and do
not change value precedence, array order, diagnostics ownership, or path bases.

## Composition sequence

After loading has produced an ordered sequence of document bodies, the
processor MUST compose them from earliest to latest. The root document's
root-table `spec-version` and every document's root-table `includes` key are
control data and are not merged into the composed model. Identically named
keys at other paths remain ordinary model data under their owning contracts.

Composition uses the following default rules:

| Earlier and later values | Result |
| --- | --- |
| Key absent from the earlier value | Copy the later value and its provenance. |
| Tables or name-keyed maps | Merge recursively by key. |
| Same scalar TOML type | Replace with the later value. |
| Arrays | Replace the complete earlier array with the later array. |
| Different TOML value types | Error. |

An inline table and a standard table are both TOML tables and follow recursive
table merge. The scalar and container types are exactly those in
[TOML value types](./document.md#toml-value-types). Integer and float are
different types, as are all four temporal types. An array is replaced as one
value; its elements are not merged by index.

An explicit later empty array therefore clears an earlier general array:

```toml
items = []
```

`items` is a metasyntactic key in this example, not a specification field.
The example illustrates the value-type rule only.

## Explicit composition exceptions

A field's owning chapter MAY override a default composition rule only by
stating the exception explicitly. The exception MUST define:

- the exact field path;
- the participating value types;
- order and duplicate behavior;
- the meaning of an explicit empty value; and
- provenance of the resulting value.

`ComponentConfig.overlays` is the defined append-composed exception. Its exact
integration and clearing behavior are specified in
[Overlay integration](./overlays.md#overlay-sequences-and-files). For an
append-composed array, earlier elements precede later elements and relative
order within each source array is retained. No other array is append-composed
unless its field contract says so.

An implementation MUST NOT infer append behavior from current tool behavior,
from an array's element type, or from physical key order.

## Composition errors and output

Composition is atomic. A type mismatch, invalid exception, load error, or any
other composition failure produces no composed model.

The successful output is the **composed model** defined in
[Resolution models](./resolution.md#resolution-models). It contains no
root-table `spec-version` or `includes` field, but it retains the selected
specification version as model metadata, retains value provenance, and can
contain identically named keys at non-control paths.

## Focused fixtures

The tracked [loading fixture](./examples/loading/README.md) uses only
document-control keys. It demonstrates recursive path bases, depth-first
traversal, lexical glob ordering, and a missing optional glob. It is a
loading-only metasyntactic fixture rather than a conforming object-model
document.

Additional metasyntactic fixtures cover the
[include lexical grammar](./examples/include-lexical/README.md),
[repeated canonical-document reaches](./examples/repeated-reaches/README.md),
and [relocatable provenance](./examples/provenance/README.md).

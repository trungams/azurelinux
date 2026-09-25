[Return to index](./index.md)

# Overlay transformations

This chapter defines the existing low-level overlay API as deterministic file
transformations. It does not define or imply a future semantic component
transformation model.

## Overlay sequences and files

`ComponentConfig.overlays` is an ordered append-composed sequence. Each source
document contributes its entries in array order and later document reaches
append after earlier reaches within one composed provider. Inheritance appends
the resulting provider sequences in low-to-high layer order. When several
component groups contribute overlays, their arrays append in group-name UTF-8
order while preserving each group's element order; that ordering is sequence
construction, not precedence for any replacing field.

`ComponentConfig.overlay-files` is a replacing array of paths using the
[portable relative model-path grammar](./loading.md#portable-relative-model-paths)
from each entry's defining document. Every target is a regular file containing
UTF-8 TOML with one root `overlays = [ ... ]` value and no other root key. It
is parsed with the same strict vocabulary. Expanded file operations are
appended, in `overlay-files` array order and then file array order, after all
inline effective `overlays`. An empty array clears an earlier list.

After resolving a declared path relative to its defining source document, the
processor canonicalizes it, requires containment by the project root, and
derives a normalized project-relative semantic identity plus a defining path
base equal to the overlay file's parent directory. The overlay file is a
**secondary semantic document**, not a source document: it cannot contain
`spec-version` or `includes`, does not enter the include graph, and has no
include reach, cycle, or repeated-reach semantics.

Every operation loaded from a secondary overlay document retains that
document's semantic identity and defining path base as its supplier
provenance. Its `source` field is resolved relative to that base, never
relative to the component fragment that named the overlay file. A duplicate
canonical overlay-file path in one effective array is an error before either
copy is loaded.

The resulting concatenated array is the **declared operation sequence**.
Implementations MUST NOT inject undocumented operations into this sequence.
An owning optional profile may define a separate transformation only when it
also defines its observable order relative to this pipeline.

## Common overlay field contracts

Every overlay item is a closed table. The tracked
[operation matrix](./overlay-operation-matrix.tsv) is normative and classifies
every field for every operation as:

- `required`: the field MUST be present and valid;
- `optional`: the field MAY be absent, and any conditional restriction in the
  operation contract still applies; or
- `forbidden`: presence is an error even when the TOML value is empty,
  `false`, or an empty array.

The matrix contains every operation enum value exactly once, links to one
unique operation contract below, and names one direct positive fixture. The
forbidden set is the set of common fields whose cell is `forbidden`;
implementations MUST NOT infer ignored fields.

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `type` | string enum | Required for every entry; no default. | One exact operation string from the matrix; path base N/A. | Entry scalar. | Inherited with the ordered entry. | Unknown value is an error. | Core |
| `description` | string | Optional for every operation; no default. | Descriptive UTF-8 without NUL or a C0 control other than tab or newline; path base N/A. | Entry scalar. | Inherited with the entry. | No transformation semantics. | Core |
| `file` | string path or pattern | Required, optional, or forbidden by the matrix. | Materialized-tree namespace using the [portable literal or pattern syntax](./loading.md#portable-relative-path-pattern-syntax); path base is the materialized-tree root. | Entry scalar. | Inherited with the entry. | Literal-only versus pattern use and target kind are operation-specific; project-tree traversal is not imported. | Core |
| `archive` | string filename | Optional only where the matrix allows it; otherwise forbidden. | Plain source-artifact filename: non-empty, not `.` or `..`, and no `/`, `\`, NUL, or C0 control; path base N/A. | Entry scalar. | Inherited with the entry. | Exact case-sensitive top-level materialized regular file; archive-scoped contracts apply when present. | Core |
| `section` | string RPM section token | Required, optional, or forbidden by the matrix. | `%` followed by a lowercase ASCII letter and then lowercase ASCII letters, digits, or `_`; path base N/A. | Entry scalar. | Inherited with the entry. | Matches the recognized section's canonical ASCII-lowercase token, not retained header spelling; operation-specific package relation applies. | Core |
| `package` | string RPM package selector | Required, optional, or forbidden by the matrix. | Non-empty and without NUL, CR, or LF; path base N/A. | Entry scalar. | Inherited with the entry. | Exact parser-associated selector text without case folding, Unicode normalization, or base-name synthesis. | Core |
| `tag` | string RPM tag token | Required or forbidden by the matrix. | `[A-Za-z][A-Za-z0-9]*`; path base N/A. | Entry scalar. | Inherited with the entry. | Matching is ASCII case-insensitive; emitted spelling is exact. | Core |
| `value` | string | Required, optional, or forbidden by the matrix. | Exact UTF-8 without NUL, CR, or LF; path base N/A. | Entry scalar. | Inherited with the entry. | Empty is valid only when the individual operation permits it. | Core |
| `regex` | string RE2 expression | Required or forbidden by the matrix. | Non-empty RE2 syntax; path base N/A. | Entry scalar. | Inherited with the entry. | Backreferences, lookaround, and any unsupported construct are errors. | Core |
| `replacement` | string | Required, optional, or forbidden by the matrix. | Exact UTF-8 literal text without NUL or CR; path base N/A. LF is forbidden for `spec-search-replace` and `file-rename` and permitted only for `file-search-replace`. | Entry scalar. | Inherited with the entry. | `$`, `\`, and digits have no capture-expansion meaning; omitted optional value is empty. A permitted LF is already a normalized logical-line separator, never a CRLF fragment. | Core |
| `lines` | ordered array of strings | Required or forbidden by the matrix. | Non-empty; each element has no NUL, CR, or LF; path base N/A. | Entry array. | Inherited with the entry. | Empty element represents one empty logical line. | Core |
| `source` | string path | Required or forbidden by the matrix. | Project-contained regular file using the portable model-path grammar from the operation-defining document. | Entry scalar retaining provenance. | Inherited with the entry. | Exact bytes, executable bit, semantic path, and supplier provenance are snapshotted before transformation. | Core |
| `metadata` | closed `OverlayMetadata` table | Optional for every operation; no default. | See [Overlay metadata](#overlay-metadata); path base N/A. | Recursive within the entry. | Inherited with the entry. | It never changes matching, order, bytes, or failure behavior. | Core |

## Common transformation pipeline

### Validation, staging, and provenance

Overlay application begins only after source identity and acquisition have
produced one complete candidate dist-git tree and effective artifact set.
Before publishing any result, a processor MUST:

1. validate every operation against the matrix and its individual contract;
2. resolve and snapshot every project-side `source`;
3. compile every `regex` and validate every path or pattern;
4. validate archive/origin association and every statically detectable
   cross-scope conflict;
5. create a private staging copy of the complete candidate tree;
6. apply archive groups in the order below;
7. apply non-archive operations in declared sequence order;
8. verify every operation postcondition, transformed-archive hash, final
   `sources` record, tree invariant, and provenance association; and
9. publish the complete artifact atomically under
   [Materialized artifacts](./artifacts.md#atomic-publication).

An operation reads the staging result of every earlier operation in its phase.
For an operation with several targets, the complete sorted target set is frozen
before the first target is changed. A failure while processing any target
rolls back the whole component attempt, not merely that target or operation.

Every changed or created entry MUST retain provenance identifying:

- the operation index in the declared sequence;
- the operation `type`, defining secondary/source document identity, and
  optional `description`;
- the pre-operation target identity, when one existed;
- every project-side `source` identity and SHA-256 digest used; and
- the post-operation path, type, mode, and SHA-256 digest or symlink target.

Metadata is associated with that provenance but is not transformation input.

### Materialized-tree paths and matching

Path processing is relative to the staging tree root. A literal or pattern
cannot select the active spec through a file operation and cannot select any
path having `.git` as a complete segment. A pattern match is performed without
following a symbolic link as a directory. A symbolic link can be a final
target only for `file-remove` or `file-rename`.

Every required directory enumeration uses the strict filesystem-name adapter,
excludes only `.` and `..` pseudoentries, and fails completely on decoding,
permission, containment, symlink-loop, or I/O error. Matches are sorted by the
ascending UTF-8 bytes of their normalized `/`-separated relative paths.

The operation contracts select one of these target universes:

- **regular-file targets**: regular files only;
- **entry targets**: regular files or symbolic links, never directories; or
- **spec targets**: parsed structures in the one active spec.

A pattern that reaches only directories, excluded `.spec` files, `.git`
entries, or disallowed entry types has zero eligible matches. No-match behavior
is then determined by the individual operation.

### Portable symbolic-link targets

Archive entries and final Materialized-tree entries use one host-independent
symbolic-link target grammar. A target is a non-empty Unicode scalar sequence,
represented as valid UTF-8 when serialized. Its exact accepted scalar sequence
is preserved; processors MUST NOT normalize Unicode, case, separators, or dot
segments in the stored target text.

`/` is the only separator recognized by this grammar. A target is invalid when
it contains NUL or `\`, begins with `/`, or begins with an ASCII drive prefix
`[A-Za-z]:`. These rejections include POSIX absolute paths, slash-spelled UNC
or platform-root paths, backslash-spelled UNC/device/root paths, drive-relative
paths, and drive-absolute paths. Host path libraries and host root conventions
MUST NOT participate in validation.

Containment is evaluated lexically from the link's normalized parent path.
Split the exact target on `/`; an empty segment or `.` leaves the working path
unchanged, `..` removes its final segment, and every other segment appends
exactly. Attempting to remove a segment when the working path is the relevant
archive or Materialized-tree root is an escape and is an error. The final
working path MAY name no existing entry or a later entry, but it MUST remain
within the relevant root. Lookup, canonicalization, and symbolic-link
resolution are forbidden during this check.

### Text, regular expressions, modes, and writes

Spec text and every regular file edited by a text operation MUST be valid
UTF-8. Every such operation performs these steps in this exact order:

1. decode the complete input as UTF-8;
2. validate that every line ending is LF or CRLF and reject every bare CR;
3. replace every CRLF with one LF, producing the normalized text presented to
   parsing, matching, cardinality checks, and editing;
4. perform the operation on that normalized representation; and
5. serialize with LF line endings.

Consequently `file-search-replace` never sees the CR byte from an accepted
CRLF pair. A regular expression matching `\r` has no match on CRLF input.
Line-ending normalization alone does not satisfy a changed-line or
changed-file postcondition: if the requested edit makes no change, the
operation fails and atomic rollback preserves the original bytes.

A transformed spec and every transformed text file end in exactly one LF when
they contain at least one logical line; an empty result has zero bytes.

`spec-search-replace` applies its RE2 expression independently to each logical
line and therefore cannot match across line boundaries. `file-search-replace`
applies its RE2 expression to the complete decoded file and can match LF
characters. Both replace every non-overlapping match with the literal
`replacement`.

Operation-provided `lines` elements and `value` each represent one logical
line and therefore contain no CR or LF. A `spec-search-replace` replacement
also represents text within one logical line and contains neither. A
`file-search-replace` replacement may contain LF separators but no CR. After
every text edit, the complete provisional text is validated again as UTF-8
normalized text with no bare CR; a failure rolls back the complete component
attempt.

An edit preserves the target's executable classification. Materialized regular
files have mode `0755` when executable and `0644` otherwise. A created patch
has mode `0644`. `file-add` uses `0755` when any executable bit was set on its
snapshotted project-side source and `0644` otherwise. Directory mode is `0755`.
Other source mode, ownership, timestamp, ACL, and xattr data is not semantic.

Writes MUST NOT follow a final symbolic link. Replacement of an existing path
is permitted only when an operation explicitly defines replacement; none of
the 17 operations overwrites an unrelated destination implicitly.

### RPM spec representation and selectors

The active spec is the top-level regular file
`<effective-component-name>.spec`. Before the first spec-modifying operation,
the processor normalizes text under the preceding rule and parses the logical
lines using the following deliberately bounded version-1 grammar. A construct
outside this grammar that could be confused with a tag, section, package
selector, conditional directive, or multiline macro definition is an error
before transformation; it is not implementation-defined RPM syntax.

**Line recognition precedence.** Leading and trailing spaces and tabs are
ignored only for recognition; untouched line text is retained. Lines are
classified in this order:

1. a line whose first non-space/tab character is `#`, or a blank line, is an
   ordinary line;
2. the ASCII-case-insensitive conditional tokens below are conditional
   directives;
3. `%define` and `%global`, followed by space/tab and a non-empty remainder,
   are opaque macro-definition directives; a version-1 processor rejects a
   definition whose line ends in an unescaped `\` because multiline macro
   bodies are outside this bounded grammar;
4. an ASCII-case-insensitive token in the section table below is a section
   header when its complete argument sequence satisfies the table;
5. in the global preamble or a `%package` preamble, a tag line has the exact
   lexical form `hspace* tag ":" hspace* value hspace*`, where `tag` is
   `[A-Za-z][A-Za-z0-9]*`; the parsed value begins after the post-colon
   horizontal space and excludes trailing horizontal space; and
6. every other line, including an unrecognized `%word` such as `%autosetup`,
   is ordinary body text.

A line is never both a directive and a section header. In particular `%ifarch`
and every `%elif*` line are directives, not sections. A line beginning with a
recognized section or conditional token but having an invalid argument shape
is an error rather than ordinary text.

**Recognized sections and selectors.** Tokens are recognized
ASCII-case-insensitively. The parser retains the exact header text for
unchanged serialization and separately records the recognized token's
ASCII-lowercase spelling as its **canonical section identity**. Every
operation `section` value matches that canonical identity. Thus `%BUILD` is
retained as `%BUILD` but has identity `%build`.

| Header class | Tokens | Accepted arguments and package association |
| --- | --- | --- |
| Subpackage preamble | `%package` | Exactly one suffix selector, or `-n` followed by exactly one absolute selector. The selector token is the associated package. |
| Package body | `%description`, `%files` | No argument for the main package; otherwise exactly one suffix selector or `-n` plus one absolute selector. |
| Main build body | `%prep`, `%conf`, `%build`, `%install`, `%check`, `%clean`, `%generate_buildrequires`, `%changelog`, `%patchlist`, `%sourcelist` | No arguments; package association is the main package. |
| Package script body | `%pre`, `%post`, `%preun`, `%postun`, `%pretrans`, `%posttrans`, `%preuntrans`, `%postuntrans`, `%verify`, `%triggerin`, `%triggerun`, `%triggerprein`, `%triggerpostun`, `%filetriggerin`, `%filetriggerun`, `%filetriggerpostun`, `%transfiletriggerin`, `%transfiletriggerun`, `%transfiletriggerpostun` | No argument for the main package; otherwise exactly one suffix selector or `-n` plus one absolute selector. Trigger conditions and other flags are outside the bounded grammar and are rejected before transformation. |

A selector is a non-empty token containing no space, tab, NUL, CR, or LF and
not beginning with `-`. Its **package selector identity** is that exact token
after removing only the syntactic `-n` marker. No case folding, Unicode
normalization, macro expansion, or base-package-name prefix is applied. Thus
the ordinary suffix token `devel` and absolute token `pkg-devel` are distinct;
`%package devel` and `%package -n devel` have the same selector identity.

**Conditionals.** Openers are `%if`, `%ifarch`, `%ifnarch`, `%ifos`, and
`%ifnos`. Branch directives are `%elif`, `%elifarch`, `%elifnarch`, `%elifos`,
`%elifnos`, and `%else`; `%endif` closes the innermost opener. Every opener and
`%elif*` requires a non-empty remainder after horizontal space. `%else` and
`%endif` allow no remainder. Directives are ASCII-case-insensitive and may be
nested. Each block has zero or more `%elif*` branches, then at most one
terminal `%else`, then exactly one `%endif`. A branch outside a block, a branch
after `%else`, an unmatched directive, or EOF with an open block is an error.
The processor does not evaluate conditions; every branch remains structural
input.

**Preambles, bodies, and association.** The start of the document through the
first section header is the main-package preamble. Each `%package` header and
its body through the next section header is that exact selector's subpackage
preamble. Every other recognized header and body is a section associated by
the table above. Conditional blocks are recursive structural containers:
blocks wholly inside a preamble or body are content blocks, while a block
containing section headers wraps the section nodes in each branch. Tags are
recognized only in main or subpackage preambles, including content-condition
branches wholly contained by that preamble.

The main-package preamble has one distinguished `main` selector identity.
Every subpackage preamble has its package selector identity above. Complete
model validation, before the first transformation and after every provisional
spec edit, requires exactly one main preamble and at most one `%package`
preamble for each non-main selector identity. A repeated subpackage identity,
including suffix and `-n` spellings that reduce to the same exact token, is a
duplicate-preamble model error. Tag operations and the preamble branch of
`patch-add` therefore have exactly one selected preamble: omitted `package`
selects the one main preamble; a present `package` selects zero or one
subpackage preamble, where zero is an operation selection error and a
pre-existing many case has already failed model validation.

An omitted `package` selects the main-package preamble or a main-package
section. A present value selects the exact package selector identity. For a
section-targeted operation, `(canonical section identity, package selector
identity)` selects every parsed section with that exact identity in document
order. Unless an operation says otherwise, zero matches is an error and one or
more matches are all transformed. Whole-file operations require both fields
to be absent.

**Structural removal.** A selected section range includes its header, body,
and conditional blocks wholly contained by that body. Such a block is removed
with the section. A conditional wrapper whose opener, branch directive, or
`%endif` lies outside the selected range is never partly removed. Removing a
section nested in one wrapper branch is permitted only when the surviving text
in that branch reaches another surviving section boundary before any
non-comment, non-blank ordinary line, tag, macro definition, or directive
would inherit the removed section. Otherwise the removal is a
conditional-branch-span error. These checks apply independently to every
branch and every selected section; the post-edit conditional tree MUST remain
balanced.

The parsed spec is sequential state, not a parse-once view. Immediately after
each successful spec or hybrid patch operation, the processor:

1. serializes the complete provisional logical-line sequence using LF
   separators and exactly one final LF for a non-empty sequence, or zero bytes
   for an empty sequence;
2. reparses those bytes from the beginning under this complete bounded grammar
   and reruns conditional, recognized-construct, selector, duplicate-preamble,
   and association validation; and
3. makes that reparsed structure and its normalized logical lines the sole
   input state for the next spec or patch operation.

An invalid intermediate or final state fails the complete component attempt,
including any file half of a hybrid patch operation. The last operation's
reparse is the required final grammar validation; there is no path that
publishes a merely text-edited unparsed spec. Untouched logical-line scalar
sequences remain exact, line terminators follow the common LF rule, and the
final mode is `0644`.

### Ordering and conflicts

Archive-scoped `file-remove` and `file-search-replace` operations are removed
temporarily from the declared sequence and grouped by exact `archive` name.
Groups are ordered by the first occurrence of each archive. Operations within
one group retain their declared relative order and share one
extract/modify/repack cycle. Every archive group completes before the first
non-archive operation. Non-archive operations then execute in original
declared order with archive-scoped entries skipped.

Sequential operations may intentionally edit the same spec structure or loose
file. A later spec or patch operation sees only the reparsed state produced by
the preceding operation; a later loose-file operation sees the preceding
serialized bytes. The following conflicts are errors before any output is
published:

- a non-archive operation can match, remove, replace, create, or rename to an
  archive file that is also the target of an archive group;
- two archive filenames resolve to the same tree entry;
- an operation would target the active spec through the file namespace;
- a create or rename destination already exists when that operation starts;
- a hybrid patch operation cannot establish agreement between the spec and
  loose-file namespaces; or
- any path would escape its namespace, traverse through a symbolic-link
  directory, or overwrite a path through a symlink alias.

For conflict detection, a loose-file pattern is tested against each grouped
top-level archive name using the normative path-pattern matcher even if an
earlier operation would later remove that archive. A `file-add` or `patch-add`
destination and a `file-rename` destination are tested directly.

### Archive extraction and batching

An archive-scoped operation supports tar streams whose filename has one of
these case-insensitive suffixes:

- `.tar`;
- `.tar.gz` or `.tgz`;
- `.tar.xz` or `.txz`; or
- `.tar.zst` or `.tzst`.

The actual compression is detected from content and is authoritative over the
suffix. The signatures are gzip `1f 8b`, xz `fd 37 7a 58 5a 00`, and zstd
`28 b5 2f fd`; input without one of those prefixes is interpreted as
uncompressed tar. A recognized suffix with invalid tar data or unsupported
compression is an error. Repacking preserves the content-detected compression
and the top-level archive filename.

Compressed input has exactly one member, stream, or frame and ends immediately
after it. Gzip is one RFC 1952 member with compression method 8, reserved flag
bits clear, and valid header, DEFLATE end, CRC32, and ISIZE. Xz is one `.xz`
stream with valid block and stream integrity checks. Zstd is one standard
RFC 8878 frame, not a skippable frame, with no external dictionary requirement
and valid block structure, declared content size when present, and checksum
when present. Concatenated members/streams/frames, skippable frames, integrity
errors, premature compressed EOF, and any compressed trailing byte are errors.
The decompressed byte sequence is then subject to the exact tar EOF rule below.
These are input-decoder rules only and do not select compressor or tar output
bytes under OD-7.

The accepted base-header dialect is POSIX.1-1988 ustar: each non-zero header is
512 bytes, bytes 257..262 are `ustar` followed by NUL, and bytes 263..264 are
`00`. POSIX PAX `g`/`x` and GNU `L`/`K` extension records use the same base
header dialect. V7, old-GNU base headers, sparse dialects, and every other
header dialect are errors.

Every header checksum is verified before any field is used. For checksum
calculation, the eight checksum-field bytes are treated as ASCII spaces and
all 512 unsigned byte values are summed; the stored value MUST equal that sum.
Every numeric field uses only this unsigned octal form: zero or more leading
ASCII spaces, one or more ASCII digits `0`..`7`, then only NUL or ASCII space
through the end of the field. A digit after a terminator, an empty numeric
value, overflow, a non-octal byte, or a high-bit/base-256 representation is an
error. This rule applies to mode, UID, GID, size, mtime, checksum, device major,
and device minor; non-semantic values are still syntactically validated.

The fixed `name` (100 bytes), `linkname` (100 bytes), and `prefix` (155 bytes)
fields end at their first NUL, after which every remaining byte in that field
MUST be NUL. The base path bytes are `prefix + "/" + name` when prefix is
non-empty and `name` otherwise. A non-empty prefix with an empty name is an
error. Link bytes come from `linkname`. These fixed fields remain byte strings
until effective-field precedence is resolved: only the final effective path
and, for a symlink, final effective link are decoded as strict UTF-8. Therefore
a completely overridden lower-precedence base placeholder need not itself be
UTF-8, while the selected PAX or GNU value is validated when its extension
record is decoded.

The only accepted type flags are NUL or `0` for a regular file, `5` for a
directory, `2` for a symbolic link, and `g`, `x`, `L`, or `K` for the metadata
records below. A directory or symbolic-link base header has size zero.
Hardlink `1`, device `3`/`4`, FIFO `6`, contiguous `7`, sparse/vendor flags,
and every unknown flag are errors.

Each accepted header is followed by exactly its selected payload size and then
the minimum number of zero bytes needed to reach the next 512-byte boundary.
Non-zero padding, truncated payload or padding, and payload bytes beyond the
selected size are errors. The archive ends with exactly two consecutive
all-zero 512-byte blocks. A single zero block, EOF before both blocks, an
additional zero block, or any other decompressed trailing byte is an error.

Before materialization, the complete tar namespace is interpreted and
validated:

- entry names are strict UTF-8, use `/`, and are non-empty relative paths with
  no NUL, backslash, empty, `.`, or `..` segment;
- after removing the conventional trailing `/` spelling of a directory, two
  entries cannot have the same normalized path;
- no regular file or symlink can be an ancestor of another entry;
- the only semantic entry types are directory, regular file, and symbolic
  link;
- hardlinks, devices, FIFOs, sockets, sparse entries that cannot be represented
  as ordinary regular bytes, and any other entry type are errors;
- every symlink target satisfies
  [Portable symbolic-link targets](#portable-symbolic-link-targets) relative
  to its parent and the archive root; and
- no extraction or overlay lookup follows a symbolic link as a directory.

Tar extended metadata has one required closed interpretation. The supported
metadata record forms are POSIX PAX global headers (`g`), POSIX PAX per-entry
headers (`x`), GNU long-name headers (`L`), and GNU long-link headers (`K`).
These records do not create semantic entries. Every other metadata pseudo-type,
GNU sparse form, vendor type effect, or unknown tar type is unsupported and is
an error.

A PAX payload is a sequence of exact
`decimal-length SP key "=" value LF` records whose declared byte length covers
the complete record. `decimal-length` is one or more ASCII decimal digits with
no sign or leading zero and counts every byte from its first digit through the
terminal LF. The declared boundary MUST land on that LF, records MUST be
contiguous, and the final record MUST end exactly at the PAX payload boundary.
An invalid length, missing separator, missing assignment `=`, boundary
overrun/underrun, missing terminal LF, or trailing unframed byte is an error.

Every complete key and value byte sequence is decoded as strict UTF-8 and MUST
be a Unicode scalar sequence. Invalid UTF-8 in either is an error before key
semantics are considered. Supported keys are the exact ASCII spellings below.
The `path` and `linkpath` values are non-empty and receive final namespace
validation when effective; `size` is non-empty ASCII decimal in the unsigned
64-bit range. The only entry-affecting keys are `path`, `linkpath`, and
`size`.

The recognized non-semantic keys are `mtime`, `atime`, `ctime`, `uid`, `gid`,
`uname`, `gname`, `devmajor`, `devminor`, `comment`, `charset`, and
`hdrcharset`. Each non-semantic value uses one closed rule: any non-empty valid
UTF-8 Unicode scalar sequence is accepted, retained only for metadata
precedence, duplicate detection, and global deletion bookkeeping, and ignored
when constructing the semantic archive result. No numeric, timestamp, user,
device, or charset interpretation is applied. In particular, `charset` and
`hdrcharset` never select another decoder: PAX records, GNU values, and final
effective path/link values still require UTF-8 exactly as specified here.

Any other key is unsupported. A key may occur at most once in one PAX header.
Two pending per-entry PAX headers that assign the same key, two pending GNU
`L` records, or two pending GNU `K` records are duplicate metadata and are
errors even when their values agree.

A global PAX header updates persistent key values for later base headers; an
empty global value deletes that key, including a recognized non-semantic key.
An empty per-entry value is rejected for every recognized key, including a
non-semantic key; it is not an opaque empty value. A per-entry PAX header and
GNU `L`/`K` record apply to exactly the next non-metadata base header and are
then cleared. Pending per-entry metadata at EOF is an error. For one base
header, the final effective fields have this precedence:

| Effective field | Highest to lowest precedence |
| --- | --- |
| path | per-entry PAX `path`; global PAX `path`; GNU `L`; base header name |
| link target | per-entry PAX `linkpath`; global PAX `linkpath`; GNU `K`; base header link name |
| size | per-entry PAX `size`; global PAX `size`; base header size |
| type | base header type flag only |

Different-precedence values intentionally override rather than conflict.
Metadata cannot change entry type. An effective `linkpath` is valid only for a
symbolic-link base header; an effective `size` is valid only for a regular-file
base header. The effective size selects exactly that many payload bytes.
Duplicate same-precedence assignments, an incompatible field/type pairing,
truncated payload, or trailing payload beyond the effective size is an error.

A GNU `L` or `K` payload has a positive header size, consists of non-NUL bytes
followed by exactly one terminal NUL, contains no earlier NUL, and has no
newline terminator. Remove that one NUL and decode the remaining bytes as
strict UTF-8 immediately. `L` supplies the pending path and `K` the pending
link target. A missing terminator, extra bytes after the terminator, empty
decoded value, or invalid UTF-8 is an error.

Only the final effective path, link target, size, and type are subjected to the
namespace and entry validation above. An invalid lower-precedence placeholder
that is completely overridden is not separately rejected. The final effective
entry is nevertheless rejected if its metadata-derived path or link target
escapes.

After effective entries are validated, synthesize every missing non-empty
directory ancestor required by an entry, from shallowest to deepest, before
wrapper detection or semantic-tree comparison. A synthesized directory has no
independent tar header and survives only while at least one descendant
requires it. An explicit directory header is marked explicit; it remains in
the semantic archive result even when empty after overlay removal. When an
explicit header names a directory already synthesized for an earlier
descendant, the one directory becomes explicit. The archive root itself is
never an entry.

If the validated archive tree has exactly one immediate child and that child
is a directory, inner `file` patterns are relative to that directory.
Otherwise they are relative to the archive root. The selected extraction root
does not remove the wrapper directory from the repacked semantic tree.

Archive operations use the same matching and text rules as their loose-file
forms. Explicit empty directories remain after file removal; synthesized
directories without a surviving descendant disappear. Unchanged regular
bytes, entry types, executable classifications, directory presence, and
symlink targets are preserved. The deterministic **semantic archive result**
is the sorted set of normalized entry paths with:

- type;
- regular-file bytes and executable classification;
- directory presence and whether that presence is explicit or synthesized; or
- symlink target.

Within that semantic result, directories have mode `0755`; a regular file is
executable exactly when `(base-header-mode & 0111) != 0` after octal decoding,
and therefore has semantic mode `0755` when executable and `0644` otherwise;
symlink mode is not semantic. PAX and GNU metadata cannot change this
classification.

UID, GID, owner/group names, timestamps, archive header format, compression
parameters, padding, and extended metadata do not participate in that semantic
result.

Exact canonical transformed-archive encoding remains open in
[OD-7](./open-decisions.md#od-7-canonical-transformed-archive-bytes).
Consequently no implementation may claim portable transformed-archive byte
conformance or Materialized-tree conformance in this revision. This does not
permit divergent successful output: the configured post-overlay hash below is
a required byte-level acceptance gate.

### Post-overlay hash and `sources` association

Every grouped archive:

1. MUST name exactly one upstream `sources` record and acquired regular file;
2. MUST have exactly one effective `source-files` entry with the same
   `filename`, `origin.type = "overlay"`, `replace-upstream = true`, a
   non-empty `replace-reason`, and `hash-type` of `SHA256` or `SHA512`;
3. MUST be referenced by at least one archive-scoped operation; and
4. MUST have no loose-file conflict.

An overlay-origin entry without a grouped archive is an error. A grouped
archive without the entry is an error. Bootstrap modes that omit the entry or
hash are outside conformance.

After the semantic archive result is repacked, the processor computes the
configured digest over the exact archive bytes. A mismatch with the configured
`hash` is an error and the original artifact remains unpublished. An
implementation that cannot emit bytes matching the configured hash MUST report
an unsupported transformed-archive encoding or hash mismatch; it MUST NOT
publish different bytes successfully.

The matching upstream `sources` record is replaced in its original line
position with:

```text
HASHTYPE (filename) = lowercase-digest
```

`HASHTYPE` is the configured `SHA256` or `SHA512`. The original line terminator
(LF, CRLF, or no terminator on the final line) is preserved. Every comment,
blank line, and other unmodified record byte is preserved. The materialized
archive bytes, rewritten record, configured overlay-origin entry, declared
archive operations, original upstream record, and original acquired digest are
one provenance association.

## Spec-tag operations

### Operation: `spec-add-tag`

**Fields and representation.** Parsed spec. `type`, `tag`, and `value` are
required. `package`, `description`, and `metadata` are optional; every other
common field is forbidden.

**Target and match.** The selected package preamble MUST exist. Tag matching in
that preamble is ASCII case-insensitive.

**Effect and postconditions.** When no matching tag exists, append one
`<tag>: <value>` logical line immediately before the next section header,
after existing preamble lines. The emitted tag and value use the supplied
spellings.

**Failures and provisional cardinality.** A missing package or one or more
matching tags is an error. The zero-match precondition is provisional
containment for the documentation/runtime disagreement recorded in
[OD-6](./open-decisions.md#od-6-overlay-match-cardinality). It prevents two
implementations from successfully producing singleton versus duplicate-tag
results.

### Operation: `spec-insert-tag`

**Fields and representation.** Parsed spec. The allowed and required fields
are identical to `spec-add-tag`.

**Target and match.** The selected package preamble MUST exist. A tag's family
is its ASCII-lowercased name after removing every trailing ASCII digit. Scan
the selected preamble in file order.

**Effect and postconditions.** Insert `<tag>: <value>` after the last tag in
the same family; if none exists, after the last tag of any family; if no tag
exists, at the end of the preamble. When the selected insertion tag is inside
a balanced conditional block wholly contained by the preamble, identify every
such enclosing block and insert after the `%endif` of the outermost one. This
makes the new tag unconditional with respect to all nested enclosing blocks.
Existing tags with the exact requested name do not prevent insertion.

**Failures.** A missing package, an unbalanced conditional, or a selected
conditional whose closing directive lies outside the preamble is an error.

### Operation: `spec-set-tag`

**Fields and representation.** Parsed spec. The allowed and required fields
are identical to `spec-add-tag`.

**Target and match.** Count ASCII-case-insensitive matching tag names in the
selected package preamble.

**Effect and postconditions.** Zero matches adds one tag using the
`spec-add-tag` insertion position. Exactly one match replaces that complete
tag line with `<tag>: <value>`.

**Failures and provisional cardinality.** A missing package or more than one
match is an error. The multiple-match error is provisional containment for the
first-versus-all implementation disagreement in
[OD-6](./open-decisions.md#od-6-overlay-match-cardinality).

### Operation: `spec-update-tag`

**Fields and representation.** Parsed spec. The allowed and required fields
are identical to `spec-add-tag`.

**Target and match.** Count ASCII-case-insensitive matching tag names in the
selected package preamble.

**Effect and postconditions.** Exactly one match is replaced by
`<tag>: <value>`.

**Failures and provisional cardinality.** A missing package, zero matches, or
more than one match is an error. The multiple-match error is the same
provisional containment as `spec-set-tag`.

### Operation: `spec-remove-tag`

**Fields and representation.** Parsed spec. `type` and `tag` are required.
`value`, `package`, `description`, and `metadata` are optional; every other
common field is forbidden.

**Target and match.** Select every ASCII-case-insensitive matching tag name in
the selected package preamble. When `value` is present, retain only tags whose
parsed value is exactly equal as a Unicode scalar sequence. Omitted `value`
does not filter by value.

**Effect and postconditions.** Remove every selected tag line while preserving
the order and text of every other line.

**Failures.** A missing package or zero selected tag lines is an error. The
exact value comparison is the normative v1 rule; characterized
case-insensitive value matching is not copied.

## Spec line and text operations

### Operation: `spec-prepend-lines`

**Fields and representation.** Parsed spec. `type` and non-empty `lines` are
required. `section`, `package`, `description`, and `metadata` are optional;
every other common field is forbidden. `package` requires `section`.

**Target and match.** With no `section`, target the complete spec and require
`package` to be absent. Otherwise select every exact `(section, package)`
section.

**Effect and postconditions.** Whole-file targeting inserts `lines` before the
first existing logical line. Section targeting inserts `lines` immediately
after each selected section header, before its first body line, in the supplied
order.

**Failures.** A package without a section or zero selected named sections is an
error.

### Operation: `spec-append-lines`

**Fields and representation.** Parsed spec. Its field set and target selection
are identical to `spec-prepend-lines`.

**Effect and postconditions.** Whole-file targeting inserts `lines` after the
last existing logical line. Section targeting inserts `lines` at the end of
each selected section body immediately before the next section header or EOF.

**Failures.** A package without a section or zero selected named sections is an
error.

### Operation: `spec-search-replace`

**Fields and representation.** Parsed spec text. `type` and `regex` are
required. `replacement`, `section`, `package`, `description`, and `metadata`
are optional; every other common field is forbidden. An omitted replacement is
empty. `package` requires `section`.

**Target and match.** With no `section`, examine every parsed non-header
logical line across the spec. Otherwise examine every non-header body line
belonging to every selected exact section. Section header directives are never
search-replace targets. Matching is line-by-line and cannot span line endings.

**Effect and postconditions.** Replace every non-overlapping match in every
examined line with the literal replacement. The operation succeeds only when
at least one examined line changes.

**Failures.** Invalid RE2, package without section, zero selected named
sections, or no changed line is an error. A zero-width match that reproduces
the same line does not satisfy the postcondition.

## Spec structural operations

### Operation: `spec-remove-section`

**Fields and representation.** Parsed structural spec. `type` and `section`
are required. `package`, `description`, and `metadata` are optional; every
other common field is forbidden.

**Target and match.** `section` cannot designate the global preamble. Select
every exact `(section, package)` match in file order.

**Effect and postconditions.** Remove each selected header and body range.
Conditional pairs wholly within a removed range are removed. A wrapping
conditional directive outside the section is retained only when doing so
leaves balanced structure and no `%else` or `%elif*` branch semantics are
changed.

**Failures.** Zero matches, an attempt to remove the global preamble, unmatched
conditionals, a conditional spanning section content, or a branch directive
whose enclosing conditional extends outside the removed range is an error.

### Operation: `spec-remove-subpackage`

**Fields and representation.** Parsed structural spec. `type` and `package`
are required. `description` and `metadata` are optional; every other common
field, including `section`, is forbidden.

**Target and match.** Select every `%package` preamble and every other section
whose canonical parser-associated package selector identity exactly equals
the operation's `package` token. The shared selector rule applies without an
operation-local exception: suffix and syntactic `-n` forms that yield the same
exact selector token are the same identity. No component/base-name prefix
synthesis, macro expansion, case folding, Unicode normalization, trimming, or
other token rewriting occurs.

**Effect and postconditions.** Remove every selected section in file order,
using the same conditional-balance rules as `spec-remove-section`. Main-package
sections remain.

**Failures.** Zero associated sections or any conditional-balance failure is
an error.

## Patch operations

### Operation: `patch-add`

**Fields and representation.** Hybrid parsed spec plus one created regular
file. `type` and `source` are required. `file`, `package`, `description`, and
`metadata` are optional; every other common field is forbidden.

**Target and match.** The destination is the optional `file` or, when omitted,
the final segment of the normalized `source` path. It MUST be a plain basename
ending in `.patch`. Its top-level staging destination and every spec patch
reference with that exact value MUST be absent. A present `package` MUST name
an existing subpackage preamble.

**Effect and postconditions.**

- If exactly one `%patchlist` section exists and `package` is absent, append the
  destination basename as its final non-header logical line.
- Otherwise no `%patchlist` may exist. In the selected package preamble, a
  patch tag name is ASCII-case-insensitive `Patch` followed by either no
  suffix or one or more ASCII decimal digits. Any other non-empty suffix is an
  error. Explicit decimal suffixes are parsed without leading-zero
  significance and MUST be in `0..2147483647`; two explicit spellings of the
  same integer are a duplicate-slot error.
- Reserve every explicit slot first. Then, in file order, assign each bare
  `Patch:` tag the least non-negative slot not already reserved or assigned.
  This assignment is semantic only and does not rewrite existing bare tags.
  The occupied set is the union of explicit and assigned slots. For an empty
  set, `highest = -1`; otherwise it is the greatest occupied slot.
- The new slot is `highest + 1`. If `highest` is `2147483647`, allocation
  overflows and is an error. Insert
  `Patch<new-slot>: <destination>` after the last existing patch-family tag;
  if there is none, after the last tag of any family; if there is no tag, at
  the end of the selected preamble. If that insertion tag is inside nested
  conditional blocks wholly contained by the preamble, insert after the
  outermost enclosing `%endif`, using the same unconditional-placement rule
  as `spec-insert-tag`.
- Copy the snapshotted source bytes to the top-level destination with mode
  `0644`.

The spec reference and file creation are one atomic operation.

**Failures.** Multiple `%patchlist` sections, `package` combined with a
`%patchlist`, an invalid suffix, duplicate or out-of-range explicit slot,
allocation overflow, a conditional placement block crossing the selected
preamble, missing package, invalid/non-patch basename, existing reference,
existing destination, or copy/write failure is an error.

### Operation: `patch-remove`

**Fields and representation.** Hybrid parsed spec plus loose regular files.
`type` and patterned `file` are required. `description` and `metadata` are
optional; every other common field is forbidden.

**Target and match.** Match the pattern against:

- parsed values of every bare `Patch:` or decimal `PatchN:` tag;
- every non-empty trimmed line in every `%patchlist` section; and
- eligible loose regular-file paths.

Each reference value MUST itself be a normalized portable relative path with
no macro expression. The set of unique matching reference paths MUST be
non-empty and exactly equal the set of matching loose-file paths.

**Effect and postconditions.** Remove every matching tag or patchlist line and
every matching file. Duplicate references to one path are all removed. RPM
`%patch`, `%patchN`, or `%autosetup` invocation lines are not rewritten.

**Failures.** Invalid reference syntax, zero references, zero files, unequal
reference/file sets, a non-regular matched entry, or any removal failure is an
error. The spec and file removals are atomic together.

## Loose-file and archive-file operations

### Operation: `file-prepend-lines`

**Fields and representation.** UTF-8 loose regular files. `type`, patterned
`file`, and non-empty `lines` are required. `description` and `metadata` are
optional; every other common field is forbidden.

**Target and match.** Select one or more eligible loose regular files in
sorted path order.

**Effect and postconditions.** Prefix each file with `lines` joined by LF plus
one LF, followed by the normalized decoded text. Preserve executable
classification.

**Failures.** Zero eligible files, invalid UTF-8, or any read/write failure is
an error.

### Operation: `file-search-replace`

**Fields and representation.** UTF-8 regular files in loose or archive scope.
`type`, patterned `file`, and `regex` are required. `archive`,
`replacement`, `description`, and `metadata` are optional; every other common
field is forbidden. Omitted replacement is empty.

**Target and match.** Without `archive`, select eligible loose regular files.
With `archive`, select eligible regular files below the chosen extraction
root. Zero eligible files is an error.

**Effect and postconditions.** Apply the RE2 expression to each complete file
after CRLF normalization and replace every non-overlapping match literally.
Preserve executable classification. Files with no requested edit remain
untouched, but at least one target file MUST change as a result of the
replacement rather than line-ending normalization alone.

**Failures.** Invalid RE2, invalid UTF-8, zero eligible files, no changed file,
or any read/write/archive failure is an error.

### Operation: `file-add`

**Fields and representation.** One created loose regular file. `type`, literal
`file`, and `source` are required. `description` and `metadata` are optional;
every other common field is forbidden.

**Target and match.** `file` is a literal normalized materialized-tree path,
not a pattern. Its parent directory MUST already exist and MUST contain no
symlink traversal. The destination MUST be absent and cannot end in `.spec`.

**Effect and postconditions.** Copy the snapshotted source bytes to the
destination. Set mode `0755` when the source snapshot is executable and `0644`
otherwise.

**Failures.** Missing parent, existing destination, forbidden spec path,
symlink traversal, or copy/write failure is an error.

### Operation: `file-remove`

**Fields and representation.** Loose or archive entry targets. `type` and
patterned `file` are required. `archive`, `description`, and `metadata` are
optional; every other common field is forbidden.

**Target and match.** Without `archive`, select loose regular files and
symbolic links. With `archive`, select regular files and symbolic links below
the extraction root. Directories are never eligible. A symlink match removes
the link itself and never its target.

**Effect and postconditions.** Remove every selected entry in sorted path
order. For loose-tree staging, now-empty parent directories MAY remain as
private traversal nodes for later operations, but final Materialized-tree
directories are re-derived from surviving descendants and therefore disappear
when empty. In archive scope, an explicit empty directory header remains,
while each synthesized directory with no surviving descendant disappears
before the next archive operation and from the semantic archive result.

**Failures.** Zero eligible entries or any removal/archive failure is an
error.

### Operation: `file-rename`

**Fields and representation.** One loose regular file or symbolic link.
`type`, patterned `file`, and `replacement` are required. `description` and
`metadata` are optional; every other common field is forbidden.

**Target and match.** The pattern MUST select exactly one eligible entry.
`replacement` MUST be a plain basename: non-empty, not `.` or `..`, and
containing no `/`, `\`, NUL, CR, or LF. The destination is that basename in
the selected entry's existing parent directory and MUST be absent. A
replacement ending in `.spec` is forbidden.

**Effect and postconditions.** Rename the entry without changing regular-file
bytes, executable classification, or a symlink's exact target text.

**Failures.** Zero or multiple matches, invalid replacement, existing
destination, forbidden spec destination, or rename failure is an error. There
is no archive-scoped form.

## Overlay metadata

`OverlayMetadata` is reusable both as `overlays[].metadata` and
`component-groups.<group>.metadata`.

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `category` | string enum | Required when metadata is present. | Exactly `upstream-backport`, `azl-pruning`, `azl-compatibility`, `azl-temp-workaround`, `azl-branding-policy`, `azl-disable-flaky-tests`, `azl-disable-unsupported-tests`, `azl-security-compliance`, `azl-release-management`, or `azl-platform-adaptation`; path base N/A. | Scalar replace. | Inherited with owning object. | Unknown value is an error. | Core |
| `commits` | Array of closed `{ url = string }` tables | Optional; default `[]`. | URLs must be absolute `https`; path base N/A. | Array replace. | Inherited with owning object. | Duplicate URLs are errors. | Core |
| `bugs` | Array of closed `{ url = string }` tables | Optional; default `[]`. | URLs must be absolute `https`; path base N/A. | Array replace. | Inherited with owning object. | Duplicate URLs are errors. | Core |
| `upstream-status` | string enum | Required when metadata is present. | Exactly `upstreamed`, `upstreamable`, `needs-upstream-hook`, `inapplicable`, or `unknown`; path base N/A. | Scalar replace. | Inherited with owning object. | Unknown value is an error. | Core |

Metadata never changes operation order or transformation semantics.

## Evidence, provisional rules, and claim status

The operation names and low-level effects are characterized from current
azldev code, tests, documentation, and Azure Linux usage. Normative v1 rules
deliberately differ where current behavior is non-atomic, accepts irrelevant
fields, follows host-dependent paths, or has contradictory cardinality.

The [overlay fixture directory](./examples/overlay-operations/README.md)
contains representative positive and negative semantic assertions for every
operation family, every enum value, archive safety and conflict cases,
post-overlay hash association, and atomic publication. These assertions are not
an exhaustive conformance suite.

All conformance classes remain forward-declared and non-claimable.
Transformed-archive bytes additionally remain gated by OD-7. Implementations
MUST NOT describe current-tool compatibility, a passing representative
fixture, or a configured post-overlay hash as a portable Materialized-tree
claim.

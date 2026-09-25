# Overlay operation fixtures

`cases.toml` contains representative semantic assertions for all 17 low-level
overlay operations and positive/negative coverage for every operation family.
It also executes the bounded RPM line grammar, sequential reparse state,
uppercase canonical section identity, duplicate-preamble rejection, nested
conditional placement, directive-versus-section recognition, `patch-add` slot
allocation, mixed suffix/`-n` package-selector identity, generated
required/forbidden field failures, CRLF-before-match behavior, and post-edit
line-ending/grammar validation.

`archive-cases.toml` covers first-occurrence grouping, sequential intra-archive
order, synthesized-ancestor removal, explicit empty directories, portable
symlink targets, closed PAX/GNU metadata precedence, actual POSIX ustar header
parsing, raw PAX `g`/`x` record framing and UTF-8 decoding, opaque
non-semantic values, empty-global deletion, checksums, GNU payload
terminators, executable classification, tar EOF, gzip/xz/zstd framing,
traversal, hardlink, duplicate-entry, loose/archive conflict, bidirectional
overlay-origin association, post-overlay hash verification, and all-or-nothing
publication.

These fixtures use logical spec lines and exact hexadecimal file bytes where a
small local oracle is useful. Raw archive vectors are tracked as base64 of a
zlib fixture-transport encoding; the helper first reconstructs the exact input
bytes, then performs the tested archive decompression and header parsing.
Every counted negative invokes a predicate and checks its machine-readable
failure category. They are representative semantic assertions, not an
exhaustive RPM, filesystem, or conformance suite.

# Include lexical cases

`cases.toml` is metasyntactic test data for the host-independent include-entry
grammar. Each `valid` case must pass with the stated literal/glob
classification. The strings under `invalid` must fail before classification or
host path conversion. Each `lookup` case compares decoded Unicode scalar
sequences exactly.

The cases cover `/` separators, backslash rejection, rooted, drive-relative,
drive-qualified, UNC-like, URI-like, globstar, and restricted character-class
forms. Ranges are limited to one ASCII category: digits, uppercase letters, or
lowercase letters. Non-ASCII characters remain available as individual class
literals. The cases do not define object fields.

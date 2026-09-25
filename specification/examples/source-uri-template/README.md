# Source URI-template fixture

`cases.toml` supplies exact dist-git and lookaside expansion results for the
contract in [Sources](../../sources.md#source-uri-templates). It covers
case-sensitive values, simultaneous replacement, UTF-8 and reserved-byte
encoding, rejection of `/` and `\` in substitution values, canonical existing
escapes, literal dollar tokens, cardinality, and post-expansion validation.

[Return to index](./index.md)

# Excluded, deferred, and compatibility fields

This chapter is normative about disposition: the listed characterized keys are
not accepted by conforming `0.1` source documents unless another chapter
explicitly defines a replacement.

| Characterized field | Disposition | Portable replacement or reason | Error behavior |
| --- | --- | --- | --- |
| `$schema` | Tool-specific deferred | Editor schema selection is outside source-document semantics. | Unknown root key. |
| `tools.imageCustomizer.containerTag` and containing camel-case tables | Tool-specific deferred | Tool container selection is operational and camel-case conflicts with canonical spelling policy. | `tools` is an unknown root key in conformance mode. |
| `project.log-dir`, `work-dir`, `output-dir`, `rendered-specs-dir`, `lock-dir` | Tool-specific deferred | Work/cache/output layouts are not portable model inputs; no lock file is normative. | Unknown project keys. |
| `ComponentConfig.render.skip-file-filter` | Tool-specific deferred | [Materialized artifacts](./artifacts.md) defines output bytes directly. | Unknown component key. |
| `ComponentConfig.build.failure.expected`, `expected-reason` | Tool-specific deferred | CI expectation policy. | Unknown build key. |
| `ComponentConfig.build.hints.expensive` | Tool-specific deferred | Scheduler hint. | Unknown build key. |
| `build.check.skip_reason` | Intentionally excluded spelling | Canonical replacement is `skip-reason`. | Unknown key. |
| `PackageConfig.publish.channel` | Deprecated but accepted in publishing profile | Fallback for `rpm-channel` only when the latter is absent. | Both effective values present is an error. |
| `tests.<name>.lisa.test-cases` | Replaced LISA spelling | Rename to `testcase-names`; values and order are retained. | Unknown key in a `0.1` source document. |
| `tests.<name>.lisa.test-suites` | Removed draft-only spelling | Express selection with `criteria`, `name`, `testcase-name`, or `testcase-names`. | Unknown key in a `0.1` source document. |
| `tests.<name>.lisa.framework` and its `git-url`/`ref` children | Replaced legacy spelling | Rename the table to `source`; child spellings are unchanged. | Unknown key in a `0.1` source document. |
| Legacy root `test-suites` and image `tests.test-suites` | Removed legacy object shape | Migrate definitions to `tests`, memberships to `test-groups`, and references to `tests.tests`. | Unknown root or nested key. |

A tool MAY offer a separate compatibility import mode, but it MUST NOT describe
documents accepted only through that mode as conforming source documents and
MUST NOT silently reinterpret excluded keys during a conformance claim.

This revision defines no additional deprecation aliases, grace periods,
automatic version migration, extension namespace, or removal schedule. The
accepted `publish.channel` fallback and the explicit replacement spellings in
the table are the complete approved compatibility behavior for `0.1`.

Long-term version compatibility, deprecation timing, and removal policy remain
deferred. A future policy cannot be inferred from the presence of one accepted
deprecated field.

[Return to index](./index.md)

# Packages and publishing profile

Package configuration is optional **publishing profile** data. It is validated
and preserved whenever present; publication occurs only when explicitly
selected under [the common profile rules](./profiles.md).

## Package inheritance and groups

The publishing resolver consumes an evaluated package-output inventory and one
closed input record for each artifact it plans:

- `produced-name`: the exact produced package name;
- `class`: exactly `ordinary`, `debuginfo`, `debugsource`, or `source`;
- `owning-component`: the exact component identity that produced it; and
- `corresponding-ordinary`: required only for `debuginfo`, where it is the
  exact produced name of the ordinary package whose debug symbols this package
  contains; forbidden for the other three classes.

These are resolver inputs, not new TOML fields and not package-generation
rules. The evaluated inventory MUST contain exactly one record matching the
input `(owning-component, produced-name, class)`. The owning component MUST
resolve exactly once. For `debuginfo`, `corresponding-ordinary` MUST resolve to
exactly one `ordinary` record owned by that same component. A missing or
duplicate component, produced record, or corresponding ordinary record is an
error. A producer MUST NOT infer an ordinary identity by stripping a suffix.

The **package lookup identity** is:

| Package class | Group and exact-package lookup identity | Channel leaf |
| --- | --- | --- |
| `ordinary` | `produced-name` | `rpm-channel` |
| `debuginfo` | `corresponding-ordinary` | `debuginfo-channel` |
| `debugsource` | `produced-name` | `debuginfo-channel` |
| `source` | No group or exact-package lookup | `srpm-channel` |

Group membership and `components.<owning-component>.packages` lookup both use
that exact identity with no normalization, case folding, or suffix rewriting.
Zero matching groups skips the group layer; more than one matching group is an
error. The exact-package map contributes only its exact matching key. A
declared group or exact-package identity that is not a valid lookup identity
for some produced non-source package is an error after package evaluation.

For this profile only, effective package configuration uses this total
low-to-high precedence order:

1. top-level `default-package-config`;
2. `components.<component>.publish`;
3. the one `package-groups.<group>.default-package-config` whose `packages`
   array contains the package lookup identity; and
4. `components.<owning-component>.packages.<lookup-identity>`.

Later applicable values replace earlier values independently for each channel
leaf. A binary package may belong to at most one package group; multiple
membership is an error. This package-specific order is independent of the
resolved ComponentConfig provider order.

For an ordinary binary package, `rpm-channel` participates at all four layers.
For debuginfo and debugsource packages, `debuginfo-channel` participates at all
four layers using the lookup identity above. For a source package, only the
owning component's `ComponentConfig.publish.srpm-channel` participates; the
project, package-group, and exact-package layers contribute no source-package
leaf and are skipped. An absent final channel means no route. The
[publishing fixture](./examples/publishing-precedence/README.md) uses distinct
values at every applicable layer and covers all four package classes.

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `default-package-config` | Closed `PackageConfig` table | Optional; no default. | N/A. | Recursive table. | Lowest package provider. | Only `publish` is known. | Publishing profile |
| `package-groups` | Name-keyed map of closed group tables | Optional; default empty map. | Common name constraints. | Map by key. | Supplies one candidate package provider. | A package name may occur in at most one group. | Publishing profile |
| `package-groups.<group>.description` | string | Optional; no default. | Diagnostic UTF-8; path base N/A. | Scalar replace. | N/A. | No processing semantics. | Publishing profile |
| `package-groups.<group>.packages` | Array of strings | Optional; default `[]`. | Unique package lookup identities as defined above. | Array replace. | Declares group membership. | Empty/duplicate names, identities with no produced non-source package, and multi-group membership are errors. | Publishing profile |
| `package-groups.<group>.default-package-config` | Closed `PackageConfig` table | Optional; no default. | N/A. | Recursive table. | Middle package provider. | Partial configuration allowed. | Publishing profile |
| `components.<component>.packages` | Name-keyed map of closed `PackageConfig` tables | Optional; default empty map. | Exact package lookup identities for non-source packages owned by this component. | Map by key. | Highest exact-package provider. | An entry that is not a lookup identity for a produced non-source package owned by this component is an error after package evaluation. | Publishing profile |

## Publish fields

| Field | Type | Required/default | Constraints/path base | Composition | Inheritance | Invariants/errors | Disposition |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `PackageConfig.publish` | Closed table | Optional; no default. | N/A. | Recursive table. | Participates in package order. | Only the fields below are known. | Publishing profile |
| `PackageConfig.publish.rpm-channel` | string | Optional; no default. | Non-empty; neither `.` nor `..`; no `/` or `\`; path base N/A. Reserved value `none` means do not publish. | Scalar replace. | Scalar participation. | Conflicts with deprecated `channel` in the same effective object. | Publishing profile |
| `PackageConfig.publish.debuginfo-channel` | string | Optional; no default. | Same channel grammar; `none` allowed. | Scalar replace. | Scalar participation. | Applies only to `debuginfo` and `debugsource` packages. | Publishing profile |
| `PackageConfig.publish.channel` | string | Optional; no default. | Same grammar. | Scalar replace. | Scalar participation. | Deprecated alias for `rpm-channel`; accepted only when effective `rpm-channel` is absent. Both present is an error. | Deprecated publishing field |
| `ComponentConfig.publish` | Closed table | Optional; no default. | N/A. | Recursive table. | Component-level package defaults. | Applied to every produced package before exact package override. | Publishing profile |
| `ComponentConfig.publish.rpm-channel` | string | Optional; no default. | Channel grammar above. | Scalar replace. | Scalar participation. | Default for ordinary binary packages. | Publishing profile |
| `ComponentConfig.publish.srpm-channel` | string | Optional; no default. | Channel grammar above. | Scalar replace. | Scalar participation. | Applies to the source RPM only. | Publishing profile |
| `ComponentConfig.publish.debuginfo-channel` | string | Optional; no default. | Channel grammar above. | Scalar replace. | Scalar participation. | Default for `debuginfo` and `debugsource` packages. | Publishing profile |

An absent effective channel means the profile's publishing plan has no route
for that artifact; it does not invent a channel. Publication is selected only
by `publish-packages` with an exact ordered artifact list. The publication
plan retains that input order after removing no-route artifacts; it does not
sort by package name, channel, or destination. Each remaining item binds its
evaluated route, destination capability, network context, resource limits,
route-scoped opaque credential reference, and semantic idempotency identity under
[Profiles](./profiles.md#selected-operation-contracts) and
[Security](./security.md#credentials-and-sensitive-values).

Remote publication then follows the exact single-attempt fail-stop,
receipt-ledger, transaction-capability, and partial-effect rules in
[Remote publication attempts](./profiles.md#remote-publication-attempts).
Without an explicit complete-plan destination transaction, an earlier accepted
package may remain remotely effective when a later package fails; the failed
operation MUST report that effect and MUST NOT claim rollback.

Remote retention, replication timing, repository metadata bytes, and the
package build bytes themselves are outside the deterministic output boundary
of this revision.

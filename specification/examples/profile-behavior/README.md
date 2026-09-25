# Profile behavior fixture

`cases.toml` exercises the centralized distinction between data presence and
explicit operation selection in
[Optional profile data and operations](../../profiles.md). In particular,
shared RPM-build/image resource data is validated and preserved without
selecting an operation. The cases also cover core materialization, exact
RPM-build, publishing, image, test, and custom-source operation selection, and
missing explicit operation inputs. They do not simulate publication transport,
receipts, replay, retries, transactions, or remote rollback.

The root contains exactly `cases`. Every case has the exact common keys
`name`, `data`, `operation`, `operation-supported`, `data-valid`, and
`expected`. `operation` is `none` or one of the six identifiers in
[Optional profile data and operations](../../profiles.md), and `data` must
belong to that operation's profile family.

The outcome and operation select the only additional fixture fields:

| Scenario | Additional exact fields |
| --- | --- |
| no selected operation, invalid data, or unsupported capability | none |
| executable RPM-build, package publication, image-build, or image publication | `environment-complete` |
| missing required operation input | `environment-complete`, `missing-input` |
| RPM repository TLS opt-out preflight | `environment-complete`, `security-preflight` |
| executable test | none |
| executable or isolation-unavailable custom generation | `isolation-supported` |

Unknown, missing, wrong-type, cross-operation, and cross-scenario fields are
errors. These records are representative validation/preflight examples, not a
portable selected-operation request or publication simulation.

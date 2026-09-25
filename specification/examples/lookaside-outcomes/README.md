# Lookaside HTTP and fallback fixture

`cases.toml` is a representative fixture for the shared deterministic HTTPS
artifact-fetch state machine, deterministic request construction, redirect
header reconstruction, ambient request-state rejection, exact canonical
DNS/IP authorities and origins, repository GPG-key reuse, exact
identity-coded digest bytes, typed lookaside/origin outcomes, and source
selection in [Source identity and
acquisition](../../sources.md#https-artifact-fetch-and-source-selection).
It includes a non-identity encoded successful response and a premature EOF
after valid successful headers.

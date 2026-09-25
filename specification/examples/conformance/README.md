# Ordinary conformance fixture example

`manifest.toml` demonstrates the strict ordinary fixture format in
[Conformance examples and fixture format](../../conformance.md). It contains
the two retained case kinds:

- positive semantic facts;
- typed negative diagnostics.

Every case and nested variant is a closed union. The executable helper mutates
unknown, missing, wrong-type, cross-kind, and wrong-variant fields and requires
each mutation to fail.

This directory defines no suite registry, producer/consumer protocol, claim
lifecycle, selected-profile operation transport, external-operation replay,
resolved-model transport, materialization operation envelope, package-manager
parser vector, or generator execution-root closure. It provides no executable
Resolved-model or Materialized-tree success case. No conformance class is
enabled or claimed.

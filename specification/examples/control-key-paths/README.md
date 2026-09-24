# Document-control key paths

These metasyntactic fixtures distinguish exact root-table document controls
from identical strings used as names in a nested name-keyed map.

- `root.toml` has root controls and nested entries named `includes` and
  `spec-version`.
- `fragment.toml` has root `includes`, no root `spec-version`, and the same
  nested names.

The nested `objects` map is test vocabulary only and does not define a Slice 2
field.

# Source-model validation falsifier

This Slice 1 fixture uses the metasyntactic field contract
`example.names: array<string>`. It does not define an object field.

The earlier document contains an integer array element:

```toml
[example]
names = [1]
```

The later document contains a valid replacement:

```toml
[example]
names = ["valid"]
```

The input is invalid during source-model validation of `earlier-invalid.toml`.
A processor MUST NOT compose the documents first and validate only the later
winning array.

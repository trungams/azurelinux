# Custom generator profile fixture

`cases.toml` exercises the selected-operation contract in
[Custom-source generation](../../sources.md#custom-source-generation-profile)
and [custom generator isolation](../../security.md#custom-generator-isolation).
It covers declared input filenames, declared mock packages,
implementation-specific isolation availability, default-deny network access,
semantic output validation, and configured output-hash verification. It does
not define execution-root bytes, package payload closure, kernel-observation
interfaces, or a reproducible sandbox proof.

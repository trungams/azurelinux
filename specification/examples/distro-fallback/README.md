# Default distro fallback fixture

`cases.toml` exercises the whole-table fallback rule in
[Project](../../project.md). The project default applies only when the entire
component `upstream-distro` table is absent; a present partial table is an
error and is never completed field-by-field.

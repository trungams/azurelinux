
[Return to top-level objects](./objects.md)

# components

> **Slice 2 status:** This retained field page is provisional and
> non-normative in Slice 1. It does not yet define canonical key spellings,
> types, requiredness, defaults, composition, inheritance, path bases,
> constraints, or error conditions.

The components object provides configuration for one or more components (i.e. source packages). It contains any number object fields, with free-format keys corresponding to the name of each component, and fields as described below.

Effective component values are produced during
[resolution](./resolution.md#defaults-and-inheritance-boundary), not during
TOML parsing. Inheritance layer precedence, multiple-group ordering, and
conflict handling remain [explicit open decisions](./open-decisions.md); this
chapter does not select them.

## spec

This optional object field provides the configuration for the source dist-git repository to use for this component, and is available in two different types, each with different fields.

- type
  This required string field must have a value of either 'upstream' or 'local'.
  
Spec objects with type 'local' have the following fields:

- path
  This required string field provides a path (relative to the toml file containing this config) to the local rpm spec file to use.

Spec objects with type 'upstream' have the following fields:

- upstream_distro
  This required object field provides configuration for the upstream dist-git repository to use, and contains the these fields:

  - name
    This required string field provides the name of the distribution dist-git to use.

  - version
    This required string field provides the version of the distribution dist-git to use.

  - snapshot
    This optional string field provides RFC-3339 formatted date/time of the distribution dist-git to use.

- upstream_name
  This optional string field specifies the upstream package name, and is only needed if the upstream package name differs from the local component name.

- upstream_commit
  This optional string field specifies the upstream dist-git commit hash.

## release

This optional object field must contain a single key:

- calculation
  This required string field must contain one of 'auto', 'autorelease', 'static', or 'manual'. The behavior of each value is described below:

  - manual
    If the value is 'manual', no (automatic) modification at all is made to the spec file's 'Release:' property.

  - static
    If the value is 'static', the spec file 'Release:' value must be a single integer optionally followed by '%{dist}' or '%{?dist}' (and it is an error if not), and that integer value will be automatically incremented by 1.

  - autorelease
    If the value is 'autorelease', the spec file 'Release:' value must be managed by rpmautospec, and no (automatic) modification at all is made to the spec file's 'Release:' property.

  - auto
    if the value is 'auto', and the spec file 'Release:' value contains '%autorelease', it is handled as if the value was 'autorelease'; otherwise it is handled as if the value was 'static'.
  
## overlays

This optional array field contains one or more objects which contain configuration [as described here](./overlays.md) that is used to modify the upstream dist-git contents.

## build

This optional object field contains build-time configuration [as described here](./build.md).

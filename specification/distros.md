
[Return to top-level objects](./objects.md)

# distros

> **Slice 2 status:** This retained field page is provisional and
> non-normative in Slice 1. It does not yet define canonical key spellings,
> types, requiredness, defaults, composition, inheritance, path bases,
> constraints, or error conditions.

The distros object provides configuration for one or more distributions. It contains any number object fields, with free-format keys corresponding to the name of each distribution, and fields as described below.

## dist_git_base_uri

This required string field contains a URI-format base template for the distribution's dist-git repositories.

## lookaside_base_uri

This required string field contains a URI-format base template for the distribution's lookaside caches.

## versions

This required object field contains configuration for one or more versions of a distribution. It contains any number object fields, with free-format keys corresponding to the name of each version, and fields as described below.

- dist_git_branch
  This optional string field contains the name of the dist-git branch to use.

- default_component_config
  This optional object field contains the default component configuration to use when building for this distribution version. The object format is defined in the [component configuration](./components.md).

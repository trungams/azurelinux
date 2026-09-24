
[Return to top-level objects](./objects.md)

# component_groups

> **Slice 2 status:** This retained field page is provisional and
> non-normative in Slice 1. It does not yet define canonical key spellings,
> types, requiredness, defaults, composition, inheritance, path bases,
> constraints, or error conditions.

The component_groups object provides configuration for one or more components (i.e. source packages). It contains any number object fields, with free-format keys corresponding to the name of each group, and fields as described below.

- components
  This optional array field contains string component names this group should be applied to.

- default_component_config
  This optional object field contains [component configuration](./components.md) that should be inherited by each of this group's components.

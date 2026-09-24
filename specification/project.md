
[Return to top-level objects](./objects.md)

# project

> **Slice 2 status:** This retained field page is provisional and
> non-normative in Slice 1. It does not yet define canonical key spellings,
> types, requiredness, defaults, composition, inheritance, path bases,
> constraints, or error conditions.

The project object provides general project-wide configuration, and contains the following fields.

## default_distro

This optional object field contains these fields:

- name
  This required string field contains the name of the default distro, which must match a distro defined in the top-level 'distros' object.

- version
  This required string field contains the version of the default distro, which must match a distro version defined in the distro object named above.

## rendered_specs_dir

This required string field contains the (relative, from the directory containing this config) path to the directory to the local dist-git dirs.

When a component dist-git directory is created or updated, a directory named for the first letter of the component name (converted to lowercase) will be created (if needed) in this directory, and the dist-git directory using the component name (with no lowercase conversion) will be created under the single-letter directory.

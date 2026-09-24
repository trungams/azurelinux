[Return to index](./index.md)

# Top-level object families

The [composed model](./resolution.md#composed-model) and
[resolved model](./resolution.md#resolved-model) contain the object families
that describe a project and its components.

This chapter is navigation for Slice 1. The linked object-field pages preserve
useful baseline material but are provisional and non-normative until Slice 2
defines each field's canonical spelling, type, requiredness, default,
composition behavior, inheritance behavior, path base, constraints, and error
conditions.

## Project

Project-wide identity and defaults are introduced in
[project](./project.md).

## Distros

Named distributions and versions are introduced in
[distros](./distros.md).

## Components

Named source-package components are introduced in
[components](./components.md).

Component inheritance is a resolution operation, not a TOML parsing behavior.
Its precedence remains an
[explicit open decision](./open-decisions.md#od-2-inheritance-layer-precedence).

## Component groups

Named component groups are introduced in
[component groups](./component_groups.md).

The canonical top-level field spelling is expected to be hyphenated, but its
complete field contract and multiple-group behavior belong to Slice 2. No
underscore alias is implied.

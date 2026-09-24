
[Return to components](./components.md)

# overlay

> **Delivery status:** This retained field page is provisional and
> non-normative in Slice 1. The component field that references overlay
> operations and its object-model integration are completed with Slice 2. The
> 17 operation objects listed below, including their fields, matching, ordering,
> transformation semantics, failure behavior, and other operation contracts,
> are owned by Slice 3.

The overlay object defines a specific modification for a component, and contains the following field.

- type
  This required string field specifies the type of the overlay, and must be set to one of the follow values. Each overlay type defines additional fields as described below.

## spec_add_tag

## spec_insert_tag

## spec_set_tag

## spec_update_tag

## spec_remove_tag

## spec_prepend_lines

## spec_append_lines

## spec_search_replace

## spec_remove_section

## spec_remove_subpackage

## patch_add

## patch_remove

## file_prepend_lines

## file_search_replace

## file_add

## file_remove

## file_rename

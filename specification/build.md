
[Return to components](./components.md)

# build

> **Slice 2 status:** This retained field page is provisional and
> non-normative in Slice 1. It does not yet define canonical key spellings,
> types, requiredness, defaults, composition, inheritance, path bases,
> constraints, or error conditions.

The build object provides build-related configuration for a component, and contains the following fields.

## with

This optional array field provides the string names of options to build the component with, using the **rpmbuild** parameter *--with*, e.g. *--with NAME*.

## without

This is identical to the *with* field, but uses the **rpmbuild** parameter *--without*.

## defines

This optional object field provides string key-value pairs of macros to define when building the component, using the **rpmbuild** parameter *--define*, e.g. *--define 'KEY VALUE'*.

## undefines

This optional array field provides the string names of macros to undefine when building the component, using the **rpmbuild** parameter *--undefine*, e.g. *--undefine 'NAME'*.

## check

This optional object field contains the following field.

- skip
  This required boolean field, if True, specifies that the component *%check* section will be disabled by prepending *exit 0* to the section. This defaults to False.

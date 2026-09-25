# TOML semantic-equality cases

`cases.toml` exercises the revision `0.1` TOML type lattice and
[semantic equality](../../document.md#semantic-equality). The paired values
cover NaN, infinities, signed zero, all four temporal types, equivalent
fractional precision, equivalent offset date-times, and the integer/float type
distinction.

These are metasyntactic values, not object fields.

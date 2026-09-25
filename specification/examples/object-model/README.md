# Complete object-model fixture

`complete.toml` is a parseable example of the Slice 2 canonical vocabulary. It
contains core component data plus RPM-build/image shared resource, publishing,
image, and test profile data. Presence selects validation and preservation, not
an operation. It deliberately avoids excluded tool layout fields and uses
an exact source-controlled upstream commit.

The file is an object-model fixture, not a materialized-tree fixture. Overlay
operation effects, image bytes, RPM build environment, and publishing transport
remain outside its expected result.

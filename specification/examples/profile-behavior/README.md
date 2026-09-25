# Profile behavior fixture

`cases.toml` exercises the centralized distinction between data presence and
explicit operation selection in
[Optional profile data and operations](../../profiles.md). In particular,
shared RPM-build/image resource data is validated and preserved without
selecting an operation. The cases also cover core materialization, exact
RPM-build, publishing, image, test, and custom-source operation selection, and
missing explicit operation inputs.

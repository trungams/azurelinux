# Repeated canonical-document reaches

`cases.toml` describes the four minimum non-cycle repeated-reach categories.
Each case has two reaches outside the active recursion chain whose operational
canonical identities are equal. The second reach is therefore a repeated
canonical-document reach even when both reaches have the same parent.

Each case provides an ordered canonical-node graph. The repeated shared node
has an observable descendant edge, so retraversing its outgoing subtree changes
the derived reach order even if the repeated node body itself is skipped.
Validation derives reach order, first-evaluation body order, and composed
subtree contributions from the graph; the fixture does not supply counters.

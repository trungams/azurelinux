# Pin refresh ordering fixture

`normal-root.toml` reaches the producer-managed contribution before the
author-controlled component contribution. Refresh is permitted and ordinary
composition leaves the author pin effective.

`reversed-root.toml` reaches the author contribution first. A refresh producer
must reject that layout before changing either TOML file because updating the
later managed contribution would override the author pin.

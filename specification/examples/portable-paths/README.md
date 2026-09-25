# Portable relative path fixture

`cases.toml` exercises the reusable lexical normalization and rejection rules
in [Document loading](../../loading.md#portable-relative-model-paths), plus the
portable relative-path pattern syntax and project-tree traversal. The semantic
oracle builds a real project tree for target-kind, symlink, and complete
literal-directory enumeration checks, including a valid target beside an
unrelated invalid-UTF-8 sibling.

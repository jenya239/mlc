# mlc editor (planned)

Native text editor dogfooding MLC + `misc/gui`.

**Do not implement here until** `TRACK_EDITOR_MVP` STEP=0 is activated
(see [docs/EDITOR.md](../../docs/EDITOR.md) and
[docs/agent/TRACK_EDITOR_MVP.md](../../docs/agent/TRACK_EDITOR_MVP.md)).

Intended layout after STEP=0+:

```text
document/  layout/  syntax/  workspace/  ui/  commands/  tests/  main.mlc
ARCHITECTURE.md   # module boundaries (authoritative once work starts)
```

Run commands will land in this README when the binary exists.

# Reddit demo — media checklist (manual)

Track: [TRACK_REDDIT_DEMO.md](TRACK_REDDIT_DEMO.md). Script: `scripts/reddit_demo.sh`. Baseline: [reddit_demo_baseline.txt](reddit_demo_baseline.txt).

## Before recording

- [ ] `compiler/out/mlcc` present (`compiler/build.sh` or `MLCC_FORCE_RUBY=1 compiler/build.sh`)
- [ ] `scripts/reddit_demo.sh --run` prints test summary and self-host timing
- [ ] Terminal: dark theme, font ≥14pt, width ≥100 cols
- [ ] `export TMPDIR=$PWD/tmp` if `/tmp` is tight

## Screenshot (static post / README)

Capture one frame after:

```bash
scripts/reddit_demo.sh --run
```

Checklist:

- [ ] Visible: `selfhost_codegen_sec`, `compiler_tests=… passed, 0 failed`, `selfhost_generated_cpp`
- [ ] No secrets, paths, or unrelated windows in frame
- [ ] Save as `docs/agent/reddit_demo_screenshot.png` (or attach to post only)

Optional second frame: `compiler/tests/e2e/run_e2e.sh compiler/out/mlcc` ending with `6 passed, 0 failed`.

## Asciinema (terminal recording)

Install: `asciinema` (https://asciinema.org/docs/installation).

```bash
asciinema rec reddit_demo.cast
# inside the recording shell:
scripts/reddit_demo.sh --commands
echo
scripts/reddit_demo.sh --run
compiler/tests/e2e/run_e2e.sh compiler/out/mlcc
exit
asciinema upload reddit_demo.cast   # optional; note URL in BLOG draft (STEP=4)
```

Checklist:

- [ ] Recording ≤90 s; no long pauses
- [ ] Commands match `scripts/reddit_demo.sh --commands`
- [ ] Exit codes 0 for `--run` and e2e
- [ ] Store `.cast` locally or upload URL documented for STEP=4

## Numbers to quote (from baseline)

See [reddit_demo_baseline.txt](reddit_demo_baseline.txt): codegen p1/p2 seconds, `diff_exit=0`, test count.

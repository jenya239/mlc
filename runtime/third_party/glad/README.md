# Vendored GLAD2 (generated, not hand-written)

Loader sources for desktop OpenGL and headless EGL/GLES2.
**Do not edit** — regenerate and replace the whole tree.

## Decision (locked 2026-07-11)

| Item | Value |
|------|-------|
| Tool | `glad2==2.0.8` (`python -m glad`, same generator as https://glad.sh) |
| Why not web POST | `POST https://gen.glad.sh/generate` returned 302→flash error / follow 405 (2026-07-11); vendored via CLI once |
| Desktop | `gl:core=3.3`, no extensions, C + loader → `gl/` |
| Headless | `egl=1.5,gles2=2.0` + `EGL_EXT_platform_base`, `EGL_MESA_platform_surfaceless`, `EGL_KHR_surfaceless_context` → `egl/` |
| Link | compile `gl/src/gl.c` and `egl/src/{egl,gles2}.c` in `build_bin.sh` / GUI link (STEP=3) — not `extern lib` |
| Include | `-I runtime/third_party/glad/gl/include` or `…/egl/include` |

## Regenerate

```bash
python3 -m venv /tmp/glad2_venv && /tmp/glad2_venv/bin/pip install 'glad2==2.0.8'
/tmp/glad2_venv/bin/python -m glad --api='gl:core=3.3' --out-path=/tmp/glad_gl \
  --reproducible --extensions='' c --loader
/tmp/glad2_venv/bin/python -m glad --api='egl=1.5,gles2=2.0' --out-path=/tmp/glad_egl \
  --reproducible \
  --extensions='EGL_EXT_platform_base,EGL_MESA_platform_surfaceless,EGL_KHR_surfaceless_context' \
  c --loader
# then replace gl/ and egl/ from those out-paths
```

## Inventory check (STEP=1)

All GL entry points used by `glfw_gl_dispatch` wrappers exist in GL Core 3.3
(`glClear`…`glDrawArrays`…`glTexImage2D`…). Headless smoke needs
`eglGetProcAddress` + `glGenBuffers`/`glDeleteBuffers` (present in `egl/`).
GLFW window/context/input wrappers stay outside glad (not GL symbols).

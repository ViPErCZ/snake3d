# Contributing

Thanks for your interest. snake3d is in early development, so the bar for
contributions is currently **discussion-first**: open an issue describing the
change before sending a PR, especially for anything beyond a bug fix.

## Engine / game boundary

Read [ENGINE_BOUNDARY.md](ENGINE_BOUNDARY.md) before adding files. Engine code
(under repo root) must stay free of Snake3-specific vocabulary; game code
lives under `examples/snake3/src/`. The CMake split enforces this at the
target level (`snake3d_engine` vs `snake3d_game`).

## Building & running

See the [README](README.md#build).

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target Snake3
cmake --build build --target Tests
cd build && ctest --output-on-failure
```

All PRs must build cleanly with both `Debug` and `Release` configurations and
keep `ctest` green.

## Code style

- C++23.
- 4-space indentation, no tabs.
- `.clang-format` configured (Google base with project tweaks). Run
  `clang-format -i` on touched files before committing.
- Don't introduce `using namespace` in headers — qualify names instead.
  Headers are exported to the public engine API.
- Prefer composition over inheritance for engine features.

## Commit style

- Short imperative subject line (under ~70 chars).
- Reference the roadmap phase tag where applicable (`[H4]`, `[D1]`, etc.).
- Wrap body at 72.

## Testing

- Catch2 v3 unit tests live under `Tests/`.
- New engine features should ship with tests where feasible (no GL context
  available in CTest — pure data structures, math, parsers OK).
- Material/shader changes: run `./build/Snake3` from `build/` and verify the
  pre-flight log reports `0 failed` shaders.

## License

By contributing you agree your work is licensed under [MIT](LICENSE).

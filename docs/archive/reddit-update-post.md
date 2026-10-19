**Title:** Update on the compiler I built with LLM agents - it self-hosts now, and I went way too deep into the orchestration

---

About five months ago I posted about building mlc, a language that transpiles to C\+\+20. Quick recap for those who missed it: algebraic types, pattern matching, generics - roughly a safer/cleaner syntax over C\+\+, with the heavy lifting (type safety, exhaustiveness checking) delegated to the C\+\+ compiler. ~85% agent-generated, Ruby bootstrap compiler, 2500+ tests. The post was mostly about how agents aren't magic and you need TDD or everything falls apart.

A lot happened since then.

---

**The self-hosting thing**

The main goal was always to get the compiler written in mlc itself. That's done now. The compiler compiles its own source - 143 .mlc files going through the C\+\+ backend to produce a working binary. The bootstrap round-trip (Ruby compiler ? mlcc binary ? mlcc compiles itself ? compare output) passes. So technically we're there.

"Technically" is doing a lot of work in that sentence. The generated C\+\+ is functional but not pretty. Error messages are still terrible. There's no incremental compilation. The self-hosted binary and the Ruby bootstrap agree on output, but that just means they have the same bugs.

Ruby is still needed to build mlcc from scratch - it's the bootstrap compiler. Once you have a mlcc binary you don't need Ruby anymore. Same situation as Go or any other self-hosted compiler.

---

**I built tooling around the agents and it got out of hand**

The original workflow was: write test, prompt agent, fix failures, repeat. That worked but I kept losing track of what was done, what was blocked, where we were in the plan. So I started keeping state in markdown files - PLAN.md, per-feature TRACK_*.md files with step tables and status, SESSION.md logging each agent turn.

Then I wanted to rotate between roles automatically - not every turn is implementation work, sometimes you need a planner pass, a critic pass, cleanup. So I wrote a small orchestration server in TypeScript that reads the track files, picks the next role and step, builds the prompt, and sends it to the Cursor agent via their MCP protocol.

Then I added a watchdog because agents get stuck. Then a web UI to monitor progress. Then SQLite for turn history. At some point I was spending more time on the orchestration infrastructure than on the compiler itself.

The system is at github.com/jenya239/cursor-agent-loop if anyone wants to look. It's very specific to my setup and probably not directly useful to anyone else, but the approach - external state files, role rotation, explicit track/step tracking - I think generalizes.

---

**What changed about the workflow**

The original post described pretty manual iteration: write test, prompt, fix. That's still the core, but now most of the prompting is automated. The agent gets a structured nudge every few minutes if idle, with explicit role, step, and context loaded from the relevant spec file. I intervene when something is blocked or the agent is going in circles.

The self-hosting milestone was reached this way - mostly autonomous, me checking in periodically and steering when needed. Less babysitting than before, but I wouldn't call it autonomous. More like: the agent knows what to work on, I know what it's working on, and I step in when the loop breaks.

---

**The memory model**

No GC, no borrow checker. Collections use copy-on-write - copying an array is O(1) and shares the buffer until someone mutates it. Closures always capture by value. For trees there's `Shared<T>` (reference counted) and `Weak<T>` to break cycles.

Compared to Python/Ruby: no use-after-free, no null pointer dereferences (there's no null, only `Option<T>`), no iterator invalidation from mutating a collection mid-loop. Things that silently corrupt state in dynamic languages just don't compile here.

    // this is a type error - can't ignore the None case
    let x: i32 = maybe_value   // must unwrap or match

Compared to C\+\+: you can't accidentally share a mutable reference to a collection and have two places modify it at once - mutations go through COW so each caller gets their own copy. No dangling references from closures - capture is always by value. No std::vector invalidation from push_back while iterating.

    let arr = [1, 2, 3]
    let arr2 = arr        // O(1), shared buffer
    arr2.push(4)          // arr is still [1, 2, 3], arr2 is [1, 2, 3, 4]

What you still have to think about: cyclic structures need explicit `Weak<T>` or you leak. The compiler won't tell you when you've created a cycle.

---

**No multithreading**

Single-threaded only right now. The reference counting is atomic but COW detach isn't, so sharing collections across threads would break. No Send/Sync equivalent either. The eventual plan is message-passing (no shared mutable state) but that's not close.

---

**Current state**

- Self-hosting works (compiler compiles itself, round-trip passes)
- ~2100 tests (~1100 Ruby bootstrap + ~980 self-hosted compiler suite)
- Language features: sum types, generics, pattern matching (incl. guards, string patterns), records, closures, modules, traits, Option/Result, pipe operator, type aliases
- Missing/in progress: full span coverage in error messages, optimization, incremental compilation
- Code quality of the self-hosted binary: works, could be cleaner

One concrete win: compile speed. Compiling the compiler itself (143 source files ? 92 C++ files) with warm ccache:

- Ruby bootstrap: ~13s
- mlcc codegen only: ~1.7s
- mlcc full build (codegen + clang++): ~4.3s

So mlcc is ~7-8x faster at the codegen step and ~3x faster end-to-end. Cold cache the first time is ~6 minutes either way - bottlenecked by clang++ compiling 92 files.

Next thing is stabilization - the compiler handles the cases that exist in its own source, but there are language features it doesn't use that are partially broken. Need to actually stress-test the self-hosted path before claiming it's real.

---

**Token usage**

Someone asked about costs in the original thread so here are actual numbers for the whole project. I ran everything through Cursor Ultra (which bills at the token level).

Total tokens: ~5.13 billion. That sounds absurd until you look at the breakdown: 4.82B of those are cache reads. Cursor aggressively caches context, so the same large prompt (architecture docs, current file, track state) gets read from cache hundreds of times per session. Real non-cached tokens are around 311M.

By model: most volume went through composer-2.5-fast and composer-2-fast for routine implementation turns. The Sonnet/Opus models got used for planning and architecture passes - much lower volume but higher quality. I applied a 30% adjustment to Sonnet usage to account for Cursor's pricing on those models.

The cache dominance makes sense given the workflow - every agent turn reloads roughly the same context (CONTINUITY.md, ROLES.md, current track) which stays in cache across turns.

---

Code: github.com/jenya239/mlc

If this was useful, I accept donations at izkaregn.com.

# Module System Architecture - Phase 24

## Problem Statement

**Current Issue:**
- Multi-file compilation fails because each `.mlc` file is compiled in isolation with separate IRGen instances
- When `main.mlc` imports from `./math.mlc`, functions from `math.mlc` are NOT in function_registry
- Import aliases cannot be created because imported symbols don't exist during compilation
- Tests added Oct 17, 2025 showed "All 73 tests passing", but broke after subsequent refactoring

**Root Cause:**
Each compilation creates a fresh IRGen instance → Fresh function_registry → No knowledge of symbols from other modules

## Solution: Metadata Files (.mlcmeta) - Rust Model

### Architecture Overview

**Compilation of `math.mlc`:**
```
Input:  math.mlc
Output: math.hpp (C++ header)
        math.cpp (C++ implementation)
        math.mlcmeta (JSON metadata) ← NEW
```

**Metadata Format (`math.mlcmeta`):**
```json
{
  "module_name": "math",
  "exports": {
    "functions": [
      {
        "name": "add",
        "params": [
          {"name": "a", "type": "i32"},
          {"name": "b", "type": "i32"}
        ],
        "return_type": "i32",
        "is_exported": true
      },
      {
        "name": "multiply",
        "params": [
          {"name": "a", "type": "i32"},
          {"name": "b", "type": "i32"}
        ],
        "return_type": "i32",
        "is_exported": true
      }
    ],
    "types": [
      {
        "name": "Vec2",
        "kind": "record",
        "fields": [
          {"name": "x", "type": "f32"},
          {"name": "y", "type": "f32"}
        ],
        "is_exported": true
      }
    ],
    "sum_types": [
      {
        "name": "Result",
        "type_params": ["T", "E"],
        "variants": ["Ok", "Err"],
        "is_exported": true
      }
    ]
  }
}
```

**Compilation of `main.mlc` (imports from math):**
```
1. Parse main.mlc AST
2. Find import declarations: import { add, multiply } from "./math"
3. BEFORE type-checking:
   - Look for ./math.mlcmeta
   - Parse metadata
   - Register exported functions in function_registry
   - Register exported types in type_registry
4. Continue compilation with full symbol knowledge
5. Generate #include "./math.hpp" in main.hpp
```

### Comparison to Other Compilers

| Compiler | Approach | Metadata File | Type-checking |
|----------|----------|---------------|---------------|
| **Rust** | Metadata | `.rmeta` (binary) | Full, at compile time |
| **OCaml** | Interface | `.cmi` (compiled interface) | Full, at compile time |
| **TypeScript** | Source/Types | `.d.ts` (declaration files) | Full, at compile time |
| **C/C++** | Headers only | None (uses .h/.hpp) | Delegated to C++ compiler |
| **Go** | Source parsing | None (parses .go files) | Full, at compile time |
| **MLC (Proposed)** | **Metadata** | **`.mlcmeta` (JSON)** | **Full, at compile time** |

**Why Metadata over Headers?**
- C++ headers contain full implementation details (templates, inline functions)
- We only need type signatures for type-checking
- Metadata is language-agnostic (can target LLVM, WASM, etc. in future)
- Faster to parse than C++ headers

**Why JSON over Binary?**
- Human-readable for debugging
- Easy to inspect and version control
- Simple to parse
- Can switch to binary later for performance

### Implementation Plan

#### Phase 24-A: Metadata Generation
**File**: `lib/mlc/codegen/metadata_generator.rb`

```ruby
module MLC
  module CodeGen
    class MetadataGenerator
      def generate(semantic_ir_module)
        {
          module_name: semantic_ir_module.name,
          exports: {
            functions: extract_exported_functions(semantic_ir_module),
            types: extract_exported_types(semantic_ir_module),
            sum_types: extract_exported_sum_types(semantic_ir_module)
          }
        }
      end

      def write(metadata, output_path)
        File.write(output_path, JSON.pretty_generate(metadata))
      end
    end
  end
end
```

**Integration**: Modify `MLC.to_hpp_cpp` to also generate `.mlcmeta`

```ruby
def self.to_hpp_cpp(source, output_path: nil)
  semantic_ir_module = transform_to_core_with_registry(source)

  # Generate C++ files (existing)
  hpp = CppCodeGen.generate_header(semantic_ir_module)
  cpp = CppCodeGen.generate_implementation(semantic_ir_module)

  # NEW: Generate metadata
  metadata = CodeGen::MetadataGenerator.new.generate(semantic_ir_module)

  if output_path
    File.write("#{output_path}.hpp", hpp)
    File.write("#{output_path}.cpp", cpp)
    File.write("#{output_path}.mlcmeta", metadata.to_json)
  end

  {header: hpp, implementation: cpp, metadata: metadata}
end
```

#### Phase 24-B: Metadata Loading
**File**: `lib/mlc/services/metadata_loader_service.rb`

```ruby
module MLC
  module Services
    class MetadataLoaderService
      def initialize(function_registry:, type_registry:, type_decl_table:)
        @function_registry = function_registry
        @type_registry = type_registry
        @type_decl_table = type_decl_table
      end

      # Load metadata from .mlcmeta file and register symbols
      def load(metadata_path)
        return unless File.exist?(metadata_path)

        metadata = JSON.parse(File.read(metadata_path))

        # Register exported functions
        metadata["exports"]["functions"].each do |func_meta|
          register_function_from_metadata(func_meta, metadata["module_name"])
        end

        # Register exported types
        metadata["exports"]["types"].each do |type_meta|
          register_type_from_metadata(type_meta)
        end

        # Register sum types
        metadata["exports"]["sum_types"].each do |sum_type_meta|
          register_sum_type_from_metadata(sum_type_meta)
        end
      end

      private

      def register_function_from_metadata(func_meta, module_name)
        # Convert metadata to function signature
        param_types = func_meta["params"].map { |p| parse_type(p["type"]) }
        return_type = parse_type(func_meta["return_type"])

        @function_registry.register(
          name: func_meta["name"],
          param_types: param_types,
          return_type: return_type,
          module_name: module_name
        )
      end

      # ... similar for types and sum types
    end
  end
end
```

#### Phase 24-C: Import Resolution with Metadata
**File**: `lib/mlc/irgen/program_transformer.rb`

Modify `pass_collect_imports`:

```ruby
def pass_collect_imports(context)
  program = context[:program]

  program.imports.each do |import_decl|
    if @stdlib_resolver.stdlib_module?(import_decl.path)
      # Stdlib: immediate registration
      register_stdlib_imports(import_decl)
    else
      # User module: load metadata BEFORE deferring
      load_user_module_metadata(import_decl, context)

      # Defer alias registration until Pass 4
      context[:user_module_imports] << import_decl
    end
  end
end

def load_user_module_metadata(import_decl, context)
  # Resolve path: "./math" → "/abs/path/to/math.mlcmeta"
  metadata_path = resolve_metadata_path(import_decl.path, context)

  return unless File.exist?(metadata_path)

  # Load metadata into registries
  @metadata_loader_service.load(metadata_path)
end
```

#### Phase 24-D: Path Resolution
**File**: `lib/mlc/services/module_path_resolver.rb`

```ruby
module MLC
  module Services
    class ModulePathResolverService
      # Resolve relative import path to absolute .mlcmeta file
      # "./math" → "/abs/path/to/math.mlcmeta"
      # "../core/utils" → "/abs/path/to/core/utils.mlcmeta"
      def resolve_metadata_path(import_path, current_file_dir:)
        # Handle relative paths
        if import_path.start_with?("./", "../")
          abs_path = File.expand_path(import_path, current_file_dir)
          "#{abs_path}.mlcmeta"
        else
          # Future: Handle package imports
          raise "Package imports not yet supported: #{import_path}"
        end
      end
    end
  end
end
```

### Implementation Order

1. **Phase 24-A**: Metadata generation (1-2 days)
   - Create MetadataGenerator
   - Integrate into MLC.to_hpp_cpp
   - Test metadata output format

2. **Phase 24-B**: Metadata loading (2-3 days)
   - Create MetadataLoaderService
   - Implement function/type registration from metadata
   - Test with simple examples

3. **Phase 24-C**: Import resolution (1-2 days)
   - Modify pass_collect_imports to load metadata
   - Update ProgramTransformer

4. **Phase 24-D**: Path resolution (1 day)
   - Create ModulePathResolverService
   - Handle relative paths (./,../)
   - Test multi-directory projects

5. **Phase 24-E**: Integration testing (2-3 days)
   - Run multi_file_modules_test.rb
   - Fix edge cases
   - Document behaviour

**Total Estimate**: 7-11 days

### Benefits

✅ **Full type-checking** at MLC level (not delegated to C++)
✅ **Incremental compilation** - only recompile changed files
✅ **Separate compilation** - files don't need to be compiled together
✅ **Fast** - JSON parsing faster than C++ header parsing
✅ **Extensible** - easy to add new metadata (visibility, generics, etc.)
✅ **Rust-like** - industry-proven approach

### Alternative: C++ Header-Only Model

**Simpler but incomplete:**
```ruby
# Just generate #include, skip type-checking
def pass_collect_imports(context)
  program.imports.each do |import_decl|
    context[:imports] << SemanticIR::Import.new(
      path: import_decl.path,
      items: import_decl.items
    )
    # No symbol registration - let C++ compiler handle it
  end
end
```

✅ Simple
❌ No type-checking at MLC level
❌ Errors only at C++ compilation (late feedback)
❌ Can't do MLC-level optimizations

**Verdict**: Metadata approach is better for long-term project health.

---

## Next Steps

1. Review this architecture plan
2. Get approval for Phase 24 work
3. Start implementation with Phase 24-A (metadata generation)

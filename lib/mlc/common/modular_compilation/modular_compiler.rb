# frozen_string_literal: true

require "fileutils"

require "etc"

module MLC
  module Common
    module ModularCompilation
      # Compiles a project in modular mode: per-module .hpp and .cpp, optional g++ build.
      # Steps 19-21 of modular compilation plan.
      PARALLEL_JOBS = begin
        n = ENV["MLC_JOBS"].to_i
        n > 0 ? n : [Etc.nprocessors, 1].max
      end

      class ModularCompiler
        def initialize(entry_path:, out_dir:, root_dir: nil, binary_name: "app")
          @entry_path = File.expand_path(entry_path)
          @out_dir = File.expand_path(out_dir)
          @root_dir = root_dir ? File.expand_path(root_dir) : File.dirname(@entry_path)
          @binary_name = binary_name
        end

        def compile
          graph = ModuleGraph.new(root_dir: @root_dir)
          graph.add_file(@entry_path)
          order = graph.dependency_order

          pipeline = build_pipeline
          modules_ir = {}

          build_progress("semantic: #{order.size} module(s)")
          order.each_with_index do |path, module_index|
            source = File.read(path)
            ast = MLC.parse(source, filename: path)
            module_name = File.basename(path, ".mlc")
            pipeline.services.module_path_resolver.source_file_dir = File.dirname(path)
            build_progress("  semantic #{module_index + 1}/#{order.size}: #{module_name}")
            ir = pipeline.transform(ast, module_name: module_name)
            modules_ir[path] = { ir: ir, module_name: module_name }
          end

          FileUtils.mkdir_p(@out_dir)
          cpp_files = []
          hpp_files = []

          stdlib_scanner = MLC::Common::Stdlib::Scanner.new
          lowerer = MLC::Backends::Cpp::Codegen.new(
            type_registry: pipeline.type_registry,
            function_registry: pipeline.function_registry,
            trait_registry: pipeline.services.trait_registry,
            stdlib_scanner: stdlib_scanner
          )
          generator = MLC::Backends::Cpp::HeaderGenerator.new(lowerer)

          all_modules = order.map { |path| modules_ir[path][:ir] }
          build_progress("codegen: #{order.size} module(s)")
          order.each_with_index do |path, module_index|
            mod = modules_ir[path]
            base = mod[:module_name]
            build_progress("  emit #{module_index + 1}/#{order.size}: #{base}.cpp")
            result = generator.generate(mod[:ir], all_modules: all_modules)
            hpp_path = File.join(@out_dir, "#{base}.hpp")
            cpp_path = File.join(@out_dir, "#{base}.cpp")
            write_if_changed(hpp_path, result[:header])
            write_if_changed(cpp_path, result[:implementation])
            hpp_files << hpp_path
            cpp_files << cpp_path
          end

          { cpp_files: cpp_files, hpp_files: hpp_files }
        end

        def build
          result = compile
          entry_base = File.basename(@entry_path, ".mlc")
          main_cpp = File.join(@out_dir, "#{entry_base}.cpp")
          wrapped = wrap_main(File.read(main_cpp))
          build_progress("wrap entry: #{entry_base}.cpp")
          write_if_changed(main_cpp, wrapped)

          bin_path = File.join(@out_dir, @binary_name)
          build_progress("link via build_bin.sh: #{File.basename(bin_path)}")
          build_bin = find_build_bin_script
          env = { "MLCC_ENTRY_BASENAME" => entry_base }
          ok = system(env, build_bin, @out_dir, bin_path)
          raise("build_bin.sh failed: #{build_bin} #{@out_dir} #{bin_path}") unless ok

          { binary: bin_path, cpp_files: result[:cpp_files], hpp_files: result[:hpp_files] }
        end

        private

        def write_if_changed(path, content)
          return if File.exist?(path) && File.read(path) == content

          File.write(path, content)
        end

        def find_build_bin_script
          script = File.join(File.dirname(find_runtime_dir), "compiler", "build_bin.sh")
          raise("build_bin.sh not found: #{script}") unless File.executable?(script)

          script
        end

        # Progress lines go to stderr so stdout stays suitable for piping (e.g. tee).
        # Enable with MLCC_BUILD_VERBOSE=1 (or true/yes; disable with 0/false/no).
        def verbose_build?
          value = ENV["MLCC_BUILD_VERBOSE"]
          return false if value.nil? || value.empty?

          !%w[0 false no].include?(value.downcase)
        end

        def build_progress(message)
          warn("[mlc build] #{message}") if verbose_build?
        end

        def build_pipeline
          type_registry = MLC::Registries::TypeRegistry.new
          function_registry = MLC::Registries::FunctionRegistry.new
          MLC::Representations::Semantic::Gen::Pipeline.new(
            type_registry: type_registry,
            function_registry: function_registry,
            source_file_dir: @root_dir
          )
        end

        def find_runtime_dir
          base = File.expand_path("../../../..", __dir__)
          rt = File.join(base, "runtime")
          File.directory?(rt) ? rt : File.expand_path("runtime", Dir.pwd)
        end

        def wrap_main(cpp_content)
          entry_base = File.basename(@entry_path, ".mlc").downcase
          reserved = %w[main stdin stdout stderr errno]
          ns = reserved.include?(entry_base) ? "mlc_#{entry_base}" : entry_base
          qualified = "::#{ns}::mlc_user_main"

          <<~CPP
            #include "mlc.hpp"

            #define main mlc_user_main
            #{cpp_content}
            #undef main

            static void mlc_cli_set_args(int argc, char** argv) {
              std::vector<mlc::String> arguments;
              arguments.reserve(argc > 0 ? argc - 1 : 0);
              for (int i = 1; i < argc; ++i) {
                arguments.emplace_back(argv[i]);
              }
              mlc::io::set_args(std::move(arguments));
            }

            int main(int argc, char** argv) {
              mlc_cli_set_args(argc, argv);
              #{cpp_content.match?(/\bvoid\s+main\s*\(/) ? "#{qualified}(argc, argv); return 0;" : "return #{qualified}(argc, argv);"}
            }
          CPP
        end
      end
    end
  end
end

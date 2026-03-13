# frozen_string_literal: true

require "fileutils"

module MLC
  module Common
    module ModularCompilation
      # Compiles a project in modular mode: per-module .hpp and .cpp, optional g++ build.
      # Steps 19-21 of modular compilation plan.
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

          order.each do |path|
            source = File.read(path)
            ast = MLC.parse(source, filename: path)
            module_name = File.basename(path, ".mlc")
            pipeline.services.module_path_resolver.source_file_dir = File.dirname(path)
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
            stdlib_scanner: stdlib_scanner
          )
          generator = MLC::Backends::Cpp::HeaderGenerator.new(lowerer)

          all_modules = order.map { |path| modules_ir[path][:ir] }
          order.each do |path|
            mod = modules_ir[path]
            result = generator.generate(mod[:ir], all_modules: all_modules)
            base = mod[:module_name]
            hpp_path = File.join(@out_dir, "#{base}.hpp")
            cpp_path = File.join(@out_dir, "#{base}.cpp")
            File.write(hpp_path, result[:header])
            File.write(cpp_path, result[:implementation])
            hpp_files << hpp_path
            cpp_files << cpp_path
          end

          { cpp_files: cpp_files, hpp_files: hpp_files }
        end

        def build
          result = compile
          runtime_dir = find_runtime_dir
          runtime_include = File.join(runtime_dir, "include")
          runtime_cpp = [
            File.join(runtime_dir, "src/core/string.cpp"),
            File.join(runtime_dir, "src/io/io.cpp")
          ].select { |p| File.exist?(p) }

          entry_base = File.basename(@entry_path, ".mlc")
          main_cpp = File.join(@out_dir, "#{entry_base}.cpp")
          main_content = File.read(main_cpp)
          wrapped = wrap_main(main_content)
          File.write(main_cpp, wrapped)

          obj_dir = File.join(@out_dir, "obj")
          FileUtils.mkdir_p(obj_dir)
          objs = []

          extra_flags = (ENV["MLC_CXX_FLAGS"] || "").split

          result[:cpp_files].each do |cpp|
            obj = File.join(obj_dir, "#{File.basename(cpp, ".cpp")}.o")
            cmd = ["g++", "-std=c++20", "-O2", *extra_flags, "-I", @out_dir, "-I", runtime_include, "-c", cpp, "-o", obj]
            system(*cmd) || raise("g++ failed: #{cmd.join(' ')}")
            objs << obj
          end

          runtime_cpp.each do |rcpp|
            obj = File.join(obj_dir, "runtime_#{File.basename(rcpp, ".cpp")}.o")
            cmd = ["g++", "-std=c++20", "-O2", *extra_flags, "-I", runtime_include, "-c", rcpp, "-o", obj]
            system(*cmd) || raise("g++ failed: #{cmd.join(' ')}")
            objs << obj
          end

          bin_path = File.join(@out_dir, @binary_name)
          cmd = ["g++", "-std=c++20", *extra_flags, "-o", bin_path, *objs]
          system(*cmd) || raise("g++ link failed: #{cmd.join(' ')}")

          { binary: bin_path, cpp_files: result[:cpp_files], hpp_files: result[:hpp_files] }
        end

        private

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
          entry_base = File.basename(@entry_path, ".mlc")
          ns = (entry_base == "main" ? "mlc_main" : entry_base.downcase)
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
              return #{qualified}(argc, argv);
            }
          CPP
        end
      end
    end
  end
end

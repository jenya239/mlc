#include "module.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "transform.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "decl.hpp"

namespace module {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace transform;
using namespace context;
using namespace cpp_naming;
using namespace decl;
using namespace ast_tokens;

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept;

context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept;

mlc::String gen_program(ast::Program program) noexcept;

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<decl_index::LoadItem> all_items) noexcept{return context::PrecomputedCtx{decl_index::build_field_orders(prog), decl_index::build_variant_types_from_decls(prog.decls), decl_index::build_item_index(all_items), ctor_info::build_ctor_type_infos_from_decls(prog.decls), decl_index::build_generic_variants_from_decls(prog.decls)};}

context::GenModuleOut gen_module(semantic_ir::SLoadItem s_item, mlc::Array<decl_index::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept{
mlc::String base = cpp_naming::path_to_module_base(s_item.path);
mlc::HashMap<mlc::String, mlc::String> qualified = decl_index::build_qualified(s_item.imports, all_items);
context::CodegenContext context = context::CodegenContext{precomp.field_orders, mlc::String(""), qualified, mlc::String(""), decl_index::build_method_owners_from_decls(full_prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, precomp.ctor_type_infos, precomp.variant_types, {}, {}, precomp.generic_variants};
mlc::String module_namespace = base == mlc::String("main") ? mlc::String("mlc_main") : base;
bool is_entry = decl::decls_have_main(s_item.decls);
mlc::String std_includes = mlc::String("#include \"mlc.hpp\"\n#include <variant>\n\n") + cpp_naming::include_lines(s_item.imports) + mlc::String("\n");
mlc::Array<mlc::String> type_fwds = decl::collect_decl_parts(s_item.decls, context, 0);
mlc::Array<mlc::String> type_defs = decl::collect_decl_parts(s_item.decls, context, 1);
mlc::Array<mlc::String> fn_protos = decl::collect_decl_parts(s_item.decls, context, 2);
mlc::Array<mlc::String> fn_defs = decl::collect_decl_parts(s_item.decls, context, 3);
mlc::String guard = base.upper() + mlc::String("_HPP");
mlc::String header = mlc::Array<mlc::String>{mlc::String("#ifndef ") + guard + mlc::String("\n"), mlc::String("#define ") + guard + mlc::String("\n"), mlc::String("\n"), std_includes, mlc::String("namespace ") + module_namespace + mlc::String(" {\n"), mlc::String("\n"), type_fwds.join(mlc::String("")), type_defs.join(mlc::String("")), fn_protos.join(mlc::String("")), mlc::String("\n"), mlc::String("} // namespace ") + module_namespace + mlc::String("\n"), mlc::String("\n"), mlc::String("#endif // ") + guard + mlc::String("\n")}.join(mlc::String(""));
mlc::String cli_wrapper = is_entry ? mlc::String("\n#undef main\n\nstatic void mlc_cli_set_args(int argc, char** argv) {\n  std::vector<mlc::String> arguments;\n  arguments.reserve(argc > 0 ? argc - 1 : 0);\n  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n  mlc::io::set_args(std::move(arguments));\n}\n\nint main(int argc, char** argv) {\n  mlc_cli_set_args(argc, argv);\n  return ::") + module_namespace + mlc::String("::mlc_user_main(argc, argv);\n}\n") : mlc::String("");
mlc::String impl = mlc::Array<mlc::String>{mlc::String("#define main mlc_user_main\n"), mlc::String("#include \"") + base + mlc::String(".hpp\"\n"), mlc::String("\n"), mlc::String("namespace ") + module_namespace + mlc::String(" {\n"), mlc::String("\n"), fn_defs.join(mlc::String("")), mlc::String("\n"), mlc::String("} // namespace ") + module_namespace + mlc::String("\n"), cli_wrapper}.join(mlc::String(""));
return context::GenModuleOut{header, impl};
}

mlc::String gen_program(ast::Program program) noexcept{
mlc::Array<decl_index::LoadItem> all_items = {};
context::PrecomputedCtx precomp = precompute(program, all_items);
registry::TypeRegistry registry = registry::build_registry(program);
decl_index::LoadItem single = decl_index::LoadItem{mlc::String("test_main"), program.decls, {}};
mlc::Array<semantic_ir::SLoadItem> s_items = transform::transform_load_items(mlc::Array<decl_index::LoadItem>{single}, registry);
context::GenModuleOut result = gen_module(s_items[0], all_items, program, precomp);
return result.h + result.c;
}

} // namespace module

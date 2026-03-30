#include "module.hpp"

#include "ast.hpp"
#include "context.hpp"
#include "cpp_naming.hpp"
#include "decl.hpp"

namespace module {

using namespace ast;
using namespace context;
using namespace cpp_naming;
using namespace decl;
using namespace ast_tokens;

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<context::LoadItem> all_items) noexcept;

mlc::String gen_program(ast::Program prog) noexcept;

mlc::String path_to_module_base(mlc::String path) noexcept;

context::GenModuleOut gen_module(context::LoadItem item, mlc::Array<context::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept;

context::PrecomputedCtx precompute(ast::Program prog, mlc::Array<context::LoadItem> all_items) noexcept{return context::PrecomputedCtx{context::build_field_orders(prog), context::build_variant_types_from_decls(prog.decls), context::build_item_index(all_items), context::build_ctor_type_infos_from_decls(prog.decls), context::build_generic_variants_from_decls(prog.decls)};}

mlc::String gen_program(ast::Program prog) noexcept{
context::CodegenContext context = context::create_codegen_context(prog);
mlc::Array<mlc::String> preamble = mlc::Array<mlc::String>{mlc::String("#include \"mlc/core/string.hpp\"\n"), mlc::String("#include \"mlc/core/array.hpp\"\n"), mlc::String("#include \"mlc/core/hashmap.hpp\"\n"), mlc::String("#include \"mlc/core/match.hpp\"\n"), mlc::String("#include \"mlc/io/io.hpp\"\n"), mlc::String("#include \"mlc/io/file.hpp\"\n"), mlc::String("#include <functional>\n"), mlc::String("#include <memory>\n"), mlc::String("#include <variant>\n"), mlc::String("#include <vector>\n\n")};
mlc::Array<mlc::String> type_fwds = decl::collect_decl_parts(prog.decls, context, 0);
mlc::Array<mlc::String> type_defs = decl::collect_decl_parts(prog.decls, context, 1);
mlc::Array<mlc::String> fn_protos = decl::collect_decl_parts(prog.decls, context, 2);
mlc::Array<mlc::String> fn_decls = decl::collect_decl_parts(prog.decls, context, 3);
return ast::errs_append(ast::errs_append(ast::errs_append(ast::errs_append(preamble, type_fwds), type_defs), ast::errs_append(fn_protos, mlc::Array<mlc::String>{mlc::String("\n")})), fn_decls).join(mlc::String(""));
}

mlc::String path_to_module_base(mlc::String path) noexcept{
int last_slash = -1;
int last_dot = path.length();
int i = 0;
while (i < path.length()){
{
if (path.char_at(i) == mlc::String("/")){
{
last_slash = i;
}
} else {
{
if (path.char_at(i) == mlc::String(".") && i > last_slash){
last_dot = i;
}
}
}
i = i + 1;
}
}
return last_dot > last_slash ? path.substring(last_slash + 1, last_dot - last_slash - 1) : path.substring(last_slash + 1, path.length() - last_slash - 1);
}

context::GenModuleOut gen_module(context::LoadItem item, mlc::Array<context::LoadItem> all_items, ast::Program full_prog, context::PrecomputedCtx precomp) noexcept{
mlc::String base = path_to_module_base(item.path);
mlc::HashMap<mlc::String, mlc::String> qualified = context::build_qualified(item.imports, all_items);
context::CodegenContext context = context::CodegenContext{precomp.field_orders, mlc::String(""), qualified, mlc::String(""), context::build_method_owners_from_decls(full_prog.decls), {}, {}, mlc::HashMap<mlc::String, mlc::String>(), {}, precomp.ctor_type_infos, precomp.variant_types, {}, {}, precomp.generic_variants};
mlc::String module_namespace = base == mlc::String("main") ? mlc::String("mlc_main") : base;
bool is_entry = decl::decls_have_main(item.decls);
mlc::String std_includes = mlc::String("#include \"mlc.hpp\"\n#include <variant>\n\n") + cpp_naming::include_lines(item.imports) + mlc::String("\n");
mlc::Array<mlc::String> type_fwds = decl::collect_decl_parts(item.decls, context, 0);
mlc::Array<mlc::String> type_defs = decl::collect_decl_parts(item.decls, context, 1);
mlc::Array<mlc::String> fn_protos = decl::collect_decl_parts(item.decls, context, 2);
mlc::Array<mlc::String> fn_defs = decl::collect_decl_parts(item.decls, context, 3);
mlc::String guard = base.upper() + mlc::String("_HPP");
mlc::String header = mlc::Array<mlc::String>{mlc::String("#ifndef ") + guard + mlc::String("\n"), mlc::String("#define ") + guard + mlc::String("\n"), mlc::String("\n"), std_includes, mlc::String("namespace ") + module_namespace + mlc::String(" {\n"), mlc::String("\n"), type_fwds.join(mlc::String("")), type_defs.join(mlc::String("")), fn_protos.join(mlc::String("")), mlc::String("\n"), mlc::String("} // namespace ") + module_namespace + mlc::String("\n"), mlc::String("\n"), mlc::String("#endif // ") + guard + mlc::String("\n")}.join(mlc::String(""));
mlc::String cli_wrapper = is_entry ? mlc::String("\n#undef main\n\nstatic void mlc_cli_set_args(int argc, char** argv) {\n  std::vector<mlc::String> arguments;\n  arguments.reserve(argc > 0 ? argc - 1 : 0);\n  for (int i = 1; i < argc; ++i) { arguments.emplace_back(argv[i]); }\n  mlc::io::set_args(std::move(arguments));\n}\n\nint main(int argc, char** argv) {\n  mlc_cli_set_args(argc, argv);\n  return ::") + module_namespace + mlc::String("::mlc_user_main(argc, argv);\n}\n") : mlc::String("");
mlc::String impl = mlc::Array<mlc::String>{mlc::String("#define main mlc_user_main\n"), mlc::String("#include \"") + base + mlc::String(".hpp\"\n"), mlc::String("\n"), mlc::String("namespace ") + module_namespace + mlc::String(" {\n"), mlc::String("\n"), fn_defs.join(mlc::String("")), mlc::String("\n"), mlc::String("} // namespace ") + module_namespace + mlc::String("\n"), cli_wrapper}.join(mlc::String(""));
return context::GenModuleOut{header, impl};
}

} // namespace module

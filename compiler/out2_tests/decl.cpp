#define main mlc_user_main
#include "decl.hpp"

namespace decl {

context::CodegenContext CodegenContext_for_type_body(context::CodegenContext self, mlc::String type_name) noexcept{
return context::make_body_context(self, {}, {}, {}, {}, type_name, {}, {});
}
mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
auto prefix = (cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds));
auto safe_name = context::context_resolve(context, name);
auto prototype_context = (((params.length() > 0) && (params[0]->name == mlc::String("self", 4))) ? std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [type_name] = tyNamed; return [&]() {
auto resolved_type = (((type_name == mlc::String("Self", 4)) || (type_name == mlc::String("self", 4))) ? context.self_type : type_name);
return CodegenContext_for_type_body(context, resolved_type);
}(); },
[&](const auto& __v) { return context; }
}, (*params[0]->typ)) : context);
if (((name == mlc::String("main", 4)) && (params.length() == 0))) {
return (prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list()));
} else {
return (prefix + expr::noexcept_function_prototype(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params_proto(prototype_context, params)));
}
}
context::CodegenContext compute_fn_body_context(mlc::String name, mlc::Array<std::shared_ptr<ast::Param>> params, context::CodegenContext context) noexcept{
auto fn_shared_params = param_analysis::collect_shared_params(params);
auto fn_shared_array_params = param_analysis::collect_shared_array_params(params);
auto fn_array_elem_types = param_analysis::collect_array_elem_types(params);
auto fn_shared_map_params = param_analysis::collect_shared_map_params(params);
auto fn_value_params = param_analysis::collect_value_params(params);
if (((params.length() > 0) && (params[0]->name == mlc::String("self", 4)))) {
return std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [self_type_name] = tyNamed; return [&]() {
auto resolved_type = (((self_type_name == mlc::String("Self", 4)) || (self_type_name == mlc::String("self", 4))) ? context.self_type : self_type_name);
return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, resolved_type, fn_value_params, {});
}(); },
[&](const auto& __v) { return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, {}); }
}, (*params[0]->typ));
} else {
return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, {});
}
}
mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept{
auto prefix = (cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds));
auto safe_name = context::context_resolve(context, name);
auto body_context = compute_fn_body_context(name, params, context);
auto prototype_context = (((params.length() > 0) && (params[0]->name == mlc::String("self", 4))) ? body_context : context);
if (((name == mlc::String("main", 4)) && (params.length() == 0))) {
auto preamble = expr::user_main_arguments_copy_into_runtime_statement();
return ((((prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, expr::main_program_parameter_list())) + preamble) + return_body::gen_fn_body(body, body_context)) + expr::block_close_newline());
} else {
return (((prefix + expr::noexcept_function_body_open(type_gen::sem_type_to_cpp(prototype_context, return_type), safe_name, decl_extend::gen_params_def(prototype_context, params))) + return_body::gen_fn_body(body, body_context)) + expr::block_close_newline());
}
}
mlc::String gen_decl(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [type_name, type_params, variants, derive_traits] = sDeclType; return (type_gen::gen_type_decl(context, type_name, type_params, variants) + type_gen::gen_derive_methods(context, type_name, variants, derive_traits)); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [name, type_params, methods] = sDeclTrait; return decl_extend::gen_trait_decl(context, name, type_params, methods); },
[&](const semantic_ir::SDeclFn& sDeclFn) { auto [name, type_params, type_bounds, params, return_type, body] = sDeclFn; return std::visit(overloaded{[&](const semantic_ir::SExprExtern& sExprExtern) { auto [__0, __1] = sExprExtern; return mlc::String("", 0); },
[&](const auto& __v) { return gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context); }
}, (*body)); },
[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [type_name, trait_name, methods] = sDeclExtend; return decl_extend::gen_decl_extend(type_name, trait_name, methods, context, [=](auto ctx, auto name) { return context::context_resolve(ctx, name); }, [=](auto decl, auto ctx) { return gen_decl(decl, ctx); }); },
[&](const semantic_ir::SDeclImport& sDeclImport) { auto [__0, __1] = sDeclImport; return mlc::String("", 0); },
[&](const semantic_ir::SDeclExported& sDeclExported) { auto [d] = sDeclExported; return gen_decl(d, context); }
}, (*decl));
}
mlc::String gen_proto(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [__0, __1, __2, __3] = sDeclType; return mlc::String("", 0); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [__0, __1, __2] = sDeclTrait; return mlc::String("", 0); },
[&](const semantic_ir::SDeclFn& sDeclFn) { auto [name, type_params, type_bounds, params, ret, body] = sDeclFn; return std::visit(overloaded{[&](const semantic_ir::SExprExtern& sExprExtern) { auto [__0, __1] = sExprExtern; return mlc::String("", 0); },
[&](const auto& __v) { return gen_fn_proto(name, type_params, type_bounds, params, ret, context); }
}, (*body)); },
[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [type_name, __1, methods] = sDeclExtend; return [&]() {
auto extend_context = CodegenContext_for_type_body(context, type_name);
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < methods.length())) {
parts.push_back(gen_proto(methods[index], extend_context));
index = (index + 1);
}
return parts.join(mlc::String("", 0));
}(); },
[&](const semantic_ir::SDeclImport& sDeclImport) { auto [__0, __1] = sDeclImport; return mlc::String("", 0); },
[&](const semantic_ir::SDeclExported& sDeclExported) { auto [d] = sDeclExported; return gen_proto(d, context); }
}, (*decl));
}
bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept{
auto found = false;
auto index = 0;
while ((index < decls.length())) {
auto inner = semantic_ir::sdecl_inner(decls[index]);
auto is_main = std::visit(overloaded{[&](const semantic_ir::SDeclFn& sDeclFn) { auto [name, __1, __2, __3, __4, __5] = sDeclFn; return (name == mlc::String("main", 4)); },
[&](const auto& __v) { return false; }
}, (*inner));
if (is_main) {
found = true;
} else {
/* unit */;
}
index = (index + 1);
}
return found;
}
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context, int phase) noexcept{
auto parts = mlc::Array<mlc::String>{};
if ((phase == 0)) {
auto index = 0;
[&]() {
while ((index < decls.length())) {
auto wrapper = std::visit(overloaded{[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [type_name, trait_name, methods] = sDeclExtend; return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, context); },
[&](const semantic_ir::SDeclExported& sDeclExported) { auto [d] = sDeclExported; return std::visit(overloaded{[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [type_name, trait_name, methods] = sDeclExtend; return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, context); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*semantic_ir::sdecl_inner(d))); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*decls[index]));
if ((wrapper.length() > 0)) {
parts.push_back(wrapper);
}
index = (index + 1);
}
}();
}
auto index = 0;
while ((index < decls.length())) {
auto segment = std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [type_name, type_params, variants, derive_traits] = sDeclType; return ((phase == 0) ? type_gen::gen_type_decl_fwd_only(context, type_name, type_params, variants) : ((phase == 1) ? (type_gen::gen_type_decl_body_only(context, type_name, type_params, variants) + type_gen::gen_derive_methods(context, type_name, variants, derive_traits)) : mlc::String("", 0))); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [name, type_params, methods] = sDeclTrait; return ((phase == 0) ? decl_extend::gen_trait_decl(context, name, type_params, methods) : mlc::String("", 0)); },
[&](const semantic_ir::SDeclFn& sDeclFn) { auto [__0, __1, __2, __3, __4, __5] = sDeclFn; return ((phase == 2) ? gen_proto(decls[index], context) : ((phase == 3) ? gen_decl(decls[index], context) : mlc::String("", 0))); },
[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [__0, __1, __2] = sDeclExtend; return ((phase == 2) ? gen_proto(decls[index], context) : ((phase == 3) ? gen_decl(decls[index], context) : mlc::String("", 0))); },
[&](const semantic_ir::SDeclImport& sDeclImport) { auto [__0, __1] = sDeclImport; return mlc::String("", 0); },
[&](const semantic_ir::SDeclExported& sDeclExported) { auto [d] = sDeclExported; return ((phase == 0) ? std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [type_name, type_params, variants, __3] = sDeclType; return type_gen::gen_type_decl_fwd_only(context, type_name, type_params, variants); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [name, type_params, methods] = sDeclTrait; return decl_extend::gen_trait_decl(context, name, type_params, methods); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*semantic_ir::sdecl_inner(d))) : ((phase == 1) ? std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [type_name, type_params, variants, derive_traits] = sDeclType; return (type_gen::gen_type_decl_body_only(context, type_name, type_params, variants) + type_gen::gen_derive_methods(context, type_name, variants, derive_traits)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*semantic_ir::sdecl_inner(d))) : ((phase == 2) ? gen_proto(d, context) : ((phase == 3) ? std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [__0, __1, __2, __3] = sDeclType; return mlc::String("", 0); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [__0, __1, __2] = sDeclTrait; return mlc::String("", 0); },
[&](const auto& __v) { return gen_decl(d, context); }
}, (*semantic_ir::sdecl_inner(d))) : mlc::String("", 0))))); }
}, (*decls[index]));
if ((segment.length() > 0)) {
parts.push_back(segment);
}
index = (index + 1);
}
return parts;
}

} // namespace decl

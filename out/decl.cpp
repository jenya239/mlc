#define main mlc_user_main
#include "decl.hpp"

namespace decl {

mlc::String gen_fn_proto(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, context::CodegenContext context) noexcept{
auto prefix = (cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds));
auto safe_name = CodegenContext_resolve(context, name);
auto prototype_context = [&]() -> context::CodegenContext {
if (((params.length() > 0) && (params[0]->name == mlc::String("self", 4)))) {
return std::visit(overloaded{[&](const ast::TyNamed& tyNamed) { auto [type_name] = tyNamed; return [&]() {
auto resolved_type = (((type_name == mlc::String("Self", 4)) || (type_name == mlc::String("self", 4))) ? (context.self_type) : (type_name));
return CodegenContext_for_type_body(context, resolved_type);
}(); },
[&](const auto& __v) { return context; }
}, (*params[0]->typ));
} else {
return context;
}
}();
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
auto resolved_type = (((self_type_name == mlc::String("Self", 4)) || (self_type_name == mlc::String("self", 4))) ? (context.self_type) : (self_type_name));
return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, resolved_type, fn_value_params, mlc::Array<mlc::String>{});
}(); },
[&](const auto& __v) { return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, mlc::Array<mlc::String>{}); }
}, (*params[0]->typ));
} else {
return context::make_body_context(context, fn_shared_params, fn_shared_array_params, fn_array_elem_types, fn_shared_map_params, context.self_type, fn_value_params, mlc::Array<mlc::String>{});
}
}
mlc::String gen_fn_decl(mlc::String name, mlc::Array<mlc::String> type_params, mlc::Array<mlc::Array<mlc::String>> type_bounds, mlc::Array<std::shared_ptr<ast::Param>> params, std::shared_ptr<registry::Type> return_type, std::shared_ptr<semantic_ir::SExpr> body, context::CodegenContext context) noexcept{
auto prefix = (cpp_naming::template_prefix(type_params) + type_gen::requires_clause(type_params, type_bounds));
auto safe_name = CodegenContext_resolve(context, name);
auto body_context = compute_fn_body_context(name, params, context);
auto prototype_context = [&]() -> context::CodegenContext {
if (((params.length() > 0) && (params[0]->name == mlc::String("self", 4)))) {
return body_context;
} else {
return context;
}
}();
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
[&](const semantic_ir::SDeclFn& sDeclFn) { auto [name, type_params, type_bounds, params, return_type, body, __6] = sDeclFn; return std::visit(overloaded{[&](const semantic_ir::SExprExtern& sExprExtern) { auto [__0, __1] = sExprExtern; return mlc::String("", 0); },
[&](const auto& __v) { return gen_fn_decl(name, type_params, type_bounds, params, return_type, body, context); }
}, (*body)); },
[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [type_name, trait_name, methods] = sDeclExtend; return decl_extend::gen_decl_extend(type_name, trait_name, methods, context, [=](context::CodegenContext ctx, mlc::String name) mutable { return CodegenContext_resolve(ctx, name); }, [=](std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext ctx) mutable { return gen_decl(decl, ctx); }); },
[&](const semantic_ir::SDeclImport& sDeclImport) { auto [__0, __1] = sDeclImport; return mlc::String("", 0); },
[&](const semantic_ir::SDeclExported& sDeclExported) { auto [d] = sDeclExported; return gen_decl(d, context); },
[&](const semantic_ir::SDeclAssocBind& sDeclAssocBind) { auto [__0, __1, __2] = sDeclAssocBind; return mlc::String("", 0); }
}, (*decl));
}
mlc::String gen_proto(std::shared_ptr<semantic_ir::SDecl> decl, context::CodegenContext context) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [__0, __1, __2, __3] = sDeclType; return mlc::String("", 0); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [__0, __1, __2] = sDeclTrait; return mlc::String("", 0); },
[&](const semantic_ir::SDeclFn& sDeclFn) { auto [name, type_params, type_bounds, params, ret, body, __6] = sDeclFn; return std::visit(overloaded{[&](const semantic_ir::SExprExtern& sExprExtern) { auto [__0, __1] = sExprExtern; return mlc::String("", 0); },
[&](const auto& __v) { return gen_fn_proto(name, type_params, type_bounds, params, ret, context); }
}, (*body)); },
[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [type_name, __1, methods] = sDeclExtend; return [&]() {
auto extend_context = CodegenContext_for_type_body(context, type_name);
return methods.map([=](std::shared_ptr<semantic_ir::SDecl> method) mutable { return gen_proto(method, extend_context); }).join(mlc::String("", 0));
}(); },
[&](const semantic_ir::SDeclImport& sDeclImport) { auto [__0, __1] = sDeclImport; return mlc::String("", 0); },
[&](const semantic_ir::SDeclExported& sDeclExported) { auto [d] = sDeclExported; return gen_proto(d, context); },
[&](const semantic_ir::SDeclAssocBind& sDeclAssocBind) { auto [__0, __1, __2] = sDeclAssocBind; return mlc::String("", 0); }
}, (*decl));
}
bool inner_declaration_is_main(std::shared_ptr<semantic_ir::SDecl> declaration) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SDeclFn& sDeclFn) { auto [name, __1, __2, __3, __4, __5, __6] = sDeclFn; return (name == mlc::String("main", 4)); },
[&](const auto& __v) { return false; }
}, (*semantic_ir::sdecl_inner(declaration)));
}
bool decls_have_main(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls) noexcept{
return decls.any([=](std::shared_ptr<semantic_ir::SDecl> declaration) mutable { return inner_declaration_is_main(declaration); });
}
mlc::String extend_wrapper_segment(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext codegen_context) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [type_name, trait_name, methods] = sDeclExtend; return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, codegen_context); },
[&](const semantic_ir::SDeclExported& sDeclExported) { auto [inner_declaration] = sDeclExported; return std::visit(overloaded{[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [type_name, trait_name, methods] = sDeclExtend; return decl_extend::gen_extend_wrapper_protos(type_name, trait_name, methods, codegen_context); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*semantic_ir::sdecl_inner(inner_declaration))); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*declaration));
}
mlc::String decl_segment(std::shared_ptr<semantic_ir::SDecl> declaration, context::CodegenContext codegen_context, int phase) noexcept{
return std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [type_name, type_params, variants, derive_traits] = sDeclType; return ((phase == 0) ? (type_gen::gen_type_decl_fwd_only(codegen_context, type_name, type_params, variants)) : (((phase == 1) ? ((type_gen::gen_type_decl_body_only(codegen_context, type_name, type_params, variants) + type_gen::gen_derive_methods(codegen_context, type_name, variants, derive_traits))) : (mlc::String("", 0))))); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [name, type_params, methods] = sDeclTrait; return ((phase == 0) ? (decl_extend::gen_trait_decl(codegen_context, name, type_params, methods)) : (mlc::String("", 0))); },
[&](const semantic_ir::SDeclFn& sDeclFn) { auto [__0, __1, __2, __3, __4, __5, __6] = sDeclFn; return ((phase == 2) ? (gen_proto(declaration, codegen_context)) : (((phase == 3) ? (gen_decl(declaration, codegen_context)) : (mlc::String("", 0))))); },
[&](const semantic_ir::SDeclExtend& sDeclExtend) { auto [__0, __1, __2] = sDeclExtend; return ((phase == 2) ? (gen_proto(declaration, codegen_context)) : (((phase == 3) ? (gen_decl(declaration, codegen_context)) : (mlc::String("", 0))))); },
[&](const semantic_ir::SDeclImport& sDeclImport) { auto [__0, __1] = sDeclImport; return mlc::String("", 0); },
[&](const semantic_ir::SDeclAssocBind& sDeclAssocBind) { auto [__0, __1, __2] = sDeclAssocBind; return mlc::String("", 0); },
[&](const semantic_ir::SDeclExported& sDeclExported) { auto [inner_declaration] = sDeclExported; return ((phase == 0) ? (std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [type_name, type_params, variants, __3] = sDeclType; return type_gen::gen_type_decl_fwd_only(codegen_context, type_name, type_params, variants); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [name, type_params, methods] = sDeclTrait; return decl_extend::gen_trait_decl(codegen_context, name, type_params, methods); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*semantic_ir::sdecl_inner(inner_declaration)))) : (((phase == 1) ? (std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [type_name, type_params, variants, derive_traits] = sDeclType; return (type_gen::gen_type_decl_body_only(codegen_context, type_name, type_params, variants) + type_gen::gen_derive_methods(codegen_context, type_name, variants, derive_traits)); },
[&](const auto& __v) { return mlc::String("", 0); }
}, (*semantic_ir::sdecl_inner(inner_declaration)))) : (((phase == 2) ? (gen_proto(inner_declaration, codegen_context)) : (((phase == 3) ? (std::visit(overloaded{[&](const semantic_ir::SDeclType& sDeclType) { auto [__0, __1, __2, __3] = sDeclType; return mlc::String("", 0); },
[&](const semantic_ir::SDeclTrait& sDeclTrait) { auto [__0, __1, __2] = sDeclTrait; return mlc::String("", 0); },
[&](const auto& __v) { return gen_decl(inner_declaration, codegen_context); }
}, (*semantic_ir::sdecl_inner(inner_declaration)))) : (mlc::String("", 0))))))))); }
}, (*declaration));
}
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<semantic_ir::SDecl>> decls, context::CodegenContext context, int phase) noexcept{
auto wrapper_segments = ((phase == 0) ? (decls.map([=](std::shared_ptr<semantic_ir::SDecl> declaration) mutable { return extend_wrapper_segment(declaration, context); }).filter([=](mlc::String segment) mutable { return (segment.length() > 0); })) : (mlc::Array<mlc::String>{}));
return wrapper_segments.concat(decls.map([=](std::shared_ptr<semantic_ir::SDecl> declaration) mutable { return decl_segment(declaration, context, phase); }).filter([=](mlc::String segment) mutable { return (segment.length() > 0); }));
}

} // namespace decl

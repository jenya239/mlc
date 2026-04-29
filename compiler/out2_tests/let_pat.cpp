#define main mlc_user_main
#include "let_pat.hpp"

namespace let_pat {

mlc::String gen_let_array_rest_slice(mlc::String temp, int from_index, mlc::String element_cpp) noexcept{
return ((((((((mlc::String("mlc::Array<", 11) + element_cpp) + mlc::String(">(", 2)) + temp) + mlc::String(".cbegin() + ", 12)) + mlc::to_string(from_index)) + mlc::String(", ", 2)) + temp) + mlc::String(".cend())", 8));
}
mlc::String gen_let_pattern_statement(std::shared_ptr<ast::Pat> pat, std::shared_ptr<semantic_ir::SExpr> value, std::shared_ptr<registry::Type> value_type, context::CodegenContext context, std::function<mlc::String(std::shared_ptr<semantic_ir::SExpr>, context::CodegenContext)> eval_fn) noexcept{
auto rhs = eval_fn(value, context);
return std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [n, __1] = patIdent; return expr::auto_binding_statement(cpp_naming::cpp_safe(n), rhs); },
[&](const ast::PatTuple& patTuple) { auto [subs, __1] = patTuple; return [&]() {
auto names = match_analysis::pat_bind_names(subs);
return ((((mlc::String("{\nauto __lt = ", 14) + rhs) + mlc::String(";\n", 2)) + expr::tuple_destructure_binding(names, mlc::String("__lt", 4))) + mlc::String("\n}\n", 3));
}(); },
[&](const ast::PatRecord& patRecord) { auto [__0, field_pats, __2] = patRecord; return [&]() {
auto b = ((mlc::String("{\nauto __lt = ", 14) + rhs) + mlc::String(";\n", 2));
auto i = 0;
while ((i < field_pats.length())) {
std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [fname, __1] = patIdent; return [&]() {
b = (b + expr::auto_binding_statement(cpp_naming::cpp_safe(fname), (mlc::String("__lt.", 5) + cpp_naming::cpp_safe(fname))));
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*field_pats[i]));
i = (i + 1);
}
return (b + mlc::String("}\n", 2));
}(); },
[&](const ast::PatArray& patArray) { auto [subs, rest, __2] = patArray; return [&]() {
auto et = std::visit(overloaded{[&](const registry::TArray& tArray) { auto [inn] = tArray; return type_gen::sem_type_to_cpp(context, inn); },
[&](const auto& __v) { return mlc::String("auto", 4); }
}, (*value_type));
auto b = ((mlc::String("{\nauto __lt = ", 14) + rhs) + mlc::String(";\n", 2));
auto j = 0;
while ((j < subs.length())) {
std::visit(overloaded{[&](const ast::PatIdent& patIdent) { auto [n, __1] = patIdent; return [&]() {
b = (((((b + mlc::String("auto ", 5)) + cpp_naming::cpp_safe(n)) + mlc::String(" = __lt[", 8)) + mlc::to_string(j)) + mlc::String("];\n", 3));
return /* unit */;
}(); },
[&](const auto& __v) { return /* unit */; }
}, (*subs[j]));
j = (j + 1);
}
if ((rest != mlc::String("", 0))) {
b = (((((b + mlc::String("auto ", 5)) + cpp_naming::cpp_safe(rest)) + mlc::String(" = ", 3)) + gen_let_array_rest_slice(mlc::String("__lt", 4), subs.length(), et)) + mlc::String(";\n", 2));
}
return (b + mlc::String("}\n", 2));
}(); },
[&](const ast::PatCtor& patCtor) { auto [ctor_name, sub_patterns, __2] = patCtor; return [&]() {
auto q = context::context_resolve(context, ctor_name);
auto bind = ((sub_patterns.length() == 0) ? mlc::String("", 0) : expr::tuple_destructure_binding(match_analysis::pat_bind_names(sub_patterns), mlc::String("__b", 3)));
auto is_generic = decl_index::list_contains(context.generic_variants, ctor_name);
auto targs = (is_generic ? mlc::String("<auto>", 6) : mlc::String("", 0));
return ((((((((((((((((mlc::String("{\n", 2) + mlc::String("auto __lt = ", 12)) + rhs) + mlc::String(";\n", 2)) + mlc::String("if (!std::holds_alternative<", 28)) + q) + targs) + mlc::String(">(__lt)) { std::abort(); }\n", 27)) + mlc::String("const ", 6)) + q) + targs) + mlc::String("& __b = std::get<", 17)) + q) + targs) + mlc::String(">(__lt);\n", 9)) + bind) + mlc::String("\n}\n", 3));
}(); },
[&](const auto& __v) { return expr::suffix_semicolon_newline(rhs); }
}, (*pat));
}

} // namespace let_pat

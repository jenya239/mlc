#include "check_mutations.hpp"

#include "ast.hpp"
#include "semantic_ir.hpp"
#include "registry.hpp"
#include "expr_visitor.hpp"
#include "names.hpp"
#include "diagnostic_codes.hpp"
#include "transform.hpp"
#include "transform_stmts.hpp"

namespace check_mutations {

using namespace ast;
using namespace semantic_ir;
using namespace registry;
using namespace expr_visitor;
using namespace names;
using namespace diagnostic_codes;
using namespace transform;
using namespace transform_stmts;
using namespace ast_tokens;

struct MutationsPass {mlc::Array<mlc::String> mutable_locals;int seed;};

bool scope_has(mlc::Array<mlc::String> scope, mlc::String sought_name) noexcept;

mlc::Array<mlc::String> mutable_scope_with_binding(mlc::Array<mlc::String> scope, mlc::String binding_name) noexcept;

mlc::Array<mlc::String> mutable_scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept;

mlc::Array<mlc::String> mutable_scope_after_let(mlc::Array<mlc::String> scope, mlc::String name, bool is_mut) noexcept;

mlc::Array<mlc::String> mutable_scope_after_pattern_let(mlc::Array<mlc::String> scope, std::shared_ptr<ast::Pattern> pattern, bool is_mut) noexcept;

check_mutations::MutationsPass mutations_pass_new(mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> mutation_check_empty() noexcept;

mlc::Array<ast::Diagnostic> ident_assignment_mutation_errors(mlc::String binding_name, mlc::Array<mlc::String> mutable_locals, ast::Span source_span) noexcept;

mlc::Array<ast::Diagnostic> assignment_target_mutation_errors(std::shared_ptr<semantic_ir::SemanticExpression> left_expression, mlc::Array<mlc::String> mutable_locals) noexcept;

check_mutations::MutationCheckResult mutation_check_ok(mlc::Array<mlc::String> mutable_locals) noexcept;

check_mutations::MutationCheckResult merge_mutation_check_results(check_mutations::MutationCheckResult first, check_mutations::MutationCheckResult second) noexcept;

check_mutations::MutationCheckResult dispatch_mutations_pass(check_mutations::MutationsPass mutations_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept;

check_mutations::MutationCheckResult check_mutation_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, check_mutations::MutationsPass mutations_pass) noexcept;

check_mutations::MutationCheckResult check_mutation_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, check_mutations::MutationsPass mutations_pass) noexcept;

check_mutations::MutationCheckResult check_mutation_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> mutable_locals) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_int(check_mutations::MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_str(check_mutations::MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_bool(check_mutations::MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_unit(check_mutations::MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_float(check_mutations::MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_i64(check_mutations::MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_u8(check_mutations::MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_usize(check_mutations::MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_char(check_mutations::MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_extern(check_mutations::MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_ident(check_mutations::MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_bin(check_mutations::MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_un(check_mutations::MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_call(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_method(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_field(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_index(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_if(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_block(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_while(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_for(check_mutations::MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_match(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_record(check_mutations::MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_record_update(check_mutations::MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_array(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_tuple(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_question(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_lambda(check_mutations::MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_with(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept;

check_mutations::MutationCheckResult MutationsPass_visit_unsupported(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept;

mlc::Array<ast::Diagnostic> check_mutation_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> mutable_locals) noexcept;

mlc::Array<ast::Diagnostic> check_fn_body_semantic_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept;

std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression_for_mutations(std::shared_ptr<ast::Expr> expression) noexcept;

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept;

bool scope_has(mlc::Array<mlc::String> scope, mlc::String sought_name) noexcept{return scope.any([sought_name](mlc::String scope_entry) mutable { return scope_entry == sought_name; });}

mlc::Array<mlc::String> mutable_scope_with_binding(mlc::Array<mlc::String> scope, mlc::String binding_name) noexcept{return scope.concat(mlc::Array<mlc::String>{binding_name});}

mlc::Array<mlc::String> mutable_scope_with_bindings(mlc::Array<mlc::String> scope, mlc::Array<mlc::String> binding_names) noexcept{return binding_names.fold(scope, [](mlc::Array<mlc::String> accumulated_scope, mlc::String binding_name) mutable { return accumulated_scope.concat(mlc::Array<mlc::String>{binding_name}); });}

mlc::Array<mlc::String> mutable_scope_after_let(mlc::Array<mlc::String> scope, mlc::String name, bool is_mut) noexcept{return is_mut ? mutable_scope_with_binding(scope, name) : scope;}

mlc::Array<mlc::String> mutable_scope_after_pattern_let(mlc::Array<mlc::String> scope, std::shared_ptr<ast::Pattern> pattern, bool is_mut) noexcept{return is_mut ? mutable_scope_with_bindings(scope, names::pattern_bindings(pattern)) : scope;}

check_mutations::MutationsPass mutations_pass_new(mlc::Array<mlc::String> mutable_locals) noexcept{return check_mutations::MutationsPass{mutable_locals, 0};}

mlc::Array<ast::Diagnostic> mutation_check_empty() noexcept{
mlc::Array<ast::Diagnostic> empty_diagnostics = {};
return empty_diagnostics;
}

mlc::Array<ast::Diagnostic> ident_assignment_mutation_errors(mlc::String binding_name, mlc::Array<mlc::String> mutable_locals, ast::Span source_span) noexcept{return !scope_has(mutable_locals, binding_name) ? mlc::Array<ast::Diagnostic>{ast::diagnostic_error_with_code(mlc::String("cannot assign to immutable binding: ") + binding_name, source_span, diagnostic_codes::diagnostic_code_e031())} : mutation_check_empty();}

mlc::Array<ast::Diagnostic> assignment_target_mutation_errors(std::shared_ptr<semantic_ir::SemanticExpression> left_expression, mlc::Array<mlc::String> mutable_locals) noexcept{return [&]() -> mlc::Array<ast::Diagnostic> { if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*left_expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*left_expression)._); auto [name, _w0, _w1] = _v_semanticexpressionident; return ident_assignment_mutation_errors(name, mutable_locals, semantic_ir::sexpr_span(left_expression)); } return mutation_check_empty(); }();}

check_mutations::MutationCheckResult mutation_check_ok(mlc::Array<mlc::String> mutable_locals) noexcept{return check_mutations::MutationCheckResult{mutation_check_empty(), mutable_locals};}

check_mutations::MutationCheckResult merge_mutation_check_results(check_mutations::MutationCheckResult first, check_mutations::MutationCheckResult second) noexcept{return check_mutations::MutationCheckResult{ast::diagnostics_append(first.diagnostics, second.diagnostics), second.mutable_locals};}

check_mutations::MutationCheckResult dispatch_mutations_pass(check_mutations::MutationsPass mutations_pass, std::shared_ptr<semantic_ir::SemanticExpression> expression) noexcept{return [&]() -> check_mutations::MutationCheckResult { if (std::holds_alternative<semantic_ir::SemanticExpressionInt>((*expression)._)) { auto _v_semanticexpressionint = std::get<semantic_ir::SemanticExpressionInt>((*expression)._); auto [integer_value, type_value, _w0] = _v_semanticexpressionint; return MutationsPass_visit_int(mutations_pass, integer_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionStr>((*expression)._)) { auto _v_semanticexpressionstr = std::get<semantic_ir::SemanticExpressionStr>((*expression)._); auto [string_value, type_value, _w0] = _v_semanticexpressionstr; return MutationsPass_visit_str(mutations_pass, string_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBool>((*expression)._)) { auto _v_semanticexpressionbool = std::get<semantic_ir::SemanticExpressionBool>((*expression)._); auto [boolean_value, type_value, _w0] = _v_semanticexpressionbool; return MutationsPass_visit_bool(mutations_pass, boolean_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUnit>((*expression)._)) { auto _v_semanticexpressionunit = std::get<semantic_ir::SemanticExpressionUnit>((*expression)._); auto [type_value, _w0] = _v_semanticexpressionunit; return MutationsPass_visit_unit(mutations_pass, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionFloat>((*expression)._)) { auto _v_semanticexpressionfloat = std::get<semantic_ir::SemanticExpressionFloat>((*expression)._); auto [float_value, type_value, _w0] = _v_semanticexpressionfloat; return MutationsPass_visit_float(mutations_pass, float_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionI64>((*expression)._)) { auto _v_semanticexpressioni64 = std::get<semantic_ir::SemanticExpressionI64>((*expression)._); auto [integer_value, type_value, _w0] = _v_semanticexpressioni64; return MutationsPass_visit_i64(mutations_pass, integer_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionU8>((*expression)._)) { auto _v_semanticexpressionu8 = std::get<semantic_ir::SemanticExpressionU8>((*expression)._); auto [byte_value, type_value, _w0] = _v_semanticexpressionu8; return MutationsPass_visit_u8(mutations_pass, byte_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUsize>((*expression)._)) { auto _v_semanticexpressionusize = std::get<semantic_ir::SemanticExpressionUsize>((*expression)._); auto [size_value, type_value, _w0] = _v_semanticexpressionusize; return MutationsPass_visit_usize(mutations_pass, size_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionChar>((*expression)._)) { auto _v_semanticexpressionchar = std::get<semantic_ir::SemanticExpressionChar>((*expression)._); auto [character_value, type_value, _w0] = _v_semanticexpressionchar; return MutationsPass_visit_char(mutations_pass, character_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionExtern>((*expression)._)) { auto _v_semanticexpressionextern = std::get<semantic_ir::SemanticExpressionExtern>((*expression)._); auto [type_value, _w0] = _v_semanticexpressionextern; return MutationsPass_visit_extern(mutations_pass, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIdent>((*expression)._)) { auto _v_semanticexpressionident = std::get<semantic_ir::SemanticExpressionIdent>((*expression)._); auto [name_value, type_value, _w0] = _v_semanticexpressionident; return MutationsPass_visit_ident(mutations_pass, name_value, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBin>((*expression)._)) { auto _v_semanticexpressionbin = std::get<semantic_ir::SemanticExpressionBin>((*expression)._); auto [operation, left_expression, right_expression, type_value, _w0] = _v_semanticexpressionbin; return MutationsPass_visit_bin(mutations_pass, operation, left_expression, right_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionUn>((*expression)._)) { auto _v_semanticexpressionun = std::get<semantic_ir::SemanticExpressionUn>((*expression)._); auto [operation, inner_expression, type_value, _w0] = _v_semanticexpressionun; return MutationsPass_visit_un(mutations_pass, operation, inner_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionCall>((*expression)._)) { auto _v_semanticexpressioncall = std::get<semantic_ir::SemanticExpressionCall>((*expression)._); auto [function_expression, arguments, mutability_flags, type_value, _w0] = _v_semanticexpressioncall; return MutationsPass_visit_call(mutations_pass, function_expression, arguments, mutability_flags, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionMethod>((*expression)._)) { auto _v_semanticexpressionmethod = std::get<semantic_ir::SemanticExpressionMethod>((*expression)._); auto [object, method_name, arguments, mutability_flags, type_value, _w0] = _v_semanticexpressionmethod; return MutationsPass_visit_method(mutations_pass, object, method_name, arguments, mutability_flags, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionField>((*expression)._)) { auto _v_semanticexpressionfield = std::get<semantic_ir::SemanticExpressionField>((*expression)._); auto [object, field_name, type_value, _w0] = _v_semanticexpressionfield; return MutationsPass_visit_field(mutations_pass, object, field_name, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIndex>((*expression)._)) { auto _v_semanticexpressionindex = std::get<semantic_ir::SemanticExpressionIndex>((*expression)._); auto [object, index_expression, type_value, _w0] = _v_semanticexpressionindex; return MutationsPass_visit_index(mutations_pass, object, index_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionIf>((*expression)._)) { auto _v_semanticexpressionif = std::get<semantic_ir::SemanticExpressionIf>((*expression)._); auto [condition, then_expression, else_expression, type_value, _w0] = _v_semanticexpressionif; return MutationsPass_visit_if(mutations_pass, condition, then_expression, else_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionBlock>((*expression)._)) { auto _v_semanticexpressionblock = std::get<semantic_ir::SemanticExpressionBlock>((*expression)._); auto [statements, result_expression, type_value, _w0] = _v_semanticexpressionblock; return MutationsPass_visit_block(mutations_pass, statements, result_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionWhile>((*expression)._)) { auto _v_semanticexpressionwhile = std::get<semantic_ir::SemanticExpressionWhile>((*expression)._); auto [condition, statements, type_value, _w0] = _v_semanticexpressionwhile; return MutationsPass_visit_while(mutations_pass, condition, statements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionFor>((*expression)._)) { auto _v_semanticexpressionfor = std::get<semantic_ir::SemanticExpressionFor>((*expression)._); auto [variable, iterator, statements, type_value, _w0] = _v_semanticexpressionfor; return MutationsPass_visit_for(mutations_pass, variable, iterator, statements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionMatch>((*expression)._)) { auto _v_semanticexpressionmatch = std::get<semantic_ir::SemanticExpressionMatch>((*expression)._); auto [subject, arms, type_value, _w0] = _v_semanticexpressionmatch; return MutationsPass_visit_match(mutations_pass, subject, arms, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecord>((*expression)._)) { auto _v_semanticexpressionrecord = std::get<semantic_ir::SemanticExpressionRecord>((*expression)._); auto [type_name, field_values, type_value, _w0] = _v_semanticexpressionrecord; return MutationsPass_visit_record(mutations_pass, type_name, field_values, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._)) { auto _v_semanticexpressionrecordupdate = std::get<semantic_ir::SemanticExpressionRecordUpdate>((*expression)._); auto [type_name, base_expression, overrides, type_value, _w0] = _v_semanticexpressionrecordupdate; return MutationsPass_visit_record_update(mutations_pass, type_name, base_expression, overrides, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionArray>((*expression)._)) { auto _v_semanticexpressionarray = std::get<semantic_ir::SemanticExpressionArray>((*expression)._); auto [elements, type_value, _w0] = _v_semanticexpressionarray; return MutationsPass_visit_array(mutations_pass, elements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionTuple>((*expression)._)) { auto _v_semanticexpressiontuple = std::get<semantic_ir::SemanticExpressionTuple>((*expression)._); auto [elements, type_value, _w0] = _v_semanticexpressiontuple; return MutationsPass_visit_tuple(mutations_pass, elements, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionQuestion>((*expression)._)) { auto _v_semanticexpressionquestion = std::get<semantic_ir::SemanticExpressionQuestion>((*expression)._); auto [inner_expression, type_value, _w0] = _v_semanticexpressionquestion; return MutationsPass_visit_question(mutations_pass, inner_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionLambda>((*expression)._)) { auto _v_semanticexpressionlambda = std::get<semantic_ir::SemanticExpressionLambda>((*expression)._); auto [parameter_names, body_expression, type_value, _w0] = _v_semanticexpressionlambda; return MutationsPass_visit_lambda(mutations_pass, parameter_names, body_expression, type_value); } if (std::holds_alternative<semantic_ir::SemanticExpressionWith>((*expression)._)) { auto _v_semanticexpressionwith = std::get<semantic_ir::SemanticExpressionWith>((*expression)._); auto [resource, binder, statements, type_value, _w0] = _v_semanticexpressionwith; return MutationsPass_visit_with(mutations_pass, resource, binder, statements, type_value); } return MutationsPass_visit_unsupported(mutations_pass, expression); }();}

check_mutations::MutationCheckResult check_mutation_semantic_field_values(mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, check_mutations::MutationsPass mutations_pass) noexcept{return field_values.fold(mutation_check_ok(mutations_pass.mutable_locals), [mutations_pass](check_mutations::MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticFieldVal> field_value_under_walk) mutable { return merge_mutation_check_results(accumulator, dispatch_mutations_pass(mutations_pass, field_value_under_walk->value)); });}

check_mutations::MutationCheckResult check_mutation_semantic_elements(mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, check_mutations::MutationsPass mutations_pass) noexcept{return elements.fold(mutation_check_ok(mutations_pass.mutable_locals), [mutations_pass](check_mutations::MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticExpression> element_under_walk) mutable { return merge_mutation_check_results(accumulator, dispatch_mutations_pass(mutations_pass, element_under_walk)); });}

check_mutations::MutationCheckResult check_mutation_semantic_statements(mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, mlc::Array<mlc::String> mutable_locals) noexcept{return statements.fold(check_mutations::MutationCheckResult{{}, mutable_locals}, [](check_mutations::MutationCheckResult accumulator, std::shared_ptr<semantic_ir::SemanticStatement> statement_under_walk) mutable { return std::visit(overloaded{
  [&](const SemanticStatementLet& semanticstatementlet) -> check_mutations::MutationCheckResult { auto [name, is_mut, value, _w0, _w1] = semanticstatementlet; return [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<mlc::String> scope_after = mutable_scope_after_let(accumulator.mutable_locals, name, is_mut);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, accumulator.mutable_locals)), scope_after};
 }(); },
  [&](const SemanticStatementLetPattern& semanticstatementletpattern) -> check_mutations::MutationCheckResult { auto [pattern, is_mut, value, _w0, _w1, _w2, _w3] = semanticstatementletpattern; return [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<mlc::String> scope_after_value = accumulator.mutable_locals;
  mlc::Array<mlc::String> scope_after_bindings = mutable_scope_after_pattern_let(scope_after_value, pattern, is_mut);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, scope_after_value)), scope_after_bindings};
 }(); },
  [&](const SemanticStatementLetConst& semanticstatementletconst) -> check_mutations::MutationCheckResult { auto [name, value, _w0, _w1] = semanticstatementletconst; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(value, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const SemanticStatementExpr& semanticstatementexpr) -> check_mutations::MutationCheckResult { auto [expression, _w0] = semanticstatementexpr; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const SemanticStatementReturn& semanticstatementreturn) -> check_mutations::MutationCheckResult { auto [expression, _w0] = semanticstatementreturn; return check_mutations::MutationCheckResult{ast::diagnostics_append(accumulator.diagnostics, check_mutation_semantic_expression(expression, accumulator.mutable_locals)), accumulator.mutable_locals}; },
  [&](const SemanticStatementBreak& semanticstatementbreak) -> check_mutations::MutationCheckResult { auto [_w0] = semanticstatementbreak; return accumulator; },
  [&](const SemanticStatementContinue& semanticstatementcontinue) -> check_mutations::MutationCheckResult { auto [_w0] = semanticstatementcontinue; return accumulator; }
}, (*statement_under_walk)._); });}

check_mutations::MutationCheckResult MutationsPass_visit_int(check_mutations::MutationsPass self, int _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_str(check_mutations::MutationsPass self, mlc::String _string_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_bool(check_mutations::MutationsPass self, bool _boolean_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_unit(check_mutations::MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_float(check_mutations::MutationsPass self, mlc::String _float_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_i64(check_mutations::MutationsPass self, mlc::String _integer_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_u8(check_mutations::MutationsPass self, mlc::String _byte_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_usize(check_mutations::MutationsPass self, mlc::String _size_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_char(check_mutations::MutationsPass self, mlc::String _character_value, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_extern(check_mutations::MutationsPass self, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_ident(check_mutations::MutationsPass self, mlc::String _name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return mutation_check_ok(self.mutable_locals);}

check_mutations::MutationCheckResult MutationsPass_visit_bin(check_mutations::MutationsPass self, mlc::String operation, std::shared_ptr<semantic_ir::SemanticExpression> left_expression, std::shared_ptr<semantic_ir::SemanticExpression> right_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult right_check = dispatch_mutations_pass(self, right_expression);
return operation == mlc::String("=") ? [&]() -> check_mutations::MutationCheckResult { 
  mlc::Array<ast::Diagnostic> assignment_errors = assignment_target_mutation_errors(left_expression, self.mutable_locals);
  return check_mutations::MutationCheckResult{ast::diagnostics_append(assignment_errors, right_check.diagnostics), self.mutable_locals};
 }() : merge_mutation_check_results(dispatch_mutations_pass(self, left_expression), right_check);
}

check_mutations::MutationCheckResult MutationsPass_visit_un(check_mutations::MutationsPass self, mlc::String _operation, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_mutations_pass(self, inner_expression);}

check_mutations::MutationCheckResult MutationsPass_visit_call(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> function_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, function_expression), check_mutation_semantic_elements(arguments, self));}

check_mutations::MutationCheckResult MutationsPass_visit_method(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _method_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> arguments, mlc::Array<int> _mutability_flags, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, object), check_mutation_semantic_elements(arguments, self));}

check_mutations::MutationCheckResult MutationsPass_visit_field(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, mlc::String _field_name, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_mutations_pass(self, object);}

check_mutations::MutationCheckResult MutationsPass_visit_index(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> object, std::shared_ptr<semantic_ir::SemanticExpression> index_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, object), dispatch_mutations_pass(self, index_expression));}

check_mutations::MutationCheckResult MutationsPass_visit_if(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, std::shared_ptr<semantic_ir::SemanticExpression> then_expression, std::shared_ptr<semantic_ir::SemanticExpression> else_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, condition), merge_mutation_check_results(dispatch_mutations_pass(self, then_expression), dispatch_mutations_pass(self, else_expression)));}

check_mutations::MutationCheckResult MutationsPass_visit_block(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<semantic_ir::SemanticExpression> result_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
check_mutations::MutationCheckResult result_check = dispatch_mutations_pass(mutations_pass_new(statements_check.mutable_locals), result_expression);
return check_mutations::MutationCheckResult{ast::diagnostics_append(statements_check.diagnostics, result_check.diagnostics), self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_while(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> condition, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return check_mutations::MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, condition).diagnostics, statements_check.diagnostics), self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_for(check_mutations::MutationsPass self, mlc::String _variable, std::shared_ptr<semantic_ir::SemanticExpression> iterator, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return check_mutations::MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, iterator).diagnostics, statements_check.diagnostics), self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_match(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> subject, mlc::Array<std::shared_ptr<semantic_ir::SemanticMatchArm>> arms, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult accumulator = dispatch_mutations_pass(self, subject);
int arm_index = 0;
while (arm_index < arms.size()){
{
std::shared_ptr<semantic_ir::SemanticMatchArm> arm_under_walk = arms[arm_index];
mlc::Array<mlc::String> arm_scope = mutable_scope_with_bindings(self.mutable_locals, names::pattern_bindings(arm_under_walk->pattern));
check_mutations::MutationsPass arm_pass = mutations_pass_new(arm_scope);
if (arm_under_walk->has_guard){
{
accumulator = merge_mutation_check_results(accumulator, dispatch_mutations_pass(arm_pass, arm_under_walk->when_condition));
}
}
accumulator = merge_mutation_check_results(accumulator, dispatch_mutations_pass(arm_pass, arm_under_walk->body));
arm_index = arm_index + 1;
}
}
return check_mutations::MutationCheckResult{accumulator.diagnostics, self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_record(check_mutations::MutationsPass self, mlc::String _type_name, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> field_values, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_mutation_semantic_field_values(field_values, self);}

check_mutations::MutationCheckResult MutationsPass_visit_record_update(check_mutations::MutationsPass self, mlc::String _type_name, std::shared_ptr<semantic_ir::SemanticExpression> base_expression, mlc::Array<std::shared_ptr<semantic_ir::SemanticFieldVal>> overrides, std::shared_ptr<registry::Type> _semantic_type) noexcept{return merge_mutation_check_results(dispatch_mutations_pass(self, base_expression), check_mutation_semantic_field_values(overrides, self));}

check_mutations::MutationCheckResult MutationsPass_visit_array(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_mutation_semantic_elements(elements, self);}

check_mutations::MutationCheckResult MutationsPass_visit_tuple(check_mutations::MutationsPass self, mlc::Array<std::shared_ptr<semantic_ir::SemanticExpression>> elements, std::shared_ptr<registry::Type> _semantic_type) noexcept{return check_mutation_semantic_elements(elements, self);}

check_mutations::MutationCheckResult MutationsPass_visit_question(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> inner_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{return dispatch_mutations_pass(self, inner_expression);}

check_mutations::MutationCheckResult MutationsPass_visit_lambda(check_mutations::MutationsPass self, mlc::Array<mlc::String> _parameter_names, std::shared_ptr<semantic_ir::SemanticExpression> body_expression, std::shared_ptr<registry::Type> _semantic_type) noexcept{
mlc::Array<mlc::String> empty_scope = {};
return dispatch_mutations_pass(mutations_pass_new(empty_scope), body_expression);
}

check_mutations::MutationCheckResult MutationsPass_visit_with(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> resource, mlc::String _binder, mlc::Array<std::shared_ptr<semantic_ir::SemanticStatement>> statements, std::shared_ptr<registry::Type> _semantic_type) noexcept{
check_mutations::MutationCheckResult statements_check = check_mutation_semantic_statements(statements, self.mutable_locals);
return check_mutations::MutationCheckResult{ast::diagnostics_append(dispatch_mutations_pass(self, resource).diagnostics, statements_check.diagnostics), self.mutable_locals};
}

check_mutations::MutationCheckResult MutationsPass_visit_unsupported(check_mutations::MutationsPass self, std::shared_ptr<semantic_ir::SemanticExpression> _expression) noexcept{return mutation_check_ok(self.mutable_locals);}

mlc::Array<ast::Diagnostic> check_mutation_semantic_expression(std::shared_ptr<semantic_ir::SemanticExpression> expression, mlc::Array<mlc::String> mutable_locals) noexcept{
check_mutations::MutationCheckResult check_result = dispatch_mutations_pass(mutations_pass_new(mutable_locals), expression);
return check_result.diagnostics;
}

mlc::Array<ast::Diagnostic> check_fn_body_semantic_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<semantic_ir::SemanticExpression> body) noexcept{
mlc::Array<mlc::String> mutable_locals = parameters.fold([&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> empty_scope = {};
  return empty_scope;
 }(), [](mlc::Array<mlc::String> parameter_scope, std::shared_ptr<ast::Param> parameter_under_check) mutable { return mutable_scope_after_let(parameter_scope, parameter_under_check->name, parameter_under_check->is_mut); });
return check_mutation_semantic_expression(body, mutable_locals);
}

std::shared_ptr<semantic_ir::SemanticExpression> transform_parser_expression_for_mutations(std::shared_ptr<ast::Expr> expression) noexcept{return transform::transform_expr(expression, transform::empty_transform_context(), [](mlc::Array<std::shared_ptr<ast::Stmt>> statements, transform::TransformContext nested_transform_context) mutable { return transform_stmts::transform_stmts(statements, nested_transform_context); });}

mlc::Array<ast::Diagnostic> check_fn_body_mutations(mlc::Array<std::shared_ptr<ast::Param>> parameters, std::shared_ptr<ast::Expr> body) noexcept{return check_fn_body_semantic_mutations(parameters, transform_parser_expression_for_mutations(body));}

} // namespace check_mutations

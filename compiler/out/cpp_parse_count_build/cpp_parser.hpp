#ifndef CPP_PARSER_HPP
#define CPP_PARSER_HPP

#include "mlc.hpp"
#include <variant>

#include "cpp_decls.hpp"
#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"
#include "cpp_predicates.hpp"

namespace cpp_parser {

struct CppStatement;
struct CppExpression;

struct CppParseResult {cpp_ast::CppProgram program;mlc::Array<mlc::String> errors;};

cpp_parser::CppParseResult cpp_parse(mlc::Array<cpp_tokens::CppToken> tokens) noexcept;

} // namespace cpp_parser

#endif // CPP_PARSER_HPP

#define main mlc_user_main
#include "cpp_decls.hpp"

#include "cpp_types.hpp"
#include "cpp_ast.hpp"
#include "cpp_tokens.hpp"
#include "cpp_predicates.hpp"

namespace cpp_decls {

using namespace cpp_types;
using namespace cpp_ast;
using namespace cpp_tokens;
using namespace cpp_predicates;

CppDeclarationParseResult cpp_decl_empty_result(int position) noexcept{
  return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude{true, mlc::String("__skip__", 8)}), position, mlc::String("", 0)};
}
int cpp_decl_skip_token(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  if (cpp_predicates::cpp_at_end(tokens, position))   {
    return position;
  } else   {
    return mlc::arith::checked_add(position, 1);
  }
}
int cpp_decl_skip_to_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto scan_position = position;
  while (((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))))   {
    (scan_position = mlc::arith::checked_add(scan_position, 1));
  }
  if (cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))   {
    return mlc::arith::checked_add(scan_position, 1);
  } else   {
    return scan_position;
  }
}
int cpp_decl_after_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  if (cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, position)))   {
    return mlc::arith::checked_add(position, 1);
  } else   {
    return position;
  }
}
bool cpp_decl_has_attribute_start(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  return (cpp_predicates::cpp_is_left_bracket(cpp_predicates::cpp_token_kind_at(tokens, position)) && cpp_predicates::cpp_is_left_bracket(cpp_predicates::cpp_token_kind_at(tokens, mlc::arith::checked_add(position, 1))));
}
int cpp_decl_skip_one_attribute(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  if ((!cpp_decl_has_attribute_start(tokens, position)))   {
    return position;
  } else   {
    auto scan_position = mlc::arith::checked_add(position, 2);
    auto found_end = false;
    while (((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!found_end)))     {
      if ((cpp_predicates::cpp_is_right_bracket(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) && cpp_predicates::cpp_is_right_bracket(cpp_predicates::cpp_token_kind_at(tokens, mlc::arith::checked_add(scan_position, 1)))))       {
        (scan_position = mlc::arith::checked_add(scan_position, 2));
        (found_end = true);
      } else       {
        (scan_position = mlc::arith::checked_add(scan_position, 1));
      }
    }
    return scan_position;
  }
}
int cpp_decl_skip_attributes(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto scan_position = position;
  auto next_position = cpp_decl_skip_one_attribute(tokens, scan_position);
  while ((next_position != scan_position))   {
    (scan_position = next_position);
    (next_position = cpp_decl_skip_one_attribute(tokens, scan_position));
  }
  return scan_position;
}
int cpp_decl_template_depth_after_token(int depth, cpp_tokens::CppTokenKind kind) noexcept{
  if (cpp_predicates::cpp_is_right_angle(kind))   {
    return mlc::arith::checked_sub(depth, 1);
  } else if (cpp_predicates::cpp_is_angle(kind))   {
    return mlc::arith::checked_add(depth, 1);
  } else   {
    return depth;
  }
}
int cpp_decl_template_skip_brackets(mlc::Array<cpp_tokens::CppToken> tokens, int scan_position, int depth) noexcept{
  if (cpp_predicates::cpp_at_end(tokens, scan_position))   {
    return scan_position;
  } else if ((depth <= 0))   {
    return scan_position;
  } else   {
    auto kind = cpp_predicates::cpp_token_kind_at(tokens, scan_position);
    auto next_depth = cpp_decl_template_depth_after_token(depth, kind);
    if ((next_depth <= 0))     {
      return scan_position;
    } else     {
      return cpp_decl_template_skip_brackets(tokens, mlc::arith::checked_add(scan_position, 1), next_depth);
    }
  }
}
mlc::String cpp_decl_template_parameters_between(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto scan_position = start;
  while ((scan_position < end_position))   {
    auto part = cpp_predicates::cpp_type_part_string(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
    if ((part != mlc::String("", 0)))     {
      parts.push_back(part);
    }
    (scan_position = mlc::arith::checked_add(scan_position, 1));
  }
  return parts.join(mlc::String(" ", 1));
}
CppTemplateHeaderParse cpp_decl_parse_template_header(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto after_template = mlc::arith::checked_add(position, 1);
  if ((!cpp_predicates::cpp_is_angle(cpp_predicates::cpp_token_kind_at(tokens, after_template))))   {
    return CppTemplateHeaderParse{mlc::String("", 0), after_template};
  } else   {
    auto parameter_start = mlc::arith::checked_add(after_template, 1);
    auto parameter_end = cpp_decl_template_skip_brackets(tokens, parameter_start, 1);
    auto scan_position = parameter_end;
    if (cpp_predicates::cpp_is_right_angle(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    }
    return CppTemplateHeaderParse{cpp_decl_template_parameters_between(tokens, parameter_start, parameter_end), scan_position};
  }
}
CppClassMembersParse cpp_decl_parse_class_members(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto members = mlc::Array<cpp_ast::CppClassMember>{};
  auto scan_position = position;
  while (((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))))   {
    (scan_position = cpp_decl_skip_attributes(tokens, scan_position));
    if (cpp_predicates::cpp_is_public_keyword(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      members.push_back(cpp_ast::CppClassMemberAccess{cpp_ast::CppPublic{}});
      (scan_position = mlc::arith::checked_add(scan_position, 1));
      if (cpp_predicates::cpp_is_colon(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))       {
        (scan_position = mlc::arith::checked_add(scan_position, 1));
      }
    } else if (cpp_predicates::cpp_is_protected_keyword(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      members.push_back(cpp_ast::CppClassMemberAccess{cpp_ast::CppProtected{}});
      (scan_position = mlc::arith::checked_add(scan_position, 1));
      if (cpp_predicates::cpp_is_colon(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))       {
        (scan_position = mlc::arith::checked_add(scan_position, 1));
      }
    } else if (cpp_predicates::cpp_is_private_keyword(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      members.push_back(cpp_ast::CppClassMemberAccess{cpp_ast::CppPrivate{}});
      (scan_position = mlc::arith::checked_add(scan_position, 1));
      if (cpp_predicates::cpp_is_colon(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))       {
        (scan_position = mlc::arith::checked_add(scan_position, 1));
      }
    } else     {
      auto type_parsed = cpp_types::parse_cpp_type(tokens, scan_position);
      auto field_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, type_parsed.position));
      if ((field_name == mlc::String("", 0)))       {
        (scan_position = cpp_decl_skip_token(tokens, scan_position));
      } else       {
        members.push_back(cpp_ast::CppClassMemberField{type_parsed.type_node, field_name, mlc::String("", 0)});
        (scan_position = cpp_decl_after_semicolon(tokens, mlc::arith::checked_add(type_parsed.position, 1)));
      }
    }
  }
  return CppClassMembersParse{members, scan_position};
}
int cpp_decl_parse_include_skip(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto scan_position = position;
  if (cpp_predicates::cpp_is_hash(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))   {
    (scan_position = mlc::arith::checked_add(scan_position, 1));
  }
  if ((cpp_predicates::cpp_is_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) && (cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) == mlc::String("include", 7))))   {
    (scan_position = mlc::arith::checked_add(scan_position, 1));
  }
  return scan_position;
}
CppDeclarationParseResult parse_cpp_include_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto after_skip = cpp_decl_parse_include_skip(tokens, position);
  auto kind = cpp_predicates::cpp_token_kind_at(tokens, after_skip);
  if (cpp_predicates::cpp_is_angle(kind))   {
    auto name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, mlc::arith::checked_add(after_skip, 1)));
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude{true, name}), mlc::arith::checked_add(after_skip, 3), mlc::String("", 0)};
  } else if (cpp_predicates::cpp_is_string(kind))   {
    auto path = cpp_predicates::cpp_get_string(kind);
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppInclude{false, path}), mlc::arith::checked_add(after_skip, 1), mlc::String("", 0)};
  } else   {
    return cpp_decl_empty_result(after_skip);
  }
}
CppDeclarationParseResult parse_cpp_using_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto after_using = mlc::arith::checked_add(position, 1);
  auto alias = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_using));
  auto after_alias = mlc::arith::checked_add(after_using, 1);
  auto after_equals = (cpp_predicates::cpp_is_equals(cpp_predicates::cpp_token_kind_at(tokens, after_alias)) ? (mlc::arith::checked_add(after_alias, 1)) : (after_alias));
  auto type_parsed = cpp_types::parse_cpp_type_to_string(tokens, after_equals);
  auto after_type = type_parsed.position;
  return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppUsing{alias, type_parsed.type_string}), cpp_decl_after_semicolon(tokens, after_type), mlc::String("", 0)};
}
CppRecordFieldsParse cpp_decl_parse_record_fields(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto fields = mlc::Array<std::shared_ptr<cpp_ast::CppField>>{};
  auto scan_position = position;
  while (((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))))   {
    auto type_parsed = cpp_types::parse_cpp_type(tokens, scan_position);
    auto field_type = print::print_cpp_type(type_parsed.type_node);
    (scan_position = type_parsed.position);
    auto field_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
    (scan_position = mlc::arith::checked_add(scan_position, 1));
    (scan_position = cpp_decl_after_semicolon(tokens, scan_position));
    fields.push_back(std::make_shared<cpp_ast::CppField>(cpp_ast::CppField{field_type, field_name}));
  }
  return CppRecordFieldsParse{fields, scan_position};
}
CppDeclarationParseResult cpp_decl_parse_record_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position, mlc::String kind_word) noexcept{
  auto is_struct = (kind_word == mlc::String("struct", 6));
  auto after_keyword = mlc::arith::checked_add(position, 1);
  auto name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_keyword));
  auto after_name = mlc::arith::checked_add(after_keyword, 1);
  if (cpp_predicates::cpp_is_left_brace(cpp_predicates::cpp_token_kind_at(tokens, after_name)))   {
    auto members_parsed = cpp_decl_parse_class_members(tokens, mlc::arith::checked_add(after_name, 1));
    auto scan_position = members_parsed.position;
    if (cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    }
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppClassDeclaration{cpp_ast::CppClassDefinition{is_struct, mlc::String("", 0), name, {}, members_parsed.members}}), cpp_decl_after_semicolon(tokens, scan_position), mlc::String("", 0)};
  } else if (cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, after_name)))   {
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppForwardDecl{kind_word, name}), mlc::arith::checked_add(after_name, 1), mlc::String("", 0)};
  } else   {
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppStruct{mlc::String("", 0), name, {}, true}), after_name, mlc::String("", 0)};
  }
}
CppDeclarationParseResult parse_cpp_struct_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  return cpp_decl_parse_record_decl(tokens, position, mlc::String("struct", 6));
}
CppDeclarationParseResult parse_cpp_class_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  return cpp_decl_parse_record_decl(tokens, position, mlc::String("class", 5));
}
CppParameterListParse cpp_decl_parse_parameter_list(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto parameters = mlc::Array<mlc::String>{};
  auto scan_position = position;
  if ((!cpp_predicates::cpp_is_left_paren(cpp_predicates::cpp_token_kind_at(tokens, scan_position))))   {
    return CppParameterListParse{parameters, scan_position};
  } else   {
    (scan_position = mlc::arith::checked_add(scan_position, 1));
    while (((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!cpp_predicates::cpp_is_right_paren(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))))     {
      auto type_parsed = cpp_types::parse_cpp_type_to_string(tokens, scan_position);
      (scan_position = type_parsed.position);
      auto parameter_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
      if ((parameter_name != mlc::String("", 0)))       {
        parameters.push_back(((type_parsed.type_string + mlc::String(" ", 1)) + parameter_name));
        (scan_position = mlc::arith::checked_add(scan_position, 1));
      } else       {
        parameters.push_back(type_parsed.type_string);
      }
      if (cpp_predicates::cpp_is_comma(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))       {
        (scan_position = mlc::arith::checked_add(scan_position, 1));
      }
    }
    if (cpp_predicates::cpp_is_right_paren(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    }
    return CppParameterListParse{parameters, scan_position};
  }
}
CppDeclarationParseResult parse_cpp_fn_proto_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto type_parsed = cpp_types::parse_cpp_type_to_string(tokens, position);
  auto function_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, type_parsed.position));
  if ((function_name == mlc::String("", 0)))   {
    return cpp_decl_empty_result(type_parsed.position);
  } else   {
    auto parameters_parsed = cpp_decl_parse_parameter_list(tokens, mlc::arith::checked_add(type_parsed.position, 1));
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppFnProto{mlc::String("", 0), type_parsed.type_string, function_name, parameters_parsed.parameters}), cpp_decl_after_semicolon(tokens, parameters_parsed.position), mlc::String("", 0)};
  }
}
CppEnumArmsParse cpp_decl_parse_enum_arms(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto arms = mlc::Array<std::shared_ptr<cpp_ast::CppVariantArm>>{};
  auto scan_position = position;
  while (((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))))   {
    auto arm_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
    if ((arm_name != mlc::String("", 0)))     {
      arms.push_back(std::make_shared<cpp_ast::CppVariantArm>(cpp_ast::CppVariantArm{arm_name, {}}));
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    } else     {
      (scan_position = cpp_decl_skip_token(tokens, scan_position));
    }
    if (cpp_predicates::cpp_is_equals(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
      [&]() {
while ((((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!cpp_predicates::cpp_is_comma(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))) && (!cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position))))) {
(scan_position = mlc::arith::checked_add(scan_position, 1));
}
}();
    }
    if (cpp_predicates::cpp_is_comma(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    }
  }
  return CppEnumArmsParse{arms, scan_position};
}
CppDeclarationParseResult parse_cpp_enum_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto after_enum = mlc::arith::checked_add(position, 1);
  auto enum_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_enum));
  auto after_name = mlc::arith::checked_add(after_enum, 1);
  if (cpp_predicates::cpp_is_left_brace(cpp_predicates::cpp_token_kind_at(tokens, after_name)))   {
    auto arms_parsed = cpp_decl_parse_enum_arms(tokens, mlc::arith::checked_add(after_name, 1));
    auto scan_position = arms_parsed.position;
    if (cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    }
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppVariant{mlc::String("", 0), enum_name, arms_parsed.arms}), cpp_decl_after_semicolon(tokens, scan_position), mlc::String("", 0)};
  } else if (cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, after_name)))   {
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppForwardDecl{mlc::String("enum", 4), enum_name}), mlc::arith::checked_add(after_name, 1), mlc::String("", 0)};
  } else   {
    return cpp_decl_empty_result(after_name);
  }
}
int cpp_decl_next_scan_after_parse(mlc::Array<cpp_tokens::CppToken> tokens, int scan_position, CppDeclarationParseResult parsed) noexcept{
  if ((parsed.position == scan_position))   {
    return cpp_decl_skip_token(tokens, scan_position);
  } else   {
    return parsed.position;
  }
}
CppDeclarationsBlockParse parse_cpp_declarations_until_right_brace(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto declarations = mlc::Array<std::shared_ptr<cpp_ast::CppDeclaration>>{};
  auto errors = mlc::Array<mlc::String>{};
  auto scan_position = position;
  while (((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))))   {
    (scan_position = cpp_decl_skip_attributes(tokens, scan_position));
    auto parsed = parse_cpp_declaration(tokens, scan_position);
    if ((parsed.error_message != mlc::String("", 0)))     {
      errors.push_back(parsed.error_message);
    }
    if ((!cpp_decl_is_skip(parsed.declaration)))     {
      declarations.push_back(parsed.declaration);
    }
    (scan_position = cpp_decl_next_scan_after_parse(tokens, scan_position, parsed));
  }
  return CppDeclarationsBlockParse{declarations, scan_position, errors};
}
CppDeclarationParseResult parse_cpp_namespace_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto after_namespace = mlc::arith::checked_add(position, 1);
  auto namespace_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, after_namespace));
  auto after_name = mlc::arith::checked_add(after_namespace, 1);
  if (cpp_predicates::cpp_is_left_brace(cpp_predicates::cpp_token_kind_at(tokens, after_name)))   {
    auto inner = parse_cpp_declarations_until_right_brace(tokens, mlc::arith::checked_add(after_name, 1));
    auto scan_position = inner.position;
    if (cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    }
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppNamespace{namespace_name, inner.declarations}), scan_position, inner.errors.join(mlc::String("; ", 2))};
  } else   {
    return cpp_decl_empty_result(after_name);
  }
}
bool cpp_decl_is_skip(std::shared_ptr<cpp_ast::CppDeclaration> declaration) noexcept{
  return [&]() -> bool {
auto __match_subject = declaration;
if (std::holds_alternative<cpp_ast::CppInclude>((*__match_subject))) {
const cpp_ast::CppInclude& cppInclude = std::get<cpp_ast::CppInclude>((*__match_subject));
auto [is_angle, path] = cppInclude; return (is_angle && (path == mlc::String("__skip__", 8)));
}
return false;
std::abort();
}();
}
bool cpp_decl_can_start_fn_proto(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto type_parsed = cpp_types::parse_cpp_type_to_string(tokens, position);
  auto name_position = type_parsed.position;
  auto function_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, name_position));
  return ((function_name != mlc::String("", 0)) && cpp_predicates::cpp_is_left_paren(cpp_predicates::cpp_token_kind_at(tokens, mlc::arith::checked_add(name_position, 1))));
}
bool cpp_decl_can_start_variable_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto type_parsed = cpp_types::parse_cpp_type_to_string(tokens, position);
  auto name_position = type_parsed.position;
  auto variable_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, name_position));
  return ((variable_name != mlc::String("", 0)) && cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, mlc::arith::checked_add(name_position, 1))));
}
CppDeclarationParseResult parse_cpp_variable_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto type_parsed = cpp_types::parse_cpp_type_to_string(tokens, position);
  auto variable_name = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, type_parsed.position));
  auto after_name = mlc::arith::checked_add(type_parsed.position, 1);
  return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppDeclarationFragment{(((type_parsed.type_string + mlc::String(" ", 1)) + variable_name) + mlc::String(";", 1))}), cpp_decl_after_semicolon(tokens, after_name), mlc::String("", 0)};
}
int cpp_decl_last_identifier_before_semicolon(mlc::Array<cpp_tokens::CppToken> tokens, int start) noexcept{
  auto scan_position = start;
  auto last_identifier_position = start;
  while (((!cpp_predicates::cpp_at_end(tokens, scan_position)) && (!cpp_predicates::cpp_is_semicolon(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))))   {
    if ((cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, scan_position)) != mlc::String("", 0)))     {
      (last_identifier_position = scan_position);
    }
    (scan_position = mlc::arith::checked_add(scan_position, 1));
  }
  return last_identifier_position;
}
std::shared_ptr<cpp_ast::CppType> cpp_decl_type_node_until(mlc::Array<cpp_tokens::CppToken> tokens, int start, int end_position) noexcept{
  auto parts = mlc::Array<mlc::String>{};
  auto scan_position = start;
  while ((scan_position < end_position))   {
    auto part = cpp_predicates::cpp_type_part_string(cpp_predicates::cpp_token_kind_at(tokens, scan_position));
    if ((part != mlc::String("", 0)))     {
      parts.push_back(part);
    }
    (scan_position = mlc::arith::checked_add(scan_position, 1));
  }
  return std::make_shared<cpp_ast::CppType>(cpp_ast::CppTypeName{parts.join(mlc::String(" ", 1))});
}
CppDeclarationParseResult parse_cpp_typedef_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto after_typedef = mlc::arith::checked_add(position, 1);
  auto alias_position = cpp_decl_last_identifier_before_semicolon(tokens, after_typedef);
  auto alias = cpp_predicates::cpp_get_identifier(cpp_predicates::cpp_token_kind_at(tokens, alias_position));
  return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppTypedefDeclaration{alias, cpp_decl_type_node_until(tokens, after_typedef, alias_position)}), cpp_decl_after_semicolon(tokens, mlc::arith::checked_add(alias_position, 1)), mlc::String("", 0)};
}
CppDeclarationParseResult parse_cpp_template_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto header = cpp_decl_parse_template_header(tokens, position);
  auto inner = parse_cpp_declaration(tokens, cpp_decl_skip_attributes(tokens, header.position));
  return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppTemplateDeclaration{header.parameters, inner.declaration}), inner.position, inner.error_message};
}
CppDeclarationParseResult parse_cpp_extern_block_decl(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto after_extern = mlc::arith::checked_add(position, 1);
  auto linkage = (cpp_predicates::cpp_is_string(cpp_predicates::cpp_token_kind_at(tokens, after_extern)) ? (((mlc::String("\"", 1) + cpp_predicates::cpp_get_string(cpp_predicates::cpp_token_kind_at(tokens, after_extern))) + mlc::String("\"", 1))) : (mlc::String("", 0)));
  auto after_linkage = ((linkage != mlc::String("", 0)) ? (mlc::arith::checked_add(after_extern, 1)) : (after_extern));
  if (cpp_predicates::cpp_is_left_brace(cpp_predicates::cpp_token_kind_at(tokens, after_linkage)))   {
    auto inner = parse_cpp_declarations_until_right_brace(tokens, mlc::arith::checked_add(after_linkage, 1));
    auto scan_position = inner.position;
    if (cpp_predicates::cpp_is_right_brace(cpp_predicates::cpp_token_kind_at(tokens, scan_position)))     {
      (scan_position = mlc::arith::checked_add(scan_position, 1));
    }
    return CppDeclarationParseResult{std::make_shared<cpp_ast::CppDeclaration>(cpp_ast::CppExternBlock{linkage, inner.declarations}), scan_position, inner.errors.join(mlc::String("; ", 2))};
  } else   {
    return cpp_decl_empty_result(after_linkage);
  }
}
CppDeclarationParseResult parse_cpp_declaration(mlc::Array<cpp_tokens::CppToken> tokens, int position) noexcept{
  auto start_position = cpp_decl_skip_attributes(tokens, position);
  auto kind = cpp_predicates::cpp_token_kind_at(tokens, start_position);
  if (cpp_predicates::cpp_is_hash(kind))   {
    return parse_cpp_include_decl(tokens, start_position);
  } else if (cpp_predicates::cpp_is_template_keyword(kind))   {
    return parse_cpp_template_decl(tokens, start_position);
  } else if (cpp_predicates::cpp_is_extern_keyword(kind))   {
    return parse_cpp_extern_block_decl(tokens, start_position);
  } else if (cpp_predicates::cpp_is_using_keyword(kind))   {
    return parse_cpp_using_decl(tokens, start_position);
  } else if (cpp_predicates::cpp_is_typedef_keyword(kind))   {
    return parse_cpp_typedef_decl(tokens, start_position);
  } else if (cpp_predicates::cpp_is_struct_keyword(kind))   {
    return parse_cpp_struct_decl(tokens, start_position);
  } else if (cpp_predicates::cpp_is_class_keyword(kind))   {
    return parse_cpp_class_decl(tokens, start_position);
  } else if (cpp_predicates::cpp_is_enum_keyword(kind))   {
    return parse_cpp_enum_decl(tokens, start_position);
  } else if (cpp_predicates::cpp_is_namespace_keyword(kind))   {
    return parse_cpp_namespace_decl(tokens, start_position);
  } else if (cpp_decl_can_start_fn_proto(tokens, start_position))   {
    return parse_cpp_fn_proto_decl(tokens, start_position);
  } else if (cpp_decl_can_start_variable_decl(tokens, start_position))   {
    return parse_cpp_variable_decl(tokens, start_position);
  } else   {
    auto recovered_position = cpp_decl_skip_to_semicolon(tokens, start_position);
    return CppDeclarationParseResult{cpp_decl_empty_result(start_position).declaration, recovered_position, mlc::String("unrecognized declaration", 24)};
  }
}

} // namespace cpp_decls

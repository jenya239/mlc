# frozen_string_literal: true

# rubocop:disable Lint/MissingSuper, Metrics/ParameterLists

module CppAst
  module Nodes
    # ExpressionStatement: `foo;`
    class ExpressionStatement < Statement
      attr_accessor :expression

      def initialize(expression:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @expression = expression
      end

      def to_source
        "#{leading_trivia}#{expression.to_source};"
      end
    end

    # ReturnStatement: `return 42;`
    class ReturnStatement < Statement
      attr_accessor :expression, :keyword_suffix

      def initialize(leading_trivia: "", expression: nil, keyword_suffix: " ")
        super(leading_trivia: leading_trivia)
        @expression = expression
        @keyword_suffix = keyword_suffix
      end

      def to_source
        if expression
          "#{leading_trivia}return#{keyword_suffix}#{expression.to_source};"
        else
          "#{leading_trivia}return;"
        end
      end
    end

    # BlockStatement: `{ stmt1; stmt2; }`
    class BlockStatement < Statement
      attr_accessor :statements, :statement_trailings, :lbrace_suffix, :rbrace_prefix

      def initialize(statements:, statement_trailings:, leading_trivia: "", lbrace_suffix: "", rbrace_prefix: "")
        super(leading_trivia: leading_trivia)
        @statements = statements
        @statement_trailings = statement_trailings
        @lbrace_suffix = lbrace_suffix
        @rbrace_prefix = rbrace_prefix
      end

      def inline?
        instance_variable_get(:@inline)
      end

      def to_source
        result = "#{leading_trivia}{#{lbrace_suffix}"

        statements.zip(statement_trailings).each do |stmt, trailing|
          result << stmt.to_source << trailing
        end

        result << "#{rbrace_prefix}}"
        result
      end
    end

    # IfStatement: `if (cond) { ... } else { ... }`
    class IfStatement < Statement
      attr_accessor :condition, :then_statement, :else_statement, :if_suffix, :condition_lparen_suffix, :condition_rparen_suffix,
                    :else_prefix, :else_suffix

      def initialize(condition:, then_statement:, leading_trivia: "", else_statement: nil,
                     if_suffix: "", condition_lparen_suffix: "", condition_rparen_suffix: "",
                     else_prefix: "", else_suffix: "")
        super(leading_trivia: leading_trivia)
        @condition = condition
        @then_statement = then_statement
        @else_statement = else_statement
        @if_suffix = if_suffix
        @condition_lparen_suffix = condition_lparen_suffix
        @condition_rparen_suffix = condition_rparen_suffix
        @else_prefix = else_prefix
        @else_suffix = else_suffix
      end

      def to_source
        result = "#{leading_trivia}if#{if_suffix}(#{condition_lparen_suffix}"
        result << condition.to_source
        result << "#{condition_rparen_suffix})#{then_statement.to_source}"

        result << "#{else_prefix}else#{else_suffix}#{else_statement.to_source}" if else_statement

        result
      end
    end

    # WhileStatement: `while (cond) { ... }`
    class WhileStatement < Statement
      attr_accessor :condition, :body, :while_suffix, :condition_lparen_suffix, :condition_rparen_suffix

      def initialize(condition:, body:, leading_trivia: "",
                     while_suffix: "", condition_lparen_suffix: "", condition_rparen_suffix: "")
        super(leading_trivia: leading_trivia)
        @condition = condition
        @body = body
        @while_suffix = while_suffix
        @condition_lparen_suffix = condition_lparen_suffix
        @condition_rparen_suffix = condition_rparen_suffix
      end

      def to_source
        "#{leading_trivia}while#{while_suffix}(#{condition_lparen_suffix}" \
        "#{condition.to_source}#{condition_rparen_suffix})#{body.to_source}"
      end
    end

    # DoWhileStatement: `do { ... } while (cond);`
    class DoWhileStatement < Statement
      attr_accessor :body, :condition, :do_suffix, :while_prefix, :while_suffix, :condition_lparen_suffix, :condition_rparen_suffix

      def initialize(body:, condition:, leading_trivia: "",
                     do_suffix: "", while_prefix: "", while_suffix: "",
                     condition_lparen_suffix: "", condition_rparen_suffix: "")
        super(leading_trivia: leading_trivia)
        @body = body
        @condition = condition
        @do_suffix = do_suffix
        @while_prefix = while_prefix
        @while_suffix = while_suffix
        @condition_lparen_suffix = condition_lparen_suffix
        @condition_rparen_suffix = condition_rparen_suffix
      end

      def to_source
        "#{leading_trivia}do#{do_suffix}#{body.to_source}#{while_prefix}while#{while_suffix}" \
        "(#{condition_lparen_suffix}#{condition.to_source}#{condition_rparen_suffix});"
      end
    end

    # ForStatement: `for (init; cond; inc) { ... }`
    class ForStatement < Statement
      attr_accessor :init, :condition, :increment, :body, :for_suffix, :lparen_suffix, :init_trailing, :condition_trailing, :rparen_suffix

      def initialize(init:, condition:, increment:, body:, leading_trivia: "",
                     for_suffix: "", lparen_suffix: "",
                     init_trailing: "", condition_trailing: "", rparen_suffix: "")
        super(leading_trivia: leading_trivia)
        @init = init
        @condition = condition
        @increment = increment
        @body = body
        @for_suffix = for_suffix
        @lparen_suffix = lparen_suffix
        @init_trailing = init_trailing
        @condition_trailing = condition_trailing
        @rparen_suffix = rparen_suffix
      end

      def to_source
        result = "#{leading_trivia}for#{for_suffix}(#{lparen_suffix}"

        # Range-based for: for (decl : range)
        if init_trailing.start_with?(":")
          result << (init ? init.to_source : "")
          result << init_trailing
          result << (condition ? condition.to_source : "")
        else
          # Classic for: for (init; cond; inc)
          init_str = if init
                       init.respond_to?(:to_source) ? init.to_source : init.to_s
                     else
                       ""
                     end
          result << init_str
          result << ";#{init_trailing}"
          condition_str = if condition
                            condition.respond_to?(:to_source) ? condition.to_source : condition.to_s
                          else
                            ""
                          end
          result << condition_str
          result << ";#{condition_trailing}"
          increment_str = if increment
                            increment.respond_to?(:to_source) ? increment.to_source : increment.to_s
                          else
                            ""
                          end
          result << increment_str
        end

        result << ")#{rparen_suffix}#{body.to_source}"

        result
      end
    end

    # SwitchStatement: `switch (expr) { case 1: ...; default: ...; }`
    class SwitchStatement < Statement
      attr_accessor :expression, :cases, :switch_suffix, :lparen_suffix, :rparen_suffix, :lbrace_prefix, :lbrace_suffix, :rbrace_prefix

      def initialize(expression:, cases:, leading_trivia: "",
                     switch_suffix: "", lparen_suffix: "", rparen_suffix: "",
                     lbrace_prefix: "", lbrace_suffix: "", rbrace_prefix: "")
        super(leading_trivia: leading_trivia)
        @expression = expression
        @cases = cases
        @switch_suffix = switch_suffix
        @lparen_suffix = lparen_suffix
        @rparen_suffix = rparen_suffix
        @lbrace_prefix = lbrace_prefix
        @lbrace_suffix = lbrace_suffix
        @rbrace_prefix = rbrace_prefix
      end

      def to_source
        result = "#{leading_trivia}switch#{switch_suffix}(#{lparen_suffix}"
        result << "#{expression.to_source}#{rparen_suffix})#{lbrace_prefix}{#{lbrace_suffix}"
        cases.each { |c| result << c.to_source }
        result << "#{rbrace_prefix}}"
        result
      end
    end

    # CaseClause: `case value: statements`
    class CaseClause < Node
      attr_accessor :value, :statements, :statement_trailings, :leading_trivia, :case_suffix, :colon_suffix

      def initialize(value:, statements:, statement_trailings:, leading_trivia: "",
                     case_suffix: "", colon_suffix: "")
        @leading_trivia = leading_trivia
        @value = value
        @statements = statements
        @statement_trailings = statement_trailings
        @case_suffix = case_suffix
        @colon_suffix = colon_suffix
      end

      # Alias for backward compatibility
      def body
        statements.first
      end

      def to_source
        result = "#{leading_trivia}case#{case_suffix}#{value.to_source}:#{colon_suffix}"
        statements.zip(statement_trailings).each do |stmt, trailing|
          result << stmt.to_source << trailing
        end
        result
      end
    end

    # DefaultClause: `default: statements`
    class DefaultClause < Node
      attr_accessor :statements, :statement_trailings, :leading_trivia, :colon_suffix

      def initialize(statements:, statement_trailings:, leading_trivia: "", colon_suffix: "")
        @leading_trivia = leading_trivia
        @statements = statements
        @statement_trailings = statement_trailings
        @colon_suffix = colon_suffix
      end

      # Alias for backward compatibility
      def body
        statements.first
      end

      def to_source
        result = "#{leading_trivia}default:#{colon_suffix}"
        statements.zip(statement_trailings).each do |stmt, trailing|
          result << stmt.to_source << trailing
        end
        result
      end
    end

    # BreakStatement: `break;`
    class BreakStatement < Statement
      def initialize(leading_trivia: "")
        super(leading_trivia: leading_trivia)
      end

      def to_source
        "#{leading_trivia}break;"
      end
    end

    # ContinueStatement: `continue;`
    class ContinueStatement < Statement
      def initialize(leading_trivia: "")
        super(leading_trivia: leading_trivia)
      end

      def to_source
        "#{leading_trivia}continue;"
      end
    end

    # NamespaceDeclaration: `namespace name { ... }`
    class NamespaceDeclaration < Statement
      attr_accessor :name, :body, :namespace_suffix, :name_suffix

      def initialize(name:, body:, leading_trivia: "",
                     namespace_suffix: "", name_suffix: "", lbrace_suffix: "", rbrace_suffix: "")
        super(leading_trivia: leading_trivia)
        @name = name
        @body = body
        @namespace_suffix = namespace_suffix
        @name_suffix = name_suffix
        @lbrace_suffix = lbrace_suffix
        @rbrace_suffix = rbrace_suffix
      end

      def to_source
        result = "#{leading_trivia}namespace#{namespace_suffix}"
        result << "#{name}#{name_suffix}" unless name.empty?

        if body.is_a?(NamespaceDeclaration)
          # Nested namespace needs braces around it
          result << " {"
          result << body.to_source
          result << "}"
        elsif body.is_a?(BlockStatement)
          # Regular namespace with block body (already has {})
          result << body.to_source
        else
          # Single statement or unknown body type - wrap in {}
          result << " {"
          result << body.to_source
          result << "}"
        end
        result
      end
    end

    # FunctionDeclaration: `type name(params);` or `type name(params) override { ... }`
    class FunctionDeclaration < Statement
      attr_accessor :return_type, :name, :parameters, :body, :initializer_list, :return_type_suffix, :lparen_suffix, :rparen_suffix,
                    :param_separators, :modifiers_text, :prefix_modifiers, :modifier_set, :default_suffix

      def initialize(return_type:, name:, parameters:, leading_trivia: "", body: nil,
                     return_type_suffix: "", lparen_suffix: "", rparen_suffix: "",
                     param_separators: [], modifiers_text: "", prefix_modifiers: "",
                     initializer_list: nil, default_suffix: "")
        super(leading_trivia: leading_trivia)
        @return_type = return_type
        @name = name
        @parameters = parameters
        @body = body
        @initializer_list = initializer_list
        @return_type_suffix = return_type_suffix
        @lparen_suffix = lparen_suffix
        @rparen_suffix = rparen_suffix
        @param_separators = param_separators
        @modifiers_text = modifiers_text
        @prefix_modifiers = prefix_modifiers
        @default_suffix = default_suffix
      end

      def to_source
        return_type_str = return_type.respond_to?(:to_source) ? return_type.to_source : return_type.to_s
        suffix = return_type_str.empty? ? "" : return_type_suffix
        prefix = @modifier_set ? @modifier_set.to_s : prefix_modifiers
        result = "#{leading_trivia}#{prefix}#{return_type_str}#{suffix}#{name}(#{lparen_suffix}"

        unless parameters.empty?
          parameters.each_with_index do |param, i|
            param_str = param.respond_to?(:to_source) ? param.to_source : param.to_s
            result << param_str
            result << param_separators[i] if i < parameters.size - 1
          end
        end
        result << ")"

        # Architecture: rparen_suffix contains space or code (e.g., " = 0"), add if there's content or if it contains code
        has_content_after_rparen = !modifiers_text.empty? || body || initializer_list ||
                                   (@default_suffix && !@default_suffix.empty?)
        has_code_in_suffix = rparen_suffix&.include?("=")
        result << rparen_suffix if (has_content_after_rparen || has_code_in_suffix) && rparen_suffix && !rparen_suffix.empty?

        # Architecture: space is already in rparen_suffix, don't add it here
        result << modifiers_text unless modifiers_text.empty?

        # Add initializer list if present
        result << ": #{initializer_list}" if initializer_list

        # Add default suffix if present
        result << @default_suffix if @default_suffix && !@default_suffix.empty?

        # Only generate body if it's explicitly marked as inline
        result << if body && body.respond_to?(:inline?) && body.inline?
                    body.to_source
                  elsif body
                    body.to_source
                  else
                    ";"
                  end
        result
      end
    end

    # ClassDeclaration: `class Name { ... };` or `class Name : public Base { ... };`
    class ClassDeclaration < Statement
      attr_accessor :name, :members, :member_trailings, :class_suffix, :name_suffix, :lbrace_suffix, :rbrace_suffix, :base_classes_text

      def initialize(name:, members:, member_trailings:, leading_trivia: "",
                     class_suffix: "", name_suffix: "", lbrace_suffix: "", rbrace_suffix: "",
                     base_classes_text: "", final: false, abstract: false)
        super(leading_trivia: leading_trivia)
        @name = name
        @members = members
        @member_trailings = member_trailings
        @class_suffix = class_suffix
        @name_suffix = name_suffix
        @lbrace_suffix = lbrace_suffix
        @rbrace_suffix = rbrace_suffix
        @base_classes_text = base_classes_text
        @final = final
        @abstract = abstract
      end

      def to_source
        result = "#{leading_trivia}class#{class_suffix}#{name}#{name_suffix}"
        result << base_classes_text unless base_classes_text.empty?
        result << "{#{lbrace_suffix}"

        # Flatten members in case they are arrays (from public_section, private_section)
        flattened_members = members.flat_map { |m| m.is_a?(Array) ? m : [m] }
        flattened_trailings = member_trailings.flat_map { |t| t.is_a?(Array) ? t : [t] }

        flattened_members.zip(flattened_trailings).each do |member, trailing|
          result << member.to_source << (trailing || "")
        end

        result << "#{rbrace_suffix}};"
        result
      end
    end

    # StructDeclaration: `struct Name { ... };` or `struct Name : public Base { ... };`
    class StructDeclaration < Statement
      attr_accessor :name, :members, :member_trailings, :struct_suffix, :name_suffix, :lbrace_suffix, :rbrace_suffix, :base_classes_text

      def initialize(name:, members:, member_trailings:, leading_trivia: "",
                     struct_suffix: "", name_suffix: "", lbrace_suffix: "", rbrace_suffix: "",
                     base_classes_text: "")
        super(leading_trivia: leading_trivia)
        @name = name
        @members = members
        @member_trailings = member_trailings
        @struct_suffix = struct_suffix
        @name_suffix = name_suffix
        @lbrace_suffix = lbrace_suffix
        @rbrace_suffix = rbrace_suffix
        @base_classes_text = base_classes_text
      end

      def to_source
        result = "#{leading_trivia}struct#{struct_suffix}#{name}#{name_suffix}"
        result << base_classes_text unless base_classes_text.empty?
        result << "{#{lbrace_suffix}"

        # Flatten members in case they are arrays (from field_def calls)
        flattened_members = members.flat_map { |m| m.is_a?(Array) ? m : [m] }
        flattened_trailings = member_trailings.flat_map { |t| t.is_a?(Array) ? t : [t] }

        flattened_members.zip(flattened_trailings).each do |member, trailing|
          member_str = member.respond_to?(:to_source) ? member.to_source : member.to_s
          result << member_str << (trailing || "")
        end

        result << "#{rbrace_suffix}};"
        result
      end
    end

    # AccessSpecifier: `public:`, `private:`, `protected:`
    class AccessSpecifier < Statement
      attr_accessor :keyword, :colon_suffix

      def initialize(leading_trivia: "", keyword: nil, access_type: nil, colon_suffix: "")
        super(leading_trivia: leading_trivia)
        @keyword = keyword || access_type
        @colon_suffix = colon_suffix
      end

      def to_source
        "#{leading_trivia}#{keyword}:#{colon_suffix}"
      end
    end

    # VariableDeclaration: `int x = 42;` or `const int* ptr = nullptr;`
    class VariableDeclaration < Statement
      attr_accessor :type, :declarators, :declarator_separators, :type_suffix, :prefix_modifiers

      def initialize(type:, declarators:, leading_trivia: "", declarator_separators: [], type_suffix: "", prefix_modifiers: "")
        super(leading_trivia: leading_trivia)
        @type = type
        @declarators = declarators
        @declarator_separators = declarator_separators
        @type_suffix = type_suffix
        @prefix_modifiers = prefix_modifiers
      end

      def to_source
        type_str = type.respond_to?(:to_source) ? type.to_source : type.to_s
        result = "#{leading_trivia}#{prefix_modifiers}#{type_str}#{type_suffix}"

        declarators.each_with_index do |decl, i|
          result << decl
          result << declarator_separators[i] if i < declarators.size - 1
        end

        result << ";"
        result
      end
    end

    # EnumDeclaration: `enum Color { Red, Green };` or `enum class Color { Red, Green };`
    class EnumDeclaration < Statement
      attr_accessor :name, :enumerators, :enum_suffix, :class_keyword, :class_suffix, :name_suffix, :lbrace_suffix, :rbrace_suffix,
                    :underlying_type, :colon_prefix, :colon_suffix

      def initialize(name:, enumerators:, leading_trivia: "",
                     enum_suffix: "", class_keyword: "", class_suffix: "", name_suffix: "",
                     lbrace_suffix: "", rbrace_suffix: "", underlying_type: nil,
                     colon_prefix: " ", colon_suffix: "", class_enum: false, scoped: false)
        super(leading_trivia: leading_trivia)
        @name = name
        @enumerators = enumerators
        @enum_suffix = enum_suffix
        @class_keyword = class_keyword
        @class_suffix = class_suffix
        @name_suffix = name_suffix
        @lbrace_suffix = lbrace_suffix
        @rbrace_suffix = rbrace_suffix
        @underlying_type = underlying_type
        @colon_prefix = colon_prefix
        @colon_suffix = colon_suffix
        @class_enum = class_enum
        @scoped = scoped
      end

      def to_source
        result = "#{leading_trivia}enum#{enum_suffix}"
        result << "#{class_keyword}#{class_suffix}" unless class_keyword.empty?
        result << "#{name}#{name_suffix}"
        result << "#{colon_prefix}:#{colon_suffix}#{underlying_type}" if underlying_type
        result << "{#{lbrace_suffix}"

        # Convert enumerators array to string
        enumerator_strings = if enumerators.is_a?(String)
                               [enumerators]
                             else
                               enumerators.map do |enumerator|
                                 if enumerator.is_a?(Array)
                                   if enumerator[1]
                                     "#{enumerator[0]} = #{enumerator[1]}"
                                   else
                                     enumerator[0]
                                   end
                                 else
                                   enumerator.to_s
                                 end
                               end
                             end
        result << enumerator_strings.join(", ")

        result << "#{rbrace_suffix}};"
        result
      end
    end

    # TemplateDeclaration: `template<typename T> class Foo { ... };`
    class TemplateDeclaration < Statement
      attr_accessor :template_params, :declaration, :template_suffix, :less_suffix, :params_suffix

      def initialize(template_params:, declaration:, leading_trivia: "",
                     template_suffix: "", less_suffix: "", params_suffix: "")
        super(leading_trivia: leading_trivia)
        @template_params = template_params
        @declaration = declaration
        @template_suffix = template_suffix
        @less_suffix = less_suffix
        @params_suffix = params_suffix
      end

      def to_source
        if template_params.empty?
          "#{leading_trivia}template#{template_suffix}<>#{params_suffix}#{declaration.to_source}"
        else
          "#{leading_trivia}template#{template_suffix}<#{less_suffix}#{template_params}>#{params_suffix}#{declaration.to_source}"
        end
      end

      def inline_body(body)
        # Delegate to the underlying function declaration
        declaration.inline_body(body) if declaration.respond_to?(:inline_body)
        self
      end
    end

    # ErrorStatement: represents unparseable code that was recovered
    class ErrorStatement < Statement
      attr_accessor :error_text

      def initialize(error_text:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @error_text = error_text
      end

      def to_source
        "#{leading_trivia}#{error_text}"
      end
    end

    # UsingDeclaration: `using namespace std;` or `using MyType = int;`
    class UsingDeclaration < Statement
      attr_accessor :kind, :name, :alias_target, :using_suffix, :namespace_suffix, :equals_prefix, :equals_suffix

      # kind: :namespace, :name, :alias
      def initialize(kind:, name:, leading_trivia: "", alias_target: nil,
                     using_suffix: "", namespace_suffix: "", equals_prefix: "", equals_suffix: "")
        super(leading_trivia: leading_trivia)
        @kind = kind
        @name = name
        @alias_target = alias_target
        @using_suffix = using_suffix
        @namespace_suffix = namespace_suffix
        @equals_prefix = equals_prefix
        @equals_suffix = equals_suffix
      end

      def to_source
        result = "#{leading_trivia}using#{using_suffix}"

        case kind
        when :namespace
          result << "namespace#{namespace_suffix}#{name};"
        when :name
          result << "#{name};"
        when :alias
          result << "#{name}#{equals_prefix}=#{equals_suffix}#{alias_target};"
        end

        result
      end
    end

    # UsingNamespaceDirective: `using namespace std;`
    class UsingNamespaceDirective < Statement
      attr_accessor :namespace

      def initialize(namespace:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @namespace = namespace
      end

      def to_source
        "#{leading_trivia}using namespace #{namespace};"
      end
    end

    # FriendDeclaration: `friend class MyClass;` or `friend struct hash<Key>;`
    class FriendDeclaration < Statement
      attr_accessor :type, :name, :friend_suffix

      def initialize(type:, leading_trivia: "", name: nil, friend_suffix: " ")
        super(leading_trivia: leading_trivia)
        @type = type
        @name = name
        @friend_suffix = friend_suffix
      end

      def to_source
        result = "#{leading_trivia}friend"
        result << friend_suffix if type.empty?
        result << " #{type}" unless type.empty?
        result << " #{name}" if name
        result << ";"
        result
      end
    end

    # Program: Top-level container
    # Manages spacing between statements
    class Program < Node
      attr_accessor :statements, :statement_trailings

      def initialize(statements:, statement_trailings:)
        @statements = statements
        @statement_trailings = statement_trailings
      end

      def to_source
        statements.zip(statement_trailings).map do |stmt, trailing|
          stmt.to_source + trailing
        end.join
      end
    end

    # FieldDeclaration: `float x;` or `float x{0.0f};`
    class FieldDeclaration < Statement
      attr_accessor :type, :name, :default_value

      def initialize(type:, name:, leading_trivia: "", default_value: nil)
        super(leading_trivia: leading_trivia)
        @type = type
        @name = name
        @default_value = default_value
      end

      def to_source
        result = "#{leading_trivia}#{type} #{name}"
        result << "{#{default_value}}" if default_value
        result << ";"
        result
      end
    end

    # IncludeDirective: `#include <cstdint>` or `#include "my_header.hpp"`
    class IncludeDirective < Statement
      attr_accessor :path, :system

      def initialize(path:, leading_trivia: "", system: true)
        super(leading_trivia: leading_trivia)
        @path = path
        @system = system
      end

      def to_source
        if system
          "#{leading_trivia}#include <#{path}>"
        else
          "#{leading_trivia}#include \"#{path}\""
        end
      end
    end

    # PragmaDirective: `#pragma once`
    class PragmaDirective < Statement
      attr_accessor :directive

      def initialize(directive:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @directive = directive
      end

      def to_source
        "#{leading_trivia}#pragma #{directive}"
      end
    end

    # ConstDeclaration: `constexpr type name = value;`
    class ConstDeclaration < Statement
      attr_accessor :type, :name, :value

      def initialize(type:, name:, value:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @type = type
        @name = name
        @value = value
      end

      def to_source
        "#{leading_trivia}constexpr #{type} #{name} = #{value};"
      end
    end

    # Comment nodes - Phase 2
    class InlineComment < Statement
      attr_accessor :text

      def initialize(text:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @text = text
      end

      def to_source
        "#{leading_trivia}// #{text}"
      end
    end

    class BlockComment < Statement
      attr_accessor :text

      def initialize(text:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @text = text
      end

      def to_source
        "#{leading_trivia}/* #{text} */"
      end
    end

    class DoxygenComment < Statement
      attr_accessor :text, :style

      def initialize(text:, leading_trivia: "", style: :inline)
        super(leading_trivia: leading_trivia)
        @text = text
        @style = style
      end

      def to_source
        case style
        when :inline
          "#{leading_trivia}/// #{text}"
        when :block
          "#{leading_trivia}/** #{text} */"
        else
          "#{leading_trivia}/// #{text}"
        end
      end
    end

    # Preprocessor directives - Phase 2
    class DefineDirective < Statement
      attr_accessor :name, :value

      def initialize(name:, leading_trivia: "", value: "")
        super(leading_trivia: leading_trivia)
        @name = name
        @value = value
      end

      def to_source
        if value.empty?
          "#{leading_trivia}#define #{name}"
        else
          "#{leading_trivia}#define #{name} #{value}"
        end
      end
    end

    class IfdefDirective < Statement
      attr_accessor :name, :body

      def initialize(name:, leading_trivia: "", body: [])
        super(leading_trivia: leading_trivia)
        @name = name
        @body = body
      end

      def to_source
        result = "#{leading_trivia}#ifdef #{name}\n"
        body.each do |stmt|
          result += "#{stmt.to_source}\n"
        end
        result += "#endif"
        result
      end
    end

    class IfndefDirective < Statement
      attr_accessor :name, :body

      def initialize(name:, leading_trivia: "", body: [])
        super(leading_trivia: leading_trivia)
        @name = name
        @body = body
      end

      def to_source
        result = "#{leading_trivia}#ifndef #{name}\n"
        body.each do |stmt|
          result += "#{stmt.to_source}\n"
        end
        result += "#endif"
        result
      end
    end

    # C++20 Concepts - Phase 4
    class ConceptDeclaration < Statement
      attr_accessor :name, :template_params, :requirements

      def initialize(name:, template_params:, requirements:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @name = name
        @template_params = template_params
        @requirements = requirements
      end

      def to_source
        result = "#{leading_trivia}template <#{template_params.join(', ')}>\n"
        result += "concept #{name} = #{requirements};"
        result
      end
    end

    # C++20 Modules - Phase 4
    class ModuleDeclaration < Statement
      attr_accessor :name, :body

      def initialize(name:, leading_trivia: "", body: [])
        super(leading_trivia: leading_trivia)
        @name = name
        @body = body
      end

      def to_source
        result = "#{leading_trivia}export module #{name};\n"
        body.each do |stmt|
          result += "#{stmt.to_source}\n"
        end
        result
      end
    end

    class ImportDeclaration < Statement
      attr_accessor :module_name

      def initialize(module_name:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @module_name = module_name
      end

      def to_source
        "#{leading_trivia}import #{module_name};"
      end
    end

    class ExportDeclaration < Statement
      attr_accessor :declarations

      def initialize(leading_trivia: "", declarations: [])
        super(leading_trivia: leading_trivia)
        @declarations = declarations
      end

      def to_source
        result = "#{leading_trivia}export {\n"
        declarations.each do |decl|
          result += "  #{decl.to_source}\n"
        end
        result += "}"
        result
      end
    end

    # C++20 Coroutines - Phase 4
    class CoAwaitExpression < Statement
      attr_accessor :expression

      def initialize(expression:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @expression = expression
      end

      def to_source
        "#{leading_trivia}co_await #{expression.to_source}"
      end
    end

    class CoYieldExpression < Statement
      attr_accessor :expression

      def initialize(expression:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @expression = expression
      end

      def to_source
        "#{leading_trivia}co_yield #{expression.to_source}"
      end
    end

    class CoReturnStatement < Statement
      attr_accessor :expression

      def initialize(leading_trivia: "", expression: nil)
        super(leading_trivia: leading_trivia)
        @expression = expression
      end

      def to_source
        if expression
          "#{leading_trivia}co_return #{expression.to_source};"
        else
          "#{leading_trivia}co_return;"
        end
      end
    end

    # CatchClause: `catch (Type& var) { ... }`
    class CatchClause < Statement
      attr_accessor :type, :variable, :body

      def initialize(type:, variable:, body:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @type = type
        @variable = variable
        @body = body
      end

      def to_source
        "#{leading_trivia}catch (#{type.to_source} #{variable.to_source}) #{body.to_source}"
      end
    end

    # ElseClause: `else body`
    class ElseClause < Statement
      attr_accessor :body

      def initialize(leading_trivia: "", body: nil)
        super(leading_trivia: leading_trivia)
        @body = body
      end

      def to_source
        if body
          "#{leading_trivia}else #{body.to_source}"
        else
          "#{leading_trivia}else"
        end
      end
    end

    # ThrowStatement: `throw exception;`
    class ThrowStatement < Statement
      attr_accessor :expression

      def initialize(expression:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @expression = expression
      end

      def to_source
        "#{leading_trivia}throw #{expression.to_source};"
      end
    end

    # TryStatement: `try { ... } catch (...) { ... }`
    class TryStatement < Statement
      attr_accessor :try_block, :catch_clauses

      def initialize(try_block:, leading_trivia: "", catch_clauses: [])
        super(leading_trivia: leading_trivia)
        @try_block = try_block
        @catch_clauses = catch_clauses
      end

      # Alias for backward compatibility
      def body
        try_block
      end

      def to_source
        result = "#{leading_trivia}try #{try_block.to_source}"
        catch_clauses.each do |clause|
          result += " #{clause.to_source}"
        end
        result
      end
    end

    # RangeForStatement: `for (auto& item : container) { ... }`
    class RangeForStatement < Statement
      attr_accessor :variable, :container, :body

      def initialize(variable:, container:, body:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @variable = variable
        @container = container
        @body = body
      end

      # Alias for backward compatibility
      def range
        container
      end

      def to_source
        "#{leading_trivia}for (#{variable.to_source} : #{container.to_source}) #{body.to_source}"
      end
    end

    # RawStatement: raw C++ code as string
    class RawStatement < Statement
      attr_accessor :code

      def initialize(code:, leading_trivia: "")
        super(leading_trivia: leading_trivia)
        @code = code
      end

      def to_source
        "#{leading_trivia}#{code}"
      end
    end
  end
end
# rubocop:enable Lint/MissingSuper, Metrics/ParameterLists

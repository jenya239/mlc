# frozen_string_literal: true

require 'test_helper'
require 'mlc/irgen_v2/engine'

module MLC
  module IRGenV2
    class EngineTest < Minitest::Test
      TestFunctionInfo = Struct.new(:name, :param_types, :ret_type, :type_params)

      def setup
        @function_registry = MLC::FunctionRegistry.new
        @type_registry = MLC::TypeRegistry.new

        builder = MLC::IRGenV2::Services::IRBuilder.new
        float_type = builder.prim_type(name: 'f64')

        func_info = TestFunctionInfo.new(
          'sqrt',
          [float_type],
          float_type,
          []
        )

        @function_registry.register(
          'Math.sqrt',
          func_info,
          { module_name: 'Math', namespace: 'mlc::math' }
        )
      end

      def test_transforms_module_member_expression
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        node = factory.member_access(
          object: factory.var_ref(name: 'Math'),
          member: 'sqrt'
        )

        ir = engine.run_expression(node)

        assert_instance_of MLC::HighIR::VarExpr, ir
        assert_equal 'Math.sqrt', ir.name
      end

      def test_handles_literal_expression
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        node = factory.int_literal(value: 123)

        ir = engine.run_expression(node)
        assert_equal 123, ir.value
        assert_equal 'i32', ir.type.name
      end

      def test_handles_variable_reference
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        engine.services.var_type_registry.set('x', builder.prim_type(name: 'i32'))

        factory = engine.services.ast_factory
        node = factory.var_ref(name: 'x')

        ir = engine.run_expression(node)
        assert_instance_of MLC::HighIR::VarExpr, ir
        assert_equal 'x', ir.name
        assert_equal 'i32', ir.type.name
      end

      def test_handles_function_reference
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        ret_type = builder.prim_type(name: 'f64')
        func_info = TestFunctionInfo.new(
          'len',
          [builder.prim_type(name: 'string')],
          ret_type,
          []
        )
        @function_registry.register('len', func_info)

        factory = engine.services.ast_factory
        node = factory.var_ref(name: 'len')

        ir = engine.run_expression(node)
        assert_instance_of MLC::HighIR::VarExpr, ir
        assert_equal 'len', ir.name
        assert_equal :func, ir.type.kind
      end

      def test_handles_function_call
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        string_type = builder.prim_type(name: 'string')
        ret_type = builder.prim_type(name: 'i32')

        func_info = TestFunctionInfo.new('len', [string_type], ret_type, [])
        @function_registry.register('len', func_info)

        factory = engine.services.ast_factory
        node = factory.call(
          callee: factory.var_ref(name: 'len'),
          args: [factory.string_literal(value: 'hello')]
        )

        ir = engine.run_expression(node)
        assert_instance_of MLC::HighIR::CallExpr, ir
        assert_equal 'len', ir.callee.name
        assert_equal 'i32', ir.type.name
      end

      def test_function_call_argument_mismatch
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        string_type = builder.prim_type(name: 'string')
        ret_type = builder.prim_type(name: 'i32')

        func_info = TestFunctionInfo.new('len', [string_type], ret_type, [])
        @function_registry.register('len', func_info)

        factory = engine.services.ast_factory
        node = factory.call(
          callee: factory.var_ref(name: 'len'),
          args: []
        )

        assert_raises(MLC::CompileError) do
          engine.run_expression(node)
        end
      end

      def test_handles_unary_operator
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        engine.services.var_type_registry.set('x', builder.prim_type(name: 'i32'))

        factory = engine.services.ast_factory
        node = factory.unary(op: '-', operand: factory.var_ref(name: 'x'))

        ir = engine.run_expression(node)
        assert_instance_of MLC::HighIR::UnaryExpr, ir
        assert_equal 'i32', ir.type.name
      end

      def test_handles_binary_operator
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        engine.services.var_type_registry.set('x', builder.prim_type(name: 'i32'))

        factory = engine.services.ast_factory
        node = factory.binary(
          op: '+',
          left: factory.var_ref(name: 'x'),
          right: factory.int_literal(value: 1)
        )

        ir = engine.run_expression(node)
        assert_instance_of MLC::HighIR::BinaryExpr, ir
        assert_equal 'i32', ir.type.name
      end

      def test_handles_pipe_operator_with_call
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        string_type = builder.prim_type(name: 'string')
        ret_type = builder.prim_type(name: 'i32')

        func_info = TestFunctionInfo.new('len', [string_type], ret_type, [])
        @function_registry.register('len', func_info)

        factory = engine.services.ast_factory
        call_ast = factory.call(
          callee: factory.var_ref(name: 'len'),
          args: [],
          origin: nil
        )
        pipe_ast = factory.binary(
          op: '|>',
          left: factory.string_literal(value: 'hi'),
          right: call_ast
        )

        ir = engine.run_expression(pipe_ast)
        assert_instance_of MLC::HighIR::CallExpr, ir
        assert_equal 'len', ir.callee.name
        assert_equal 'i32', ir.type.name
        assert_equal 1, ir.args.length
      end

      def test_handles_pipe_operator_with_function_reference
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        ret_type = builder.prim_type(name: 'i32')
        func_info = TestFunctionInfo.new('inc', [builder.prim_type(name: 'i32')], ret_type, [])
        @function_registry.register('inc', func_info)

        factory = engine.services.ast_factory
        pipe_ast = factory.binary(
          op: '|>',
          left: factory.int_literal(value: 1),
          right: factory.var_ref(name: 'inc')
        )

        ir = engine.run_expression(pipe_ast)
        assert_instance_of MLC::HighIR::CallExpr, ir
        assert_equal 1, ir.args.length
        assert_equal 'inc', ir.callee.name
      end

      def test_handles_unit_literal
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        node = factory.unit_literal

        ir = engine.run_expression(node)
        assert_instance_of MLC::HighIR::UnitLiteral, ir
      end

      def test_handles_let_expression
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        let_ast = factory.let(
          name: 'x',
          value: factory.int_literal(value: 2),
          body: factory.binary(
            op: '+',
            left: factory.var_ref(name: 'x'),
            right: factory.int_literal(value: 3)
          )
        )

        ir = engine.run_expression(let_ast)

        assert_instance_of MLC::HighIR::BlockExpr, ir
        assert_equal 'i32', ir.type.name
        refute engine.services.var_type_registry.has?('x'), 'let binding should not leak into outer scope'
      end

      def test_handles_array_literal
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        array_ast = factory.array_literal(
          elements: [factory.int_literal(value: 1), factory.int_literal(value: 2)]
        )

        ir = engine.run_expression(array_ast)
        assert_instance_of MLC::HighIR::ArrayLiteralExpr, ir
        assert_equal 'array', ir.type.name
        assert_equal 2, ir.elements.length
      end

      def test_handles_named_record_literal
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        builder = engine.services.ir_builder
        record_type = builder.record_type(
          name: 'Point',
          fields: [
            { name: 'x', type: builder.prim_type(name: 'i32') },
            { name: 'y', type: builder.prim_type(name: 'i32') }
          ]
        )
        @type_registry.register('Point', core_ir_type: record_type, kind: :record)

        factory = engine.services.ast_factory
        record_ast = factory.record_literal(
          type_name: 'Point',
          fields: {
            'x' => factory.int_literal(value: 1),
            'y' => factory.int_literal(value: 2)
          }
        )

        ir = engine.run_expression(record_ast)
        assert_instance_of MLC::HighIR::RecordExpr, ir
        assert_equal 'Point', ir.type.name
        assert_equal 2, ir.fields.size
      end

      def test_handles_if_expression
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        if_ast = MLC::AST::IfExpr.new(
          condition: factory.var_ref(name: 'true'),
          then_branch: factory.int_literal(value: 1),
          else_branch: factory.int_literal(value: 2)
        )

        ir = engine.run_expression(if_ast)
        assert_instance_of MLC::HighIR::IfExpr, ir
        assert_equal 'i32', ir.type.name
      end

      def test_handles_block_expression
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        block_ast = factory.block_expr(
          statements: [
            factory.variable_decl(name: 'x', value: factory.int_literal(value: 5)),
            factory.expr_stmt(expr: factory.var_ref(name: 'x'))
          ],
          result_expr: factory.var_ref(name: 'x')
        )

        ir = engine.run_expression(block_ast)

        assert_instance_of MLC::HighIR::BlockExpr, ir
        assert_equal 'i32', ir.type.name
        assert_equal 2, ir.statements.length
        refute engine.services.var_type_registry.has?('x')
      end

      def test_handles_do_expression
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        do_ast = factory.do_expr(
          body: [
            factory.int_literal(value: 1),
            factory.int_literal(value: 3)
          ]
        )

        ir = engine.run_expression(do_ast)

        assert_instance_of MLC::HighIR::BlockExpr, ir
        assert_equal 'i32', ir.type.name
      end

      def test_handles_assignment_statement
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        block_ast = factory.block_expr(
          statements: [
            factory.variable_decl(name: 'x', value: factory.int_literal(value: 1)),
            factory.assignment(
              target: factory.var_ref(name: 'x'),
              value: factory.int_literal(value: 2)
            )
          ],
          result_expr: factory.var_ref(name: 'x')
        )

        ir = engine.run_expression(block_ast)
        assert_equal 'i32', ir.type.name
        assert_instance_of MLC::HighIR::VarExpr, ir.result
        assert_equal 'x', ir.result.name
      end

      def test_handles_for_loop_expression
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        array_expr = factory.array_literal(
          elements: [factory.int_literal(value: 1), factory.int_literal(value: 2)]
        )

        for_expr = factory.for_loop(
          var_name: 'item',
          iterable: factory.var_ref(name: 'arr'),
          body: factory.var_ref(name: 'item')
        )

        block_ast = factory.block_expr(
          statements: [
            factory.variable_decl(name: 'arr', value: array_expr),
            factory.expr_stmt(expr: for_expr)
          ],
          result_expr: factory.var_ref(name: 'arr')
        )

        ir = engine.run_expression(block_ast)

        assert_instance_of MLC::HighIR::BlockExpr, ir
        assert_equal 'array', ir.type.name
        refute engine.services.var_type_registry.has?('item')
      end

      def test_handles_while_statement
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        factory = engine.services.ast_factory
        body_block = MLC::AST::Block.new(
          stmts: [
            factory.assignment(
              target: factory.var_ref(name: 'x'),
              value: factory.int_literal(value: 10)
            )
          ]
        )

        while_stmt = MLC::AST::WhileStmt.new(
          condition: factory.var_ref(name: 'false'),
          body: body_block
        )

        block_ast = factory.block_expr(
          statements: [
            factory.variable_decl(name: 'x', value: factory.int_literal(value: 1)),
            while_stmt
          ],
          result_expr: factory.var_ref(name: 'x')
        )

        ir = engine.run_expression(block_ast)

        assert_instance_of MLC::HighIR::BlockExpr, ir
        assert_equal 'i32', ir.type.name
        assert_equal 2, ir.statements.length
      end

      def test_respects_import_aliases
        engine = Engine.new(
          function_registry: @function_registry,
          type_registry: @type_registry
        )

        engine.services.module_resolver.add_alias('M', 'Math')

        factory = engine.services.ast_factory
        node = factory.member_access(
          object: factory.var_ref(name: 'M'),
          member: 'sqrt'
        )

        ir = engine.run_expression(node)
        assert_equal 'Math.sqrt', ir.name
      end
    end
  end
end

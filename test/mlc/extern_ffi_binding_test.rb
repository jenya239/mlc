# frozen_string_literal: true

require_relative "../test_helper"

class ExternFfiBindingTest < Minitest::Test
  def test_parse_extern_fn_c_name_from_header
    source = 'extern fn sqrt(x: f64) -> f64 = "sqrt" from "<math.h>"'
    ast = MLC.parse(source)
    function = ast.declarations.first

    assert_instance_of MLC::Source::AST::FuncDecl, function
    assert function.external
    assert_equal "sqrt", function.extern_c_name
    assert_equal "<math.h>", function.extern_header
    assert_nil function.body
  end

  def test_codegen_emits_include_and_c_call_wrapper
    source = <<~MLC
      extern fn cabs(x: f64) -> f64 = "cabs" from "<complex.h>"
      fn main() -> i32 = 0
    MLC

    cpp = MLC.compile(source).to_source
    assert_includes cpp, "#include <complex.h>"
    assert_includes cpp, "mlc_ffi_bind_cabs"
    assert_includes cpp, "&::cabs"
    assert_includes cpp, "return mlc_ffi_bind_cabs(x);"
  end
end

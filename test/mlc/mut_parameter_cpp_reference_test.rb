# frozen_string_literal: true

require_relative "../test_helper"

# mut parameters lower to non-const C++. Prvalues need a holder before binding to mutable references.
class MutParameterCppReferenceTest < Minitest::Test
  def test_mut_array_parameter_declaration_uses_ampersand
    mlc = <<~SRC
      fn writer(mut buckets: [string], text: string) -> () =
        if text != '' then buckets.push(text) else () end

      fn unused() -> () = ()
    SRC
    cpp = MLC.to_cpp(mlc)
    assert_match(/mlc::Array<\s*mlc::String\s*>\s*&\s*buckets/, cpp)
  end

  def test_mut_array_actual_argument_that_is_call_generates_lambda_holder
    mlc = <<~SRC
      fn touch_nonempty(mut items: [string], label: string) -> () =
        if label != '' then items.push(label) else () end

      fn spawned_collection_under_scan() -> [string] = []

      export fn demonstration() -> [string] = do
        let mutable_collection: [string] = []
        touch_nonempty(spawned_collection_under_scan(), "segment")
        mutable_collection
      end
    SRC
    cpp = MLC.to_cpp(mlc)
    assert_match(/\[\s*&\s*\]\s*\(\)\s*\{/, cpp)
    assert_match(/return\s+touch_nonempty\s*\(/, cpp)
    assert_match(/\bspawned_collection_under_scan\s*\(/, cpp)
  end
end

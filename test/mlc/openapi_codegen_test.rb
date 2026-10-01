# frozen_string_literal: true

require "open3"
require "test_helper"
require_relative "../../scripts/openapi_codegen"

class OpenapiCodegenTest < Minitest::Test
  FIXTURE = File.expand_path("../fixtures/openapi/mini_petstore.yaml", __dir__)

  def setup
    @spec = OpenapiCodegen.load_spec(FIXTURE)
    @source = OpenapiCodegen.generate(@spec)
  end

  def test_emits_record_types_with_json_derive
    assert_includes @source, "type Pet = {"
    assert_includes @source, "derive { Json }"
    assert_includes @source, "id: i64"
    assert_includes @source, "name: string"
    assert_includes @source, "photo_urls: Option<[string]>"
  end

  def test_emits_enum_as_sum
    assert_includes @source, "type PetStatus = Available | Pending | Sold derive { Json }"
  end

  def test_emits_oneof_as_sum
    assert_match(/type PetOrError = PetCase\(Pet\) \| /, @source)
  end

  def test_emits_client_functions
    assert_includes @source, "type ApiClient = { base_url: string }"
    assert_includes @source, "type ApiResult<T> = Ok(T) | Err(string)"
    assert_includes @source, "fn get_pet_by_id(client: ApiClient, pet_id: i64) -> ApiResult<Pet>"
    assert_includes @source, "fn create_pet(client: ApiClient, body: NewPet) -> ApiResult<Pet>"
  end

  def test_cli_writes_file
    Dir.mktmpdir do |directory|
      output_path = File.join(directory, "petstore.mlc")
      status = system(
        RbConfig.ruby,
        File.expand_path("../../scripts/openapi_codegen.rb", __dir__),
        FIXTURE,
        output_path,
        out: File::NULL,
        err: File::NULL
      )
      assert status
      written = File.read(output_path)
      assert_includes written, "type Pet ="
      assert_includes written, "fn get_pet_by_id"
    end
  end

  def test_mini_petstore_compiles_with_mlcc_and_gxx
    source = @source + "\nfn main() -> i32 = 0\n"
    runtime_include = File.expand_path("../../runtime/include", __dir__)
    mlcc = File.expand_path("../../compiler/out/mlcc", __dir__)
    Dir.mktmpdir("mlc_openapi_petstore") do |directory|
      entry_path = File.join(directory, "petstore.mlc")
      File.write(entry_path, source)
      generated_directory = File.join(directory, "generated")
      Dir.mkdir(generated_directory)
      mlcc_output, mlcc_status = Open3.capture2e(mlcc, "-o", generated_directory, entry_path)
      assert mlcc_status.success?, mlcc_output
      header_path = File.join(generated_directory, "petstore.hpp")
      assert File.file?(header_path), "mlcc did not emit petstore.hpp"
      syntax_output, syntax_status = Open3.capture2e(
        "clang++", "-std=c++20", "-fsyntax-only",
        "-I", runtime_include,
        "-I", generated_directory,
        header_path
      )
      assert syntax_status.success?, syntax_output
    end
  end
end

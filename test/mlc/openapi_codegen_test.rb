# frozen_string_literal: true

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
    assert_includes @source, "photo_urls: Option<Array<string>>"
  end

  def test_emits_enum_as_sum
    assert_includes @source, "type PetStatus = Available | Pending | Sold derive { Json }"
  end

  def test_emits_oneof_as_sum
    assert_match(/type PetOrError = Pet\(Pet\) \| /, @source)
  end

  def test_emits_client_functions
    assert_includes @source, "type ApiClient = { base_url: string }"
    assert_includes @source, "fn get_pet_by_id(client: ApiClient, pet_id: i64) -> Result<Pet, ApiError>"
    assert_includes @source, "fn create_pet(client: ApiClient, body: NewPet) -> Result<Pet, ApiError>"
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
end

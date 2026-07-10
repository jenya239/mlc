# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCPostgresStdlibTest < Minitest::Test
  def test_postgres_connect_exec_codegen
    source = <<~MLC
      import Postgres::{connect, exec, ntuples, nfields, getvalue, clear, finish, last_error}
      fn main() -> i32 = do
        let _connection = connect("host=127.0.0.1")
        let _result = exec(0, "SELECT 1")
        let _rows = ntuples(0)
        let _cols = nfields(0)
        let _cell = getvalue(0, 0, 0)
        let _err = last_error()
        clear(0)
        finish(0)
        0
      end
    MLC
    cpp = MLC.compile(source).to_source
    assert_includes cpp, "mlc/db/postgres_bridge.hpp"
    assert_includes cpp, "mlc::db::connect"
    assert_includes cpp, "mlc::db::exec"
    assert_includes cpp, "mlc::db::ntuples"
    assert_includes cpp, "mlc::db::nfields"
    assert_includes cpp, "mlc::db::getvalue"
    assert_includes cpp, "mlc::db::clear"
    assert_includes cpp, "mlc::db::finish"
    assert_includes cpp, "mlc::db::last_error"
  end
end

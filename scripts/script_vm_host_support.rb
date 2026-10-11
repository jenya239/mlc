require "fileutils"
require "timeout"

module ScriptVmHostSupport
  module_function

  def repository_root
    File.expand_path("..", __dir__)
  end

  def prepare_environment(root_directory)
    ENV["TMPDIR"] = File.join(root_directory, "tmp")
    ENV["MLCC_OBJ_CLEAN"] = "1"
    ENV["MLCC_PCH"] = "0"
    FileUtils.mkdir_p(ENV.fetch("TMPDIR"))
  end

  def capture_process(arguments, timeout_seconds)
    output_reader, output_writer = IO.pipe
    error_reader, error_writer = IO.pipe
    child_process_identifier = Process.spawn(*arguments, out: output_writer, err: error_writer)
    output_writer.close
    error_writer.close
    standard_output = +""
    standard_error = +""
    output_reader_thread = Thread.new { standard_output.replace(output_reader.read.to_s) }
    error_reader_thread = Thread.new { standard_error.replace(error_reader.read.to_s) }
    process_status = nil
    begin
      Timeout.timeout(timeout_seconds) do
        _, process_status = Process.wait2(child_process_identifier)
      end
    rescue Timeout::Error
      Process.kill("KILL", child_process_identifier)
      begin
        Process.wait(child_process_identifier)
      rescue Errno::ECHILD
        nil
      end
      output_reader_thread.join
      error_reader_thread.join
      output_reader.close
      error_reader.close
      return ["", "", :timeout]
    end
    output_reader_thread.join
    error_reader_thread.join
    output_reader.close
    error_reader.close
    [standard_output, standard_error, process_status]
  end

  def status_code(process_status)
    return nil if process_status == :timeout
    return nil unless process_status.is_a?(Process::Status)
    return nil unless process_status.exited?
    process_status.exitstatus
  end

  def build_embedding_host(root_directory, host_source_path, output_directory, binary_path)
    compiler_directory = File.join(root_directory, "compiler")
    mlcc_path = File.join(compiler_directory, "out", "mlcc")
    embedding_source_path = File.join(root_directory, "script_vm", "embedding.mlc")
    bridge_source_path = File.join(root_directory, "runtime", "src", "script_vm", "embedding_abi.cpp")
    return [false, "mlcc not found at #{mlcc_path}"] unless File.executable?(mlcc_path)
    return [false, "missing #{embedding_source_path}"] unless File.file?(embedding_source_path)
    return [false, "missing #{bridge_source_path}"] unless File.file?(bridge_source_path)
    return [false, "missing #{host_source_path}"] unless File.file?(host_source_path)

    FileUtils.rm_rf(output_directory)
    FileUtils.mkdir_p(output_directory)
    _compile_output, compile_error, compile_status = capture_process(
      [mlcc_path, "-o", output_directory, embedding_source_path],
      600
    )
    unless status_code(compile_status) == 0
      return [false, "mlcc failed\n#{compile_error}"]
    end

    FileUtils.cp(bridge_source_path, File.join(output_directory, "embedding_abi.cpp"))
    FileUtils.cp(host_source_path, File.join(output_directory, File.basename(host_source_path)))
    _link_output, link_error, link_status = capture_process(
      [File.join(compiler_directory, "build_bin.sh"), output_directory, binary_path],
      600
    )
    unless status_code(link_status) == 0
      return [false, "build_bin failed\n#{link_error}"]
    end
    return [false, "missing host binary"] unless File.executable?(binary_path)

    [true, ""]
  end

  def run_built_host(binary_path, arguments, timeout_seconds = 20)
    capture_process([binary_path, *arguments], timeout_seconds)
  end
end

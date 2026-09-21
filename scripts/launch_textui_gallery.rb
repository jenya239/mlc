#!/usr/bin/env ruby
# frozen_string_literal: true

root = File.expand_path("..", __dir__)
binary = File.join(root, "tmp/textui_label_gallery/bin")
textui_root = File.join(root, "misc/textui")
compile_script = File.join(root, "scripts/run_textui_label_gallery.sh")
application_reload_status = 75

def latest_mlc_mtime(directory)
  Dir.glob(File.join(directory, "**", "*.mlc")).map { |path| File.mtime(path) }.max
end

def need_build?(binary, textui_root)
  return true unless File.executable?(binary)

  source_mtime = latest_mlc_mtime(textui_root)
  source_mtime && source_mtime > File.mtime(binary)
end

def compile_gallery(compile_script)
  env = ENV.to_h
  env["MLC_TEXTUI_COMPILE_ONLY"] = "1"
  return if system(env, "/bin/bash", compile_script)

  abort "launch_textui_gallery: compile failed"
end

ENV["DISPLAY"] = ENV["DISPLAY"].to_s.empty? ? ":0.0" : ENV["DISPLAY"]
ENV["MLC_GLFW_VISIBLE"] = "1"
ENV.delete("MLC_TEXTUI_BENCH")
ENV["TMPDIR"] = File.join(root, "tmp")
ENV["MLCC_OBJ_CLEAN"] ||= "1"
ENV["MLCC_PCH"] ||= "0"
Dir.chdir(root)

loop do
  compile_gallery(compile_script) if need_build?(binary, textui_root)
  abort "launch_textui_gallery: missing #{binary}" unless File.executable?(binary)
  system(binary)
  status = $?.nil? ? 1 : $?.exitstatus
  next if status == application_reload_status

  exit status
end

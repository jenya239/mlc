#!/usr/bin/env ruby
# frozen_string_literal: true

require "fileutils"

root = File.expand_path("..", __dir__)
icon_name = "mlc-textui"
svg_source = File.join(root, "misc/textui/share/mlc-textui.svg")
icon_dir_48 = File.join(Dir.home, ".local/share/icons/hicolor/48x48/apps")
icon_dir_svg = File.join(Dir.home, ".local/share/icons/hicolor/scalable/apps")
png_path = File.join(icon_dir_48, "#{icon_name}.png")
svg_path = File.join(icon_dir_svg, "#{icon_name}.svg")
applications_dir = File.join(Dir.home, ".local/share/applications")
desktop_id = "dev.mlc.textui.desktop"
applications_desktop = File.join(applications_dir, desktop_id)
ruby_bin = "/home/jenya/.rbenv/versions/3.4.7/bin/ruby"
launch_script = File.join(root, "scripts/launch_textui_gallery.rb")
plugin_id = 47
ruby_playground_plugin = 46

FileUtils.mkdir_p(icon_dir_48)
FileUtils.mkdir_p(icon_dir_svg)
FileUtils.mkdir_p(applications_dir)
FileUtils.cp(svg_source, svg_path)

convert_status = system(
  "convert",
  "-background", "none",
  "-density", "256",
  "-resize", "48x48",
  svg_source,
  png_path
)
unless convert_status && File.file?(png_path)
  warn "install_textui_xfce_launcher: convert SVG failed, drawing PNG"
  system(
    "convert",
    "-size", "48x48", "xc:#12151a",
    "-fill", "#e8eaed",
    "-draw", "roundrectangle 9,10 39,16 2,2",
    "-fill", "#8c939c",
    "-draw", "roundrectangle 9,20 30,26 2,2",
    "-fill", "#29323a",
    "-stroke", "#5c6670",
    "-strokewidth", "2",
    "-draw", "roundrectangle 9,30 29,39 3,3",
    png_path
  ) or abort "install_textui_xfce_launcher: PNG create failed"
end

desktop_body = <<~DESKTOP
  [Desktop Entry]
  Version=1.0
  Type=Application
  Name=MLC textui
  Comment=PLAN §112 widget gallery (labels, selection, buttons)
  Exec=#{ruby_bin} #{launch_script}
  Path=#{root}
  Icon=#{png_path}
  Terminal=false
  StartupNotify=false
  StartupWMClass=mlc gl renderer
  Categories=Development;Graphics;
  Keywords=mlc;textui;opengl;gallery;
DESKTOP

File.write(applications_desktop, desktop_body)

panel_dir = File.join(Dir.home, ".config/xfce4/panel", "launcher-#{plugin_id}")
FileUtils.mkdir_p(panel_dir)
stamp = "#{Time.now.to_i}1.desktop"
panel_desktop = File.join(panel_dir, stamp)
File.write(
  panel_desktop,
  desktop_body + "\nX-XFCE-Source=file://#{applications_desktop}\n"
)

system(
  "xfconf-query", "-c", "xfce4-panel",
  "-p", "/plugins/plugin-#{plugin_id}",
  "-n", "-t", "string", "-s", "launcher"
) or abort "install_textui_xfce_launcher: plugin type failed"

items_out = IO.popen(
  [
    "xfconf-query", "-c", "xfce4-panel",
    "-p", "/plugins/plugin-#{plugin_id}/items",
    "-n", "-a", "-t", "string", "-s", stamp
  ],
  err: [:child, :out],
  &:read
)
unless $?.success?
  system(
    "xfconf-query", "-c", "xfce4-panel",
    "-p", "/plugins/plugin-#{plugin_id}/items",
    "-t", "string", "-s", stamp
  ) or abort "install_textui_xfce_launcher: plugin items failed: #{items_out}"
end

raw_ids = `xfconf-query -c xfce4-panel -p /panels/panel-0/plugin-ids`
abort "install_textui_xfce_launcher: read plugin-ids failed" unless $?.success?
ids = raw_ids.lines.map(&:strip).grep(/^\d+$/).map(&:to_i)
ids.delete(plugin_id)
insert_at = ids.index(ruby_playground_plugin)
abort "install_textui_xfce_launcher: ruby playground plugin #{ruby_playground_plugin} missing" if insert_at.nil?
ids.insert(insert_at + 1, plugin_id)
id_args = ids.flat_map { |id| ["-t", "int", "-s", id.to_s] }
system("xfconf-query", "-c", "xfce4-panel", "-p", "/panels/panel-0/plugin-ids", *id_args) or
  abort "install_textui_xfce_launcher: rewrite plugin-ids failed"

system("gtk-update-icon-cache", "-f", File.join(Dir.home, ".local/share/icons/hicolor"))
system("update-desktop-database", applications_dir)
system("xfce4-panel", "-r") or abort "install_textui_xfce_launcher: xfce4-panel restart failed"

puts "MLC textui launcher: plugin-#{plugin_id} after plugin-#{ruby_playground_plugin} (#{stamp})"

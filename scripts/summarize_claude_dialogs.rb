#!/usr/bin/env ruby
# frozen_string_literal: true

# Summarizes Claude conversation JSON files into a Markdown report.
# Usage:
#   ruby scripts/summarize_claude_dialogs.rb docs/claude_*.json > docs/CLAUDE_DIALOG_SUMMARY.md
#
# The script stays intentionally simple: it extracts metadata, counts, and
# renders a chronological message log with text/thinking/tool info flattened
# into readable bullet points.

require 'json'
require 'time'

def read_conv(path)
  JSON.parse(File.read(path))
rescue JSON::ParserError => e
  warn "Failed to parse #{path}: #{e.message}"
  exit 1
end

def fmt_ts(ts)
  return '' unless ts
  Time.parse(ts).utc.iso8601 rescue ts
end

def collect_text_chunks(msg)
  chunks = []
  chunks << msg['text'] if msg['text'] && !msg['text'].empty?

  Array(msg['content']).each do |c|
    case c['type']
    when 'text'
      chunks << c['text']
    when 'thinking'
      chunks << "[thinking] #{c['thinking']}"
    when 'tool_use'
      desc = "[tool_use: #{c['name']}]"
      desc += " #{c['message']}" if c['message'] && !c['message'].empty?
      chunks << desc
    when 'tool_result'
      inner = Array(c['content']).filter_map { |part| part['text'] if part.is_a?(Hash) }
      joined = inner.empty? ? '' : " #{inner.join(' ')}"
      chunks << "[tool_result: #{c['name']}]#{joined}"
    when 'token_budget'
      chunks << '[token_budget]'
    else
      # ignore unknown types silently
    end
  end

  chunks.compact.map(&:strip).reject(&:empty?)
end

def analyze_conv(conv)
  msgs = Array(conv['chat_messages'])
  senders = msgs.each_with_object(Hash.new(0)) { |m, h| h[m['sender']] += 1 }
  content_type_counts = Hash.new(0)
  attachments = []
  msgs.each do |m|
    Array(m['content']).each { |c| content_type_counts[c['type']] += 1 if c['type'] }
    attachments.concat(Array(m['attachments']))
  end
  {
    name: conv['name'],
    uuid: conv['uuid'],
    created: conv['created_at'],
    updated: conv['updated_at'],
    message_count: msgs.size,
    senders: senders,
    content_types: content_type_counts,
    attachments: attachments
  }
end

def render_conv(path, conv)
  meta = analyze_conv(conv)
  msgs = Array(conv['chat_messages'])

  puts "## #{meta[:name] || 'Untitled'}"
  puts "- Source file: `#{path}`"
  puts "- UUID: `#{meta[:uuid]}`"
  puts "- Created: #{fmt_ts(meta[:created])}"
  puts "- Updated: #{fmt_ts(meta[:updated])}"
  puts "- Messages: #{meta[:message_count]}"
  puts "- Senders: #{meta[:senders].map { |k, v| "#{k}: #{v}" }.join(', ')}"
  unless meta[:content_types].empty?
    puts "- Content types: #{meta[:content_types].map { |k, v| "#{k}: #{v}" }.join(', ')}"
  end
  unless meta[:attachments].empty?
    list = meta[:attachments].map { |a| a['file_name'] || 'attachment' }
    puts "- Attachments: #{list.join(', ')}"
  end
  puts
  puts "### Messages"
  msgs.each do |m|
    t = fmt_ts(m['created_at'])
    sender = m['sender'] || 'unknown'
    body_chunks = collect_text_chunks(m)
    body = body_chunks.empty? ? '(no text)' : body_chunks.join(' ')
    puts "- [#{sender} @ #{t}] #{body}"
  end
  puts
end

if ARGV.empty?
  warn 'Usage: ruby scripts/summarize_claude_dialogs.rb <conversation.json>...'
  exit 1
end

ARGV.each do |path|
  conv = read_conv(path)
  render_conv(path, conv)
end

#!/usr/bin/env ruby
# frozen_string_literal: true

# Migrates non-interpolated double-quoted string literals to single-quoted form in
# compiler/**/*.mlc (excluding compiler/out). Skips decoded length-1 strings (char vs string).

module F1StringMigrate
  module_function

  def extract_mlcc_double(source, start_index)
    return unless source[start_index] == '"'

    inner_start = start_index + 1
    index = inner_start
    while index < source.length
      if source[index] == '\\' && index + 1 < source.length
        index += 2
        next
      end
      if source[index] == '"'
        inner = source.byteslice(inner_start, index - inner_start)
        return [index + 1, inner]
      end
      index += 1
    end
    nil
  end

  def decode_mlcc_double(inner)
    output = +''
    index = 0
    while index < inner.length
      if inner[index] == '\\' && index + 1 < inner.length
        next_character = inner[index + 1]
        case next_character
        when 'n' then output << "\n"
        when 't' then output << "\t"
        when 'r' then output << "\r"
        when '"' then output << '"'
        when '\\' then output << '\\'
        when '0' then output << "\0"
        when '{' then output << '{'
        when '}' then output << '}'
        else
          output << '\\'
          output << next_character
        end
        index += 2
      else
        output << inner[index]
        index += 1
      end
    end
    output
  end

  def encode_mlcc_single(decoded)
    decoded.each_char.map do |character|
      case character
      when "'" then "\\'"
      when '\\' then '\\\\'
      when "\n" then '\\n'
      when "\t" then '\\t'
      when "\r" then '\\r'
      when "\0" then '\\0'
      else character
      end
    end.join
  end

  def skip_mlcc_single(source, start_index)
    return start_index + 1 unless source[start_index] == "'"

    index = start_index + 1
    while index < source.length
      if source[index] == '\\' && index + 1 < source.length
        index += 2
        next
      end
      return index + 1 if source[index] == "'"

      index += 1
    end
    source.length
  end

  def skip_mlcc_double_inner_for_template(source, start_index)
    return start_index + 1 unless source[start_index] == '"'

    index = start_index + 1
    while index < source.length
      if source[index] == '\\' && index + 1 < source.length
        index += 2
        next
      end
      return index + 1 if source[index] == '"'

      index += 1
    end
    source.length
  end

  def skip_template_mlcc(source, start_index)
    return start_index + 1 unless source[start_index] == '`'

    index = start_index + 1
    while index < source.length
      character = source[index]
      if character == '\\' && index + 1 < source.length
        index += 2
        next
      end
      return index + 1 if character == '`'

      if character == '$' && index + 1 < source.length && source[index + 1] == '{'
        index += 2
        depth = 1
        while index < source.length && depth.positive?
          inner_character = source[index]
          if inner_character == '{'
            depth += 1
            index += 1
          elsif inner_character == '}'
            depth -= 1
            index += 1
          elsif inner_character == '"'
            index = skip_mlcc_double_inner_for_template(source, index)
          else
            index += 1
          end
        end
        next
      end

      index += 1
    end
    source.length
  end

  def migrate_source(source)
    output = +''
    index = 0
    while index < source.length
      if source[index, 2] == '//'
        newline_index = source.index("\n", index)
        unless newline_index
          output << source.byteslice(index..)
          break
        end
        output << source.byteslice(index..newline_index)
        index = newline_index + 1
        next
      end

      if source[index, 2] == '/*'
        closing_index = source.index('*/', index + 2)
        unless closing_index
          output << source.byteslice(index..)
          break
        end
        output << source.byteslice(index..closing_index + 1)
        index = closing_index + 2
        next
      end

      character = source[index]
      case character
      when '"'
        closing_quote_index, inner_between_double_quotes = extract_mlcc_double(source, index)
        unless closing_quote_index
          output << source.byteslice(index..)
          break
        end
        decoded = decode_mlcc_double(inner_between_double_quotes)
        replacement =
          if decoded.length == 1 || decoded.include?('${')
            source.byteslice(index, closing_quote_index - index)
          else
            "'#{encode_mlcc_single(decoded)}'"
          end
        output << replacement
        index = closing_quote_index
      when "'"
        after_quote_index = skip_mlcc_single(source, index)
        output << source.byteslice(index, after_quote_index - index)
        index = after_quote_index
      when '`'
        after_template_index = skip_template_mlcc(source, index)
        output << source.byteslice(index, after_template_index - index)
        index = after_template_index
      else
        output << character
        index += 1
      end
    end
    output
  end

  def migrate_file(path)
    original = File.binread(path)
    migrated = migrate_source(original)
    return :unchanged if migrated == original

    File.binwrite(path, migrated)
    :changed
  end
end

if $PROGRAM_NAME == __FILE__
  root = File.expand_path('..', __dir__)
  paths = Dir.glob(File.join(root, 'compiler/**/*.mlc')).reject { |path| path.include?("#{File::SEPARATOR}out#{File::SEPARATOR}") }
  changed = paths.count { |path| F1StringMigrate.migrate_file(path) == :changed }
  puts "f1_migrate_compiler_string_quotes: updated #{changed} files"
end

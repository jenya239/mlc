# frozen_string_literal: true

require "test_helper"

class MlccExtendTest < Minitest::Test
  PREAMBLE = <<~MLC
    type TKind = Eof | Ident(string) | LInt(i32) | LStr(string)
              | KFn | KType | KLet | KMut | KConst | KIf | KUnless | KElse | KEnd
              | KDo | KWhile | KReturn | KBreak | KContinue | KTrue | KFalse
              | KMatch | KFor | KExtern | KExtend | KThen
              | Equal | Bar | Pipe | Colon | Comma | Dot | Spread | Arrow | FatArrow
              | Question | LParen | RParen | LBrace | RBrace | LBracket | RBracket
              | Semicolon | Op(string)

    type Token = Token { kind: TKind, line: i32 }
    type Parser = Parser { tokens: [Token], pos: i32 }

    extend TKind {
      fn is_eof(self)    -> bool = match self { Eof     => true, _ => false }
      fn is_if(self)     -> bool = match self { KIf     => true, _ => false }
      fn is_unless(self) -> bool = match self { KUnless => true, _ => false }
      fn is_else(self)   -> bool = match self { KElse   => true, _ => false }
      fn is_end(self)    -> bool = match self { KEnd    => true, _ => false }
      fn is_do(self)     -> bool = match self { KDo     => true, _ => false }
      fn is_comma(self)  -> bool = match self { Comma   => true, _ => false }
      fn is_pipe(self)   -> bool = match self { Pipe    => true, _ => false }
      fn is_ident(self)  -> bool = match self { Ident(_) => true, _ => false }
      fn get_ident(self) -> string = match self { Ident(name) => name, _ => "" }
    }

    extend Parser {
      fn kind(self)      -> TKind  = tokens[pos].kind
      fn advance(self)   -> Parser = Parser { tokens, pos: pos + 1 }
      fn advance_by(self, count: i32) -> Parser = Parser { tokens, pos: pos + count }
      fn at_eof(self)    -> bool   = match tokens[pos].kind { Eof => true, _ => false }
      fn skip_semi(self) -> Parser =
        match tokens[pos].kind { Semicolon => self.advance(), _ => self }
    }

    fn make_parser(kinds: [TKind]) -> Parser = do
      let tokens: [Token] = []
      let index = 0
      while index < kinds.length() do
        tokens.push(Token { kind: kinds[index], line: 1 })
        index = index + 1
      end
      Parser { tokens, pos: 0 }
    end
  MLC

  def compile(source)
    MLC.to_cpp(source)
  end

  def test_extend_parser_kind_is_if
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([KIf, Eof])
        parser.kind().is_if()
      end
    MLC
    assert_includes cpp, "Parser_kind("
    assert_includes cpp, "TKind_is_if("
  end

  def test_extend_parser_at_eof
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([Eof])
        parser.at_eof()
      end
    MLC
    assert_includes cpp, "Parser_at_eof("
  end

  def test_extend_parser_advance_then_eof
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([KIf, Eof])
        parser.advance().at_eof()
      end
    MLC
    assert_includes cpp, "Parser_advance("
    assert_includes cpp, "Parser_at_eof("
  end

  def test_extend_parser_skip_semi
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([Semicolon, Eof])
        parser.skip_semi().at_eof()
      end
    MLC
    assert_includes cpp, "Parser_skip_semi("
  end

  def test_extend_parser_chain_kind_is_unless
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([KUnless, Eof])
        parser.kind().is_unless()
      end
    MLC
    assert_includes cpp, "TKind_is_unless("
  end

  def test_extend_tkind_get_ident
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> string = do
        const parser = make_parser([Ident("hello"), Eof])
        parser.kind().get_ident()
      end
    MLC
    assert_includes cpp, "TKind_get_ident("
  end

  def test_record_shorthand_in_parser_advance
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> i32 = do
        const parser = make_parser([KIf, KElse, Eof])
        const advanced = parser.advance()
        advanced.pos
      end
    MLC
    refute_nil cpp
  end
end

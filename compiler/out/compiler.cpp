#include "mlc/core/string.hpp"
#include "mlc/core/collections.hpp"
#include "mlc/core/hashmap.hpp"
#include "mlc/io/io.hpp"
#include "mlc/io/file.hpp"
#include "mlc/core/match.hpp"
#include "mlc/core/result.hpp"
struct KFn;
struct KType;
struct KLet;
struct KMut;
struct KConst;
struct KReturn;
struct KBreak;
struct KContinue;
struct KIf;
struct KThen;
struct KElse;
struct KUnless;
struct KWhile;
struct KFor;
struct KIn;
struct KDo;
struct KEnd;
struct KMatch;
struct KImport;
struct KFrom;
struct KAs;
struct KExtern;
struct KExtend;
struct KWhere;
struct KTrue;
struct KFalse;
struct LInt;
struct LStr;
struct Ident;
struct Arrow;
struct FatArrow;
struct Pipe;
struct Bar;
struct Equal;
struct Question;
struct Dot;
struct LParen;
struct RParen;
struct LBrace;
struct RBrace;
struct LBracket;
struct RBracket;
struct Comma;
struct Semicolon;
struct Colon;
struct Op;
struct Eof;
using TKind = std::variant<KFn, KType, KLet, KMut, KConst, KReturn, KBreak, KContinue, KIf, KThen, KElse, KUnless, KWhile, KFor, KIn, KDo, KEnd, KMatch, KImport, KFrom, KAs, KExtern, KExtend, KWhere, KTrue, KFalse, LInt, LStr, Ident, Arrow, FatArrow, Pipe, Bar, Equal, Question, Dot, LParen, RParen, LBrace, RBrace, LBracket, RBracket, Comma, Semicolon, Colon, Op, Eof>;
struct TyI32;
struct TyString;
struct TyBool;
struct TyUnit;
struct TyNamed;
struct TyArray;
struct TyShared;
struct TyGeneric;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor>;
struct ExprInt;
struct ExprStr;
struct ExprBool;
struct ExprUnit;
struct ExprIdent;
struct ExprBin;
struct ExprUn;
struct ExprCall;
struct ExprMethod;
struct ExprField;
struct ExprIndex;
struct ExprIf;
struct ExprBlock;
struct ExprWhile;
struct ExprFor;
struct ExprMatch;
struct ExprRecord;
struct ExprArray;
struct ExprQuestion;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprArray, ExprQuestion>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord>;
struct DeclFn;
struct DeclType;
using Decl = std::variant<DeclFn, DeclType>;
struct TI32;
struct TString;
struct TBool;
struct TUnit;
struct TArray;
struct TShared;
struct TNamed;
struct TFn;
struct TUnknown;
using Type = std::variant<TI32, TString, TBool, TUnit, TArray, TShared, TNamed, TFn, TUnknown>;
struct CKAuto;
struct CKConst;
struct CKConstexpr;
struct CKIf;
struct CKElse;
struct CKWhile;
struct CKFor;
struct CKReturn;
struct CKBreak;
struct CKStruct;
struct CKUsing;
struct CKNoexcept;
struct CKInline;
struct CKStatic;
struct CKVoid;
struct CKInt;
struct CKBool;
struct CKChar;
struct CKTrue;
struct CKFalse;
struct CKNullptr;
struct CKTemplate;
struct CKTypename;
struct CKNamespace;
struct CKNew;
struct CKDelete;
struct CKOperator;
struct CIdent;
struct CLInt;
struct CLStr;
struct CArrow;
struct CScope;
struct CEq;
struct CNe;
struct CLe;
struct CGe;
struct CAnd;
struct COr;
struct CInc;
struct CDec;
struct CLShift;
struct CRShift;
struct CLParen;
struct CRParen;
struct CLBrace;
struct CRBrace;
struct CLBracket;
struct CRBracket;
struct CLAngle;
struct CRAngle;
struct COp;
struct CEof;
using CppTKind = std::variant<CKAuto, CKConst, CKConstexpr, CKIf, CKElse, CKWhile, CKFor, CKReturn, CKBreak, CKStruct, CKUsing, CKNoexcept, CKInline, CKStatic, CKVoid, CKInt, CKBool, CKChar, CKTrue, CKFalse, CKNullptr, CKTemplate, CKTypename, CKNamespace, CKNew, CKDelete, CKOperator, CIdent, CLInt, CLStr, CArrow, CScope, CEq, CNe, CLe, CGe, CAnd, COr, CInc, CDec, CLShift, CRShift, CLParen, CRParen, CLBrace, CRBrace, CLBracket, CRBracket, CLAngle, CRAngle, COp, CEof>;
struct KFn {};
struct KType {};
struct KLet {};
struct KMut {};
struct KConst {};
struct KReturn {};
struct KBreak {};
struct KContinue {};
struct KIf {};
struct KThen {};
struct KElse {};
struct KUnless {};
struct KWhile {};
struct KFor {};
struct KIn {};
struct KDo {};
struct KEnd {};
struct KMatch {};
struct KImport {};
struct KFrom {};
struct KAs {};
struct KExtern {};
struct KExtend {};
struct KWhere {};
struct KTrue {};
struct KFalse {};
struct LInt {int field0;};
struct LStr {mlc::String field0;};
struct Ident {mlc::String field0;};
struct Arrow {};
struct FatArrow {};
struct Pipe {};
struct Bar {};
struct Equal {};
struct Question {};
struct Dot {};
struct LParen {};
struct RParen {};
struct LBrace {};
struct RBrace {};
struct LBracket {};
struct RBracket {};
struct Comma {};
struct Semicolon {};
struct Colon {};
struct Op {mlc::String field0;};
struct Eof {};
struct Token {TKind kind;int line;int col;};
struct LexOut {mlc::Array<Token> tokens;mlc::Array<mlc::String> errors;};
struct TyI32 {};
struct TyString {};
struct TyBool {};
struct TyUnit {};
struct TyNamed {mlc::String field0;};
struct TyArray {std::shared_ptr<TypeExpr> field0;};
struct TyShared {std::shared_ptr<TypeExpr> field0;};
struct TyGeneric {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct FieldPat {mlc::String name;std::shared_ptr<Pat> pat;};
struct MatchArm {std::shared_ptr<Pat> pat;std::shared_ptr<Expr> body;};
struct FieldVal {mlc::String name;std::shared_ptr<Expr> val;};
struct ExprInt {int field0;};
struct ExprStr {mlc::String field0;};
struct ExprBool {bool field0;};
struct ExprUnit {};
struct ExprIdent {mlc::String field0;};
struct ExprBin {mlc::String field0;std::shared_ptr<Expr> field1;std::shared_ptr<Expr> field2;};
struct ExprUn {mlc::String field0;std::shared_ptr<Expr> field1;};
struct ExprCall {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<Expr>> field1;};
struct ExprMethod {std::shared_ptr<Expr> field0;mlc::String field1;mlc::Array<std::shared_ptr<Expr>> field2;};
struct ExprField {std::shared_ptr<Expr> field0;mlc::String field1;};
struct ExprIndex {std::shared_ptr<Expr> field0;std::shared_ptr<Expr> field1;};
struct ExprIf {std::shared_ptr<Expr> field0;std::shared_ptr<Expr> field1;std::shared_ptr<Expr> field2;};
struct ExprBlock {mlc::Array<std::shared_ptr<Stmt>> field0;std::shared_ptr<Expr> field1;};
struct ExprWhile {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<Stmt>> field1;};
struct ExprFor {mlc::String field0;std::shared_ptr<Expr> field1;mlc::Array<std::shared_ptr<Stmt>> field2;};
struct ExprMatch {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<MatchArm>> field1;};
struct ExprRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldVal>> field1;};
struct ExprArray {mlc::Array<std::shared_ptr<Expr>> field0;};
struct ExprQuestion {std::shared_ptr<Expr> field0;};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<Expr> field2;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
struct LexState {mlc::String src;int pos;int line;int col;};
struct ScanIdR {LexState st;Token tok;};
struct ScanIntR {LexState st;Token tok;};
struct ScanStrR {LexState st;Token tok;mlc::String err;};
struct ScanOpR {LexState st;Token tok;};
struct Parser {mlc::Array<Token> tokens;int pos;};
struct ExprResult {std::shared_ptr<Expr> expr;Parser parser;};
struct TypeResult {std::shared_ptr<TypeExpr> type_expr;Parser parser;};
struct PatResult {std::shared_ptr<Pat> pat;Parser parser;};
struct StmtResult {std::shared_ptr<Stmt> stmt;Parser parser;};
struct StmtsResult {mlc::Array<std::shared_ptr<Stmt>> stmts;Parser parser;};
struct ExprsResult {mlc::Array<std::shared_ptr<Expr>> exprs;Parser parser;};
struct ArmsResult {mlc::Array<std::shared_ptr<MatchArm>> arms;Parser parser;};
struct VariantResult {std::shared_ptr<TypeVariant> variant;Parser parser;};
struct VariantsResult {mlc::Array<std::shared_ptr<TypeVariant>> variants;Parser parser;};
struct ParamsResult {mlc::Array<std::shared_ptr<Param>> params;Parser parser;};
struct DeclResult {std::shared_ptr<Decl> decl;Parser parser;};
struct FieldValsResult {mlc::Array<std::shared_ptr<FieldVal>> field_vals;Parser parser;};
struct PatsResult {mlc::Array<std::shared_ptr<Pat>> pats;Parser parser;};
struct FieldDefsResult {mlc::Array<std::shared_ptr<FieldDef>> field_defs;Parser parser;};
struct TypesResult {mlc::Array<std::shared_ptr<TypeExpr>> types;Parser parser;};
struct ParamResult {std::shared_ptr<Param> param;Parser parser;};
struct NameCheckResult {mlc::Array<mlc::String> errors;mlc::Array<mlc::String> scope;};
struct TI32 {};
struct TString {};
struct TBool {};
struct TUnit {};
struct TArray {std::shared_ptr<Type> field0;};
struct TShared {std::shared_ptr<Type> field0;};
struct TNamed {mlc::String field0;};
struct TFn {mlc::Array<std::shared_ptr<Type>> field0;std::shared_ptr<Type> field1;};
struct TUnknown {};
struct TypeRegistry {mlc::HashMap<mlc::String, std::shared_ptr<Type>> fn_types;mlc::HashMap<mlc::String, std::shared_ptr<Type>> ctor_types;mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<Type>>> ctor_params;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<Type>>> field_types;};
struct InferResult {std::shared_ptr<Type> inferred_type;mlc::Array<mlc::String> errors;};
struct StmtInferResult {mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env;mlc::Array<mlc::String> errors;};
struct CheckOut {mlc::Array<mlc::String> errors;TypeRegistry registry;};
struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};
struct CKAuto {};
struct CKConst {};
struct CKConstexpr {};
struct CKIf {};
struct CKElse {};
struct CKWhile {};
struct CKFor {};
struct CKReturn {};
struct CKBreak {};
struct CKStruct {};
struct CKUsing {};
struct CKNoexcept {};
struct CKInline {};
struct CKStatic {};
struct CKVoid {};
struct CKInt {};
struct CKBool {};
struct CKChar {};
struct CKTrue {};
struct CKFalse {};
struct CKNullptr {};
struct CKTemplate {};
struct CKTypename {};
struct CKNamespace {};
struct CKNew {};
struct CKDelete {};
struct CKOperator {};
struct CIdent {mlc::String field0;};
struct CLInt {int field0;};
struct CLStr {mlc::String field0;};
struct CArrow {};
struct CScope {};
struct CEq {};
struct CNe {};
struct CLe {};
struct CGe {};
struct CAnd {};
struct COr {};
struct CInc {};
struct CDec {};
struct CLShift {};
struct CRShift {};
struct CLParen {};
struct CRParen {};
struct CLBrace {};
struct CRBrace {};
struct CLBracket {};
struct CRBracket {};
struct CLAngle {};
struct CRAngle {};
struct COp {mlc::String field0;};
struct CEof {};
struct CppToken {CppTKind kind;int line;int col;};
struct CppLexOut {mlc::Array<CppToken> tokens;mlc::Array<mlc::String> errors;};
struct CppScanIdR {LexState st;CppToken tok;};
struct CppScanIntR {LexState st;CppToken tok;};
struct CppScanStrR {LexState st;CppToken tok;mlc::String err;};
struct CppScanOpR {LexState st;CppToken tok;};
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
constexpr bool is_alpha(mlc::String c) noexcept;
constexpr bool is_digit(mlc::String c) noexcept;
constexpr bool is_alnum(mlc::String c) noexcept;
constexpr bool is_ws(mlc::String c) noexcept;
constexpr bool ls_eof(LexState st) noexcept;
mlc::String ls_cur(LexState st) noexcept;
mlc::String ls_peek(LexState st, int offset) noexcept;
LexState ls_advance(LexState st) noexcept;
LexState ls_advance_n(LexState st, int n) noexcept;
Token ls_tok(LexState st, TKind k) noexcept;
TKind keyword_kind(mlc::String s) noexcept;
ScanIdR scan_ident(LexState st) noexcept;
ScanIntR scan_int(LexState st) noexcept;
mlc::String map_escape(mlc::String c) noexcept;
ScanStrR scan_string(LexState st) noexcept;
LexState skip_ws(LexState st) noexcept;
ScanOpR scan_op(LexState st) noexcept;
LexOut tokenize(mlc::String src) noexcept;
Parser parser_new(mlc::Array<Token> tokens) noexcept;
TKind parser_kind(Parser parser) noexcept;
Parser parser_advance(Parser parser) noexcept;
Parser parser_advance_by(Parser parser, int count) noexcept;
bool parser_at_eof(Parser parser) noexcept;
Parser parser_skip_semi(Parser parser) noexcept;
bool is_keyword_fn(TKind kind) noexcept;
bool is_keyword_type(TKind kind) noexcept;
bool is_keyword_let(TKind kind) noexcept;
bool is_keyword_mut(TKind kind) noexcept;
bool is_keyword_if(TKind kind) noexcept;
bool is_keyword_unless(TKind kind) noexcept;
bool is_keyword_else(TKind kind) noexcept;
bool is_keyword_end(TKind kind) noexcept;
bool is_keyword_do(TKind kind) noexcept;
bool is_keyword_while(TKind kind) noexcept;
bool is_keyword_for(TKind kind) noexcept;
bool is_keyword_match(TKind kind) noexcept;
bool is_keyword_return(TKind kind) noexcept;
bool is_keyword_break(TKind kind) noexcept;
bool is_keyword_continue(TKind kind) noexcept;
bool is_keyword_true(TKind kind) noexcept;
bool is_keyword_false(TKind kind) noexcept;
bool is_keyword_extern(TKind kind) noexcept;
bool is_keyword_extend(TKind kind) noexcept;
bool is_ident(TKind kind) noexcept;
bool is_int_literal(TKind kind) noexcept;
bool is_str_literal(TKind kind) noexcept;
bool is_equal(TKind kind) noexcept;
bool is_bar(TKind kind) noexcept;
bool is_pipe(TKind kind) noexcept;
bool is_colon(TKind kind) noexcept;
bool is_comma(TKind kind) noexcept;
bool is_dot(TKind kind) noexcept;
bool is_question(TKind kind) noexcept;
bool is_left_paren(TKind kind) noexcept;
bool is_right_paren(TKind kind) noexcept;
bool is_left_brace(TKind kind) noexcept;
bool is_right_brace(TKind kind) noexcept;
bool is_left_bracket(TKind kind) noexcept;
bool is_right_bracket(TKind kind) noexcept;
bool is_operator(TKind kind) noexcept;
mlc::String get_ident(TKind kind) noexcept;
int get_int_literal(TKind kind) noexcept;
mlc::String get_str_literal(TKind kind) noexcept;
mlc::String get_operator(TKind kind) noexcept;
bool is_ctor_name(mlc::String name) noexcept;
TypeResult parse_type(Parser parser) noexcept;
TypesResult parse_type_args(Parser parser) noexcept;
PatResult parse_pat(Parser parser) noexcept;
PatsResult parse_pat_args(Parser parser) noexcept;
ExprResult parse_expr(Parser parser) noexcept;
ExprResult parse_pipe(Parser parser) noexcept;
ExprResult parse_or(Parser parser) noexcept;
ExprResult parse_and(Parser parser) noexcept;
constexpr bool is_cmp_op(mlc::String op) noexcept;
ExprResult parse_cmp(Parser parser) noexcept;
ExprResult parse_add(Parser parser) noexcept;
ExprResult parse_mul(Parser parser) noexcept;
ExprResult parse_unary(Parser parser) noexcept;
ExprResult parse_postfix(Parser parser) noexcept;
ExprsResult parse_arg_list(Parser parser) noexcept;
ExprResult parse_primary(Parser parser) noexcept;
ExprResult parse_array_lit(Parser parser) noexcept;
ExprResult parse_if_expr(Parser parser) noexcept;
ExprResult parse_block(Parser parser) noexcept;
std::shared_ptr<Expr> block_result(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept;
mlc::Array<std::shared_ptr<Stmt>> block_body(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept;
ExprResult parse_while_expr(Parser parser) noexcept;
ExprResult parse_for_expr(Parser parser) noexcept;
ExprResult parse_match_expr(Parser parser) noexcept;
ArmsResult parse_arms(Parser parser) noexcept;
ExprResult parse_record_expr(Parser parser, mlc::String record_name) noexcept;
StmtsResult parse_stmts_until_end(Parser parser) noexcept;
StmtsResult parse_stmts_until_else_end(Parser parser) noexcept;
StmtResult parse_stmt(Parser parser) noexcept;
DeclResult parse_decl(Parser parser) noexcept;
DeclResult parse_fn_decl(Parser parser) noexcept;
ParamsResult parse_params(Parser parser) noexcept;
ParamResult parse_param(Parser parser) noexcept;
DeclResult parse_type_decl(Parser parser) noexcept;
VariantsResult parse_variants(Parser parser) noexcept;
VariantResult parse_variant(Parser parser) noexcept;
TypesResult parse_tuple_types(Parser parser) noexcept;
FieldDefsResult parse_field_defs(Parser parser) noexcept;
Program parse_program(mlc::Array<Token> tokens) noexcept;
bool scope_has(mlc::Array<mlc::String> scope, mlc::String name) noexcept;
mlc::Array<mlc::String> pat_bindings(std::shared_ptr<Pat> pat) noexcept;
mlc::Array<mlc::String> collect_pat_binds(std::shared_ptr<Pat> pat, mlc::Array<mlc::String> acc) noexcept;
mlc::Array<mlc::String> collect_globals(Program prog) noexcept;
mlc::Array<mlc::String> check_names_expr(std::shared_ptr<Expr> expr, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept;
NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept;
TypeRegistry empty_registry() noexcept;
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<TypeExpr> type_expr) noexcept;
TypeRegistry build_registry(Program prog) noexcept;
TypeRegistry register_decl(TypeRegistry registry, std::shared_ptr<Decl> decl) noexcept;
TypeRegistry register_variant(TypeRegistry registry, mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept;
InferResult infer_ok(std::shared_ptr<Type> type_val) noexcept;
std::shared_ptr<Type> binary_op_type(mlc::String op, std::shared_ptr<Type> left_type) noexcept;
std::shared_ptr<Type> method_return_type(mlc::String method_name) noexcept;
std::shared_ptr<Type> field_lookup_type(std::shared_ptr<Type> obj_type, mlc::String field_name, TypeRegistry registry) noexcept;
InferResult infer_expr(std::shared_ptr<Expr> expr, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
StmtInferResult infer_stmt(std::shared_ptr<Stmt> stmt, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<Type>> env_for_pat(mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<Type>> env_for_pat_with_type(mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, std::shared_ptr<Type> type_val, TypeRegistry registry) noexcept;
CheckOut check(Program prog) noexcept;
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(Program prog) noexcept;
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept;
mlc::String lower_first(mlc::String name) noexcept;
mlc::String type_to_cpp(std::shared_ptr<TypeExpr> type_expr) noexcept;
mlc::String escape_str(mlc::String input) noexcept;
mlc::String gen_args(mlc::Array<std::shared_ptr<Expr>> args, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String map_method(mlc::String method_name) noexcept;
mlc::String map_builtin(mlc::String name) noexcept;
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_block_body(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_expr(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<mlc::String> field_order, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
std::shared_ptr<Expr> find_field_val(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::String name) noexcept;
mlc::String gen_match(std::shared_ptr<Expr> subject, mlc::Array<std::shared_ptr<MatchArm>> arms, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_arm(std::shared_ptr<MatchArm> arm, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_pat_header(std::shared_ptr<Pat> pat) noexcept;
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<Pat>> sub_pats) noexcept;
mlc::String gen_stmt(std::shared_ptr<Stmt> stmt, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_stmt_expr(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_fn_body(std::shared_ptr<Expr> body, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_params(mlc::Array<std::shared_ptr<Param>> params) noexcept;
mlc::String gen_type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_single_variant(mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_adt(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String variant_ctor_name(std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_variant_struct(std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type, std::shared_ptr<Expr> body, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type) noexcept;
mlc::String gen_decl(std::shared_ptr<Decl> decl, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_proto(std::shared_ptr<Decl> decl) noexcept;
mlc::String gen_program(Program prog) noexcept;
CppTKind cpp_keyword_kind(mlc::String s) noexcept;
CppScanIdR cpp_scan_ident(LexState st) noexcept;
CppScanIntR cpp_scan_int(LexState st) noexcept;
CppScanStrR cpp_scan_string(LexState st) noexcept;
CppScanOpR cpp_scan_op(LexState st) noexcept;
CppLexOut cpp_tokenize(mlc::String src) noexcept;
mlc::String compile_source(mlc::String src) noexcept;
mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept;
int main(int argc, char** argv) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
int i = 0;
while (i < src.size()){
{
dst.push_back(src[i]);
i = i + 1;
}
}
return dst;
}
constexpr bool is_alpha(mlc::String c) noexcept{return c >= mlc::String("a") && c <= mlc::String("z") || c >= mlc::String("A") && c <= mlc::String("Z") || c == mlc::String("_");}
constexpr bool is_digit(mlc::String c) noexcept{return c >= mlc::String("0") && c <= mlc::String("9");}
constexpr bool is_alnum(mlc::String c) noexcept{return is_alpha(c) || is_digit(c);}
constexpr bool is_ws(mlc::String c) noexcept{return c == mlc::String(" ") || c == mlc::String("\t") || c == mlc::String("\r") || c == mlc::String("\n");}
constexpr bool ls_eof(LexState st) noexcept{return st.pos >= st.src.length();}
mlc::String ls_cur(LexState st) noexcept{return ls_eof(st) ? mlc::String("\0") : st.src.char_at(st.pos);}
mlc::String ls_peek(LexState st, int offset) noexcept{
int p = st.pos + offset;
return p >= st.src.length() ? mlc::String("\0") : st.src.char_at(p);
}
LexState ls_advance(LexState st) noexcept{return ls_cur(st) == mlc::String("\n") ? LexState{st.src, st.pos + 1, st.line + 1, 1} : LexState{st.src, st.pos + 1, st.line, st.col + 1};}
LexState ls_advance_n(LexState st, int n) noexcept{
LexState s = std::move(st);
int i = 0;
while (i < n){
{
s = ls_advance(s);
i = i + 1;
}
}
return s;
}
Token ls_tok(LexState st, TKind k) noexcept{return Token{k, st.line, st.col};}
TKind keyword_kind(mlc::String s) noexcept{return s == mlc::String("fn") ? TKind(KFn{}) : TKind(s == mlc::String("type") ? TKind(KType{}) : TKind(s == mlc::String("let") ? TKind(KLet{}) : TKind(s == mlc::String("mut") ? TKind(KMut{}) : TKind(s == mlc::String("const") ? TKind(KConst{}) : TKind(s == mlc::String("return") ? TKind(KReturn{}) : TKind(s == mlc::String("break") ? TKind(KBreak{}) : TKind(s == mlc::String("continue") ? TKind(KContinue{}) : TKind(s == mlc::String("if") ? TKind(KIf{}) : TKind(s == mlc::String("then") ? TKind(KThen{}) : TKind(s == mlc::String("else") ? TKind(KElse{}) : TKind(s == mlc::String("unless") ? TKind(KUnless{}) : TKind(s == mlc::String("while") ? TKind(KWhile{}) : TKind(s == mlc::String("for") ? TKind(KFor{}) : TKind(s == mlc::String("in") ? TKind(KIn{}) : TKind(s == mlc::String("do") ? TKind(KDo{}) : TKind(s == mlc::String("end") ? TKind(KEnd{}) : TKind(s == mlc::String("match") ? TKind(KMatch{}) : TKind(s == mlc::String("import") ? TKind(KImport{}) : TKind(s == mlc::String("from") ? TKind(KFrom{}) : TKind(s == mlc::String("as") ? TKind(KAs{}) : TKind(s == mlc::String("extern") ? TKind(KExtern{}) : TKind(s == mlc::String("extend") ? TKind(KExtend{}) : TKind(s == mlc::String("where") ? TKind(KWhere{}) : TKind(s == mlc::String("true") ? TKind(KTrue{}) : TKind(s == mlc::String("false") ? TKind(KFalse{}) : TKind(Ident(s)))))))))))))))))))))))))));}
ScanIdR scan_ident(LexState st) noexcept{
int start = st.pos;
mlc::String src = st.src;
int tok_ln = st.line;
int tok_cl = st.col;
LexState s = std::move(st);
while (!ls_eof(s) && is_alnum(ls_cur(s))){
{
s = ls_advance(s);
}
}
mlc::String word = src.substring(start, s.pos - start);
return ScanIdR{s, Token{keyword_kind(word), tok_ln, tok_cl}};
}
ScanIntR scan_int(LexState st) noexcept{
int tok_ln = st.line;
int tok_cl = st.col;
LexState s = std::move(st);
int v = 0;
while (!ls_eof(s) && is_digit(ls_cur(s))){
{
v = v * 10 + ls_cur(s).to_i();
s = ls_advance(s);
}
}
return ScanIntR{s, Token{LInt(v), tok_ln, tok_cl}};
}
mlc::String map_escape(mlc::String c) noexcept{return c == mlc::String("n") ? mlc::String("\n") : c == mlc::String("t") ? mlc::String("\t") : c == mlc::String("r") ? mlc::String("\r") : c == mlc::String("\"") ? mlc::String("\"") : c == mlc::String("\\") ? mlc::String("\\") : c == mlc::String("0") ? mlc::String("\0") : mlc::String("");}
ScanStrR scan_string(LexState st) noexcept{
int tok_ln = st.line;
int tok_cl = st.col;
LexState s = ls_advance(st);
mlc::Array<mlc::String> parts = {};
mlc::String err = mlc::String("");
while (!ls_eof(s) && ls_cur(s) != mlc::String("\"")){
{
if (ls_cur(s) == mlc::String("\\")){
{
s = ls_advance(s);
mlc::String mapped = map_escape(ls_cur(s));
if (mapped != mlc::String("")){
{
parts.push_back(mapped);
}
} else {
{
parts.push_back(mlc::String("\\"));
parts.push_back(ls_cur(s));
}
}
s = ls_advance(s);
}
} else {
{
parts.push_back(ls_cur(s));
s = ls_advance(s);
}
}
}
}
if (ls_eof(s)){
{
err = mlc::String("unterminated string");
}
} else {
{
s = ls_advance(s);
}
}
return ScanStrR{s, Token{LStr(mlc::collections::join(parts, mlc::String(""))), tok_ln, tok_cl}, err};
}
LexState skip_ws(LexState st) noexcept{
LexState s = std::move(st);
while (!ls_eof(s)){
{
mlc::String c = ls_cur(s);
if (is_ws(c)){
{
s = ls_advance(s);
}
} else {
{
if (c == mlc::String("/") && ls_peek(s, 1) == mlc::String("/")){
s = ls_advance(ls_advance(s));
[&]() -> void { 
  while (!ls_eof(s) && ls_cur(s) != mlc::String("\n")){
{
s = ls_advance(s);
}
}
 }();
} else {
if (c == mlc::String("/") && ls_peek(s, 1) == mlc::String("*")){
s = ls_advance(ls_advance(s));
[&]() -> void { 
  while (!ls_eof(s)){
{
if (ls_cur(s) == mlc::String("*") && ls_peek(s, 1) == mlc::String("/")){
{
s = ls_advance(ls_advance(s));
break;
}
}
s = ls_advance(s);
}
}
 }();
} else {
break;
}
}
}
}
}
}
return s;
}
ScanOpR scan_op(LexState st) noexcept{
int tok_ln = st.line;
int tok_cl = st.col;
mlc::String c = ls_cur(st);
mlc::String c2 = ls_peek(st, 1);
return c == mlc::String("-") && c2 == mlc::String(">") ? ScanOpR{ls_advance_n(st, 2), Token{Arrow{}, tok_ln, tok_cl}} : c == mlc::String("=") && c2 == mlc::String(">") ? ScanOpR{ls_advance_n(st, 2), Token{FatArrow{}, tok_ln, tok_cl}} : c == mlc::String("|") && c2 == mlc::String(">") ? ScanOpR{ls_advance_n(st, 2), Token{Pipe{}, tok_ln, tok_cl}} : c == mlc::String("=") && c2 == mlc::String("=") ? ScanOpR{ls_advance_n(st, 2), Token{Op(mlc::String("==")), tok_ln, tok_cl}} : c == mlc::String("!") && c2 == mlc::String("=") ? ScanOpR{ls_advance_n(st, 2), Token{Op(mlc::String("!=")), tok_ln, tok_cl}} : c == mlc::String("<") && c2 == mlc::String("=") ? ScanOpR{ls_advance_n(st, 2), Token{Op(mlc::String("<=")), tok_ln, tok_cl}} : c == mlc::String(">") && c2 == mlc::String("=") ? ScanOpR{ls_advance_n(st, 2), Token{Op(mlc::String(">=")), tok_ln, tok_cl}} : c == mlc::String("&") && c2 == mlc::String("&") ? ScanOpR{ls_advance_n(st, 2), Token{Op(mlc::String("&&")), tok_ln, tok_cl}} : c == mlc::String("|") && c2 == mlc::String("|") ? ScanOpR{ls_advance_n(st, 2), Token{Op(mlc::String("||")), tok_ln, tok_cl}} : c == mlc::String("=") ? ScanOpR{ls_advance(st), Token{Equal{}, tok_ln, tok_cl}} : c == mlc::String("|") ? ScanOpR{ls_advance(st), Token{Bar{}, tok_ln, tok_cl}} : c == mlc::String("?") ? ScanOpR{ls_advance(st), Token{Question{}, tok_ln, tok_cl}} : c == mlc::String(".") ? ScanOpR{ls_advance(st), Token{Dot{}, tok_ln, tok_cl}} : c == mlc::String("(") ? ScanOpR{ls_advance(st), Token{LParen{}, tok_ln, tok_cl}} : c == mlc::String(")") ? ScanOpR{ls_advance(st), Token{RParen{}, tok_ln, tok_cl}} : c == mlc::String("{") ? ScanOpR{ls_advance(st), Token{LBrace{}, tok_ln, tok_cl}} : c == mlc::String("}") ? ScanOpR{ls_advance(st), Token{RBrace{}, tok_ln, tok_cl}} : c == mlc::String("[") ? ScanOpR{ls_advance(st), Token{LBracket{}, tok_ln, tok_cl}} : c == mlc::String("]") ? ScanOpR{ls_advance(st), Token{RBracket{}, tok_ln, tok_cl}} : c == mlc::String(",") ? ScanOpR{ls_advance(st), Token{Comma{}, tok_ln, tok_cl}} : c == mlc::String(";") ? ScanOpR{ls_advance(st), Token{Semicolon{}, tok_ln, tok_cl}} : c == mlc::String(":") ? ScanOpR{ls_advance(st), Token{Colon{}, tok_ln, tok_cl}} : ScanOpR{ls_advance(st), Token{Op(c), tok_ln, tok_cl}};
}
LexOut tokenize(mlc::String src) noexcept{
LexState st = LexState{src, 0, 1, 1};
mlc::Array<Token> tokens = {};
mlc::Array<mlc::String> errors = {};
while (!ls_eof(st)){
{
st = skip_ws(st);
if (ls_eof(st)){
{
break;
}
}
mlc::String c = ls_cur(st);
if (is_alpha(c)){
{
ScanIdR r = scan_ident(st);
tokens.push_back(r.tok);
st = r.st;
}
} else {
{
if (is_digit(c)){
ScanIntR r = scan_int(st);
tokens.push_back(r.tok);
st = r.st;
} else {
if (c == mlc::String("\"")){
ScanStrR r = scan_string(st);
tokens.push_back(r.tok);
st = r.st;
if (r.err != mlc::String("")){
errors.push_back(r.err);
}
} else {
ScanOpR r = scan_op(st);
tokens.push_back(r.tok);
st = r.st;
}
}
}
}
}
}
tokens.push_back(Token{Eof{}, st.line, st.col});
return LexOut{tokens, errors};
}
Parser parser_new(mlc::Array<Token> tokens) noexcept{return Parser{tokens, 0};}
TKind parser_kind(Parser parser) noexcept{return parser.tokens[parser.pos].kind;}
Parser parser_advance(Parser parser) noexcept{return Parser{parser.tokens, parser.pos + 1};}
Parser parser_advance_by(Parser parser, int count) noexcept{return Parser{parser.tokens, parser.pos + count};}
bool parser_at_eof(Parser parser) noexcept{return [&]() -> bool { if (std::holds_alternative<Eof>(parser_kind(parser))) {  return true; } return false; }();}
Parser parser_skip_semi(Parser parser) noexcept{return [&]() -> Parser { if (std::holds_alternative<Semicolon>(parser_kind(parser))) {  return parser_advance(parser); } return parser; }();}
bool is_keyword_fn(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KFn>(kind)) {  return true; } return false; }();}
bool is_keyword_type(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KType>(kind)) {  return true; } return false; }();}
bool is_keyword_let(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KLet>(kind)) {  return true; } return false; }();}
bool is_keyword_mut(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KMut>(kind)) {  return true; } return false; }();}
bool is_keyword_if(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KIf>(kind)) {  return true; } return false; }();}
bool is_keyword_unless(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KUnless>(kind)) {  return true; } return false; }();}
bool is_keyword_else(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KElse>(kind)) {  return true; } return false; }();}
bool is_keyword_end(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KEnd>(kind)) {  return true; } return false; }();}
bool is_keyword_do(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KDo>(kind)) {  return true; } return false; }();}
bool is_keyword_while(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KWhile>(kind)) {  return true; } return false; }();}
bool is_keyword_for(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KFor>(kind)) {  return true; } return false; }();}
bool is_keyword_match(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KMatch>(kind)) {  return true; } return false; }();}
bool is_keyword_return(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KReturn>(kind)) {  return true; } return false; }();}
bool is_keyword_break(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KBreak>(kind)) {  return true; } return false; }();}
bool is_keyword_continue(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KContinue>(kind)) {  return true; } return false; }();}
bool is_keyword_true(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KTrue>(kind)) {  return true; } return false; }();}
bool is_keyword_false(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KFalse>(kind)) {  return true; } return false; }();}
bool is_keyword_extern(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KExtern>(kind)) {  return true; } return false; }();}
bool is_keyword_extend(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KExtend>(kind)) {  return true; } return false; }();}
bool is_ident(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Ident>(kind)) { auto _v_ident = std::get<Ident>(kind); auto [_w0] = _v_ident; return true; } return false; }();}
bool is_int_literal(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LInt>(kind)) { auto _v_lint = std::get<LInt>(kind); auto [_w0] = _v_lint; return true; } return false; }();}
bool is_str_literal(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LStr>(kind)) { auto _v_lstr = std::get<LStr>(kind); auto [_w0] = _v_lstr; return true; } return false; }();}
bool is_equal(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Equal>(kind)) {  return true; } return false; }();}
bool is_bar(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Bar>(kind)) {  return true; } return false; }();}
bool is_pipe(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Pipe>(kind)) {  return true; } return false; }();}
bool is_colon(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Colon>(kind)) {  return true; } return false; }();}
bool is_comma(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Comma>(kind)) {  return true; } return false; }();}
bool is_dot(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Dot>(kind)) {  return true; } return false; }();}
bool is_question(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Question>(kind)) {  return true; } return false; }();}
bool is_left_paren(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LParen>(kind)) {  return true; } return false; }();}
bool is_right_paren(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<RParen>(kind)) {  return true; } return false; }();}
bool is_left_brace(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LBrace>(kind)) {  return true; } return false; }();}
bool is_right_brace(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<RBrace>(kind)) {  return true; } return false; }();}
bool is_left_bracket(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LBracket>(kind)) {  return true; } return false; }();}
bool is_right_bracket(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<RBracket>(kind)) {  return true; } return false; }();}
bool is_operator(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Op>(kind)) { auto _v_op = std::get<Op>(kind); auto [_w0] = _v_op; return true; } return false; }();}
mlc::String get_ident(TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<Ident>(kind)) { auto _v_ident = std::get<Ident>(kind); auto [name] = _v_ident; return name; } return mlc::String(""); }();}
int get_int_literal(TKind kind) noexcept{return [&]() { if (std::holds_alternative<LInt>(kind)) { auto _v_lint = std::get<LInt>(kind); auto [n] = _v_lint; return n; } return 0; }();}
mlc::String get_str_literal(TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<LStr>(kind)) { auto _v_lstr = std::get<LStr>(kind); auto [s] = _v_lstr; return s; } return mlc::String(""); }();}
mlc::String get_operator(TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<Op>(kind)) { auto _v_op = std::get<Op>(kind); auto [s] = _v_op; return s; } return mlc::String(""); }();}
bool is_ctor_name(mlc::String name) noexcept{return name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");}
TypeResult parse_type(Parser parser) noexcept{
TKind kind = parser_kind(parser);
return is_left_bracket(kind) ? [&]() -> TypeResult { 
  TypeResult inner = parse_type(parser_advance(parser));
  return TypeResult{std::make_shared<TypeExpr>(TyArray(inner.type_expr)), parser_advance(inner.parser)};
 }() : is_ident(kind) ? [&]() -> TypeResult { 
  mlc::String name = get_ident(kind);
  return name == mlc::String("i32") ? TypeResult{std::make_shared<TypeExpr>(TyI32{}), parser_advance(parser)} : name == mlc::String("string") ? TypeResult{std::make_shared<TypeExpr>(TyString{}), parser_advance(parser)} : name == mlc::String("bool") ? TypeResult{std::make_shared<TypeExpr>(TyBool{}), parser_advance(parser)} : name == mlc::String("unit") ? TypeResult{std::make_shared<TypeExpr>(TyUnit{}), parser_advance(parser)} : [&]() -> TypeResult { 
  Parser after = parser_advance(parser);
  return is_operator(parser_kind(after)) && get_operator(parser_kind(after)) == mlc::String("<") ? [&]() -> TypeResult { 
  TypesResult args = parse_type_args(parser_advance(after));
  std::shared_ptr<TypeExpr> type_expr = name == mlc::String("Shared") && args.types.size() == 1 ? std::make_shared<TypeExpr>(TyShared(args.types[0])) : std::make_shared<TypeExpr>(TyGeneric(name, args.types));
  return TypeResult{type_expr, args.parser};
 }() : TypeResult{std::make_shared<TypeExpr>(TyNamed(name)), after};
 }();
 }() : TypeResult{std::make_shared<TypeExpr>(TyUnit{}), parser_advance_by(parser, 2)};
}
TypesResult parse_type_args(Parser parser) noexcept{
mlc::Array<std::shared_ptr<TypeExpr>> types = {};
TypeResult first = parse_type(parser);
types.push_back(first.type_expr);
Parser state = first.parser;
while (is_comma(parser_kind(state))){
{
TypeResult next = parse_type(parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
Parser end_state = is_operator(parser_kind(state)) && get_operator(parser_kind(state)) == mlc::String(">") ? parser_advance(state) : state;
return TypesResult{types, end_state};
}
PatResult parse_pat(Parser parser) noexcept{
TKind kind = parser_kind(parser);
return is_ident(kind) ? [&]() -> PatResult { 
  mlc::String name = get_ident(kind);
  return name == mlc::String("_") ? PatResult{std::make_shared<Pat>(PatWild{}), parser_advance(parser)} : is_ctor_name(name) ? [&]() -> PatResult { 
  Parser after = parser_advance(parser);
  return is_left_paren(parser_kind(after)) ? [&]() -> PatResult { 
  PatsResult sub_pats = parse_pat_args(parser_advance(after));
  return PatResult{std::make_shared<Pat>(PatCtor(name, sub_pats.pats)), sub_pats.parser};
 }() : PatResult{std::make_shared<Pat>(PatCtor(name, {})), after};
 }() : PatResult{std::make_shared<Pat>(PatIdent(name)), parser_advance(parser)};
 }() : is_int_literal(kind) ? PatResult{std::make_shared<Pat>(PatInt(get_int_literal(kind))), parser_advance(parser)} : is_str_literal(kind) ? PatResult{std::make_shared<Pat>(PatStr(get_str_literal(kind))), parser_advance(parser)} : is_keyword_true(kind) ? PatResult{std::make_shared<Pat>(PatBool(true)), parser_advance(parser)} : is_keyword_false(kind) ? PatResult{std::make_shared<Pat>(PatBool(false)), parser_advance(parser)} : PatResult{std::make_shared<Pat>(PatWild{}), parser_advance(parser)};
}
PatsResult parse_pat_args(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Pat>> pats = {};
return is_right_paren(parser_kind(parser)) ? PatsResult{pats, parser_advance(parser)} : [&]() -> PatsResult { 
  PatResult first = parse_pat(parser);
  pats.push_back(first.pat);
  Parser state = first.parser;
  while (is_comma(parser_kind(state))){
{
PatResult next = parse_pat(parser_advance(state));
pats.push_back(next.pat);
state = next.parser;
}
}
  return PatsResult{pats, parser_advance(state)};
 }();
}
ExprResult parse_expr(Parser parser) noexcept{return parse_pipe(parser);}
ExprResult parse_pipe(Parser parser) noexcept{
ExprResult left = parse_or(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
while (is_pipe(parser_kind(state))){
{
ExprResult right = parse_or(parser_advance(state));
mlc::Array<std::shared_ptr<Expr>> call_args = {};
call_args.push_back(expr);
expr = std::make_shared<Expr>(ExprCall(right.expr, call_args));
state = right.parser;
}
}
return ExprResult{expr, state};
}
ExprResult parse_or(Parser parser) noexcept{
ExprResult left = parse_and(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
while (is_operator(parser_kind(state)) && get_operator(parser_kind(state)) == mlc::String("||")){
{
ExprResult right = parse_and(parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(mlc::String("||"), expr, right.expr));
state = right.parser;
}
}
return ExprResult{expr, state};
}
ExprResult parse_and(Parser parser) noexcept{
ExprResult left = parse_cmp(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
while (is_operator(parser_kind(state)) && get_operator(parser_kind(state)) == mlc::String("&&")){
{
ExprResult right = parse_cmp(parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(mlc::String("&&"), expr, right.expr));
state = right.parser;
}
}
return ExprResult{expr, state};
}
constexpr bool is_cmp_op(mlc::String op) noexcept{return op == mlc::String("==") || op == mlc::String("!=") || op == mlc::String("<") || op == mlc::String(">") || op == mlc::String("<=") || op == mlc::String(">=");}
ExprResult parse_cmp(Parser parser) noexcept{
ExprResult left = parse_add(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
TKind kind = parser_kind(state);
if (is_operator(kind) && is_cmp_op(get_operator(kind))){
{
ExprResult right = parse_add(parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(get_operator(kind), expr, right.expr));
state = right.parser;
}
}
return ExprResult{expr, state};
}
ExprResult parse_add(Parser parser) noexcept{
ExprResult left = parse_mul(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
bool go = true;
while (go){
{
TKind kind = parser_kind(state);
if (is_operator(kind) && get_operator(kind) == mlc::String("+") || get_operator(kind) == mlc::String("-")){
{
ExprResult right = parse_mul(parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(get_operator(kind), expr, right.expr));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return ExprResult{expr, state};
}
ExprResult parse_mul(Parser parser) noexcept{
ExprResult left = parse_unary(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
bool go = true;
while (go){
{
TKind kind = parser_kind(state);
if (is_operator(kind) && get_operator(kind) == mlc::String("*") || get_operator(kind) == mlc::String("/") || get_operator(kind) == mlc::String("%")){
{
ExprResult right = parse_unary(parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(get_operator(kind), expr, right.expr));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return ExprResult{expr, state};
}
ExprResult parse_unary(Parser parser) noexcept{
TKind kind = parser_kind(parser);
return is_operator(kind) && get_operator(kind) == mlc::String("!") || get_operator(kind) == mlc::String("-") ? [&]() -> ExprResult { 
  ExprResult inner = parse_unary(parser_advance(parser));
  return ExprResult{std::make_shared<Expr>(ExprUn(get_operator(kind), inner.expr)), inner.parser};
 }() : parse_postfix(parser);
}
ExprResult parse_postfix(Parser parser) noexcept{
ExprResult base = parse_primary(parser);
std::shared_ptr<Expr> expr = base.expr;
Parser state = base.parser;
bool go = true;
while (go){
{
TKind kind = parser_kind(state);
if (is_dot(kind)){
{
mlc::String field_name = get_ident(parser_kind(parser_advance(state)));
if (is_left_paren(parser_kind(parser_advance_by(state, 2)))){
ExprsResult args = parse_arg_list(parser_advance_by(state, 3));
expr = std::make_shared<Expr>(ExprMethod(expr, field_name, args.exprs));
state = args.parser;
} else {
expr = std::make_shared<Expr>(ExprField(expr, field_name));
state = parser_advance_by(state, 2);
}
}
} else {
{
if (is_left_paren(kind)){
ExprsResult args = parse_arg_list(parser_advance(state));
expr = std::make_shared<Expr>(ExprCall(expr, args.exprs));
state = args.parser;
} else {
if (is_left_bracket(kind)){
ExprResult index = parse_expr(parser_advance(state));
expr = std::make_shared<Expr>(ExprIndex(expr, index.expr));
state = parser_advance(index.parser);
} else {
if (is_question(kind)){
expr = std::make_shared<Expr>(ExprQuestion(expr));
state = parser_advance(state);
} else {
go = false;
}
}
}
}
}
}
}
return ExprResult{expr, state};
}
ExprsResult parse_arg_list(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Expr>> exprs = {};
return is_right_paren(parser_kind(parser)) ? ExprsResult{exprs, parser_advance(parser)} : [&]() -> ExprsResult { 
  ExprResult first = parse_expr(parser);
  exprs.push_back(first.expr);
  Parser state = first.parser;
  while (is_comma(parser_kind(state))){
{
ExprResult next = parse_expr(parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
}
  return ExprsResult{exprs, parser_advance(state)};
 }();
}
ExprResult parse_primary(Parser parser) noexcept{
TKind kind = parser_kind(parser);
return is_int_literal(kind) ? ExprResult{std::make_shared<Expr>(ExprInt(get_int_literal(kind))), parser_advance(parser)} : is_str_literal(kind) ? ExprResult{std::make_shared<Expr>(ExprStr(get_str_literal(kind))), parser_advance(parser)} : is_keyword_true(kind) ? ExprResult{std::make_shared<Expr>(ExprBool(true)), parser_advance(parser)} : is_keyword_false(kind) ? ExprResult{std::make_shared<Expr>(ExprBool(false)), parser_advance(parser)} : is_left_paren(kind) ? is_right_paren(parser_kind(parser_advance(parser))) ? ExprResult{std::make_shared<Expr>(ExprUnit{}), parser_advance_by(parser, 2)} : [&]() -> ExprResult { 
  ExprResult inner = parse_expr(parser_advance(parser));
  return ExprResult{inner.expr, parser_advance(inner.parser)};
 }() : is_left_bracket(kind) ? parse_array_lit(parser_advance(parser)) : is_keyword_if(kind) || is_keyword_unless(kind) ? parse_if_expr(parser) : is_keyword_do(kind) ? parse_block(parser_advance(parser)) : is_keyword_while(kind) ? parse_while_expr(parser_advance(parser)) : is_keyword_for(kind) ? parse_for_expr(parser_advance(parser)) : is_keyword_match(kind) ? parse_match_expr(parser_advance(parser)) : is_keyword_return(kind) ? [&]() -> ExprResult { 
  ExprResult value = parse_expr(parser_advance(parser));
  mlc::Array<std::shared_ptr<Stmt>> stmts = {};
  stmts.push_back(std::make_shared<Stmt>(StmtReturn(value.expr)));
  return ExprResult{std::make_shared<Expr>(ExprBlock(stmts, std::make_shared<Expr>(ExprUnit{}))), value.parser};
 }() : is_ident(kind) ? [&]() -> ExprResult { 
  mlc::String name = get_ident(kind);
  return is_ctor_name(name) && is_left_brace(parser_kind(parser_advance(parser))) ? parse_record_expr(parser_advance_by(parser, 2), name) : ExprResult{std::make_shared<Expr>(ExprIdent(name)), parser_advance(parser)};
 }() : ExprResult{std::make_shared<Expr>(ExprUnit{}), parser_advance(parser)};
}
ExprResult parse_array_lit(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Expr>> exprs = {};
return is_right_bracket(parser_kind(parser)) ? ExprResult{std::make_shared<Expr>(ExprArray(exprs)), parser_advance(parser)} : [&]() -> ExprResult { 
  ExprResult first = parse_expr(parser);
  exprs.push_back(first.expr);
  Parser state = first.parser;
  while (is_comma(parser_kind(state))){
{
ExprResult next = parse_expr(parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
}
  return ExprResult{std::make_shared<Expr>(ExprArray(exprs)), parser_advance(state)};
 }();
}
ExprResult parse_if_expr(Parser parser) noexcept{
bool is_negated = is_keyword_unless(parser_kind(parser));
ExprResult cond_result = parse_expr(parser_advance(parser));
std::shared_ptr<Expr> cond = is_negated ? std::make_shared<Expr>(ExprUn(mlc::String("!"), cond_result.expr)) : cond_result.expr;
StmtsResult then_result = parse_stmts_until_else_end(parser_advance(cond_result.parser));
std::shared_ptr<Expr> then_expr = std::make_shared<Expr>(ExprBlock(then_result.stmts, std::make_shared<Expr>(ExprUnit{})));
return is_keyword_else(parser_kind(then_result.parser)) ? [&]() -> ExprResult { 
  Parser after_else = parser_advance(then_result.parser);
  return is_keyword_if(parser_kind(after_else)) || is_keyword_unless(parser_kind(after_else)) ? [&]() -> ExprResult { 
  ExprResult else_result = parse_if_expr(after_else);
  return ExprResult{std::make_shared<Expr>(ExprIf(cond, then_expr, else_result.expr)), else_result.parser};
 }() : [&]() -> ExprResult { 
  StmtsResult else_stmts = parse_stmts_until_end(after_else);
  std::shared_ptr<Expr> else_expr = std::make_shared<Expr>(ExprBlock(else_stmts.stmts, std::make_shared<Expr>(ExprUnit{})));
  return ExprResult{std::make_shared<Expr>(ExprIf(cond, then_expr, else_expr)), else_stmts.parser};
 }();
 }() : ExprResult{std::make_shared<Expr>(ExprIf(cond, then_expr, std::make_shared<Expr>(ExprUnit{}))), parser_advance(then_result.parser)};
}
ExprResult parse_block(Parser parser) noexcept{
StmtsResult result = parse_stmts_until_end(parser);
std::shared_ptr<Expr> tail = block_result(result.stmts);
return ExprResult{std::make_shared<Expr>(ExprBlock(block_body(result.stmts), tail)), result.parser};
}
std::shared_ptr<Expr> block_result(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept{return stmts.size() == 0 ? std::make_shared<Expr>(ExprUnit{}) : [&]() -> std::shared_ptr<Expr> { if (std::holds_alternative<StmtExpr>((*stmts[stmts.size() - 1]))) { auto _v_stmtexpr = std::get<StmtExpr>((*stmts[stmts.size() - 1])); auto [expr] = _v_stmtexpr; return expr; } return std::make_shared<Expr>(ExprUnit{}); }();}
mlc::Array<std::shared_ptr<Stmt>> block_body(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept{
return stmts.size() == 0 ? stmts : [&]() -> mlc::Array<std::shared_ptr<Stmt>> { if (std::holds_alternative<StmtExpr>((*stmts[stmts.size() - 1]))) { auto _v_stmtexpr = std::get<StmtExpr>((*stmts[stmts.size() - 1])); auto [_w0] = _v_stmtexpr; return [&]() -> mlc::Array<std::shared_ptr<Stmt>> { 
  mlc::Array<std::shared_ptr<Stmt>> result = {};
  int i = 0;
  while (i < stmts.size() - 1){
{
result.push_back(stmts[i]);
i = i + 1;
}
}
  return result;
 }(); } return stmts; }();
}
ExprResult parse_while_expr(Parser parser) noexcept{
ExprResult cond = parse_expr(parser);
StmtsResult body = parse_stmts_until_end(parser_advance(cond.parser));
return ExprResult{std::make_shared<Expr>(ExprWhile(cond.expr, body.stmts)), body.parser};
}
ExprResult parse_for_expr(Parser parser) noexcept{
mlc::String var_name = get_ident(parser_kind(parser));
ExprResult iter = parse_expr(parser_advance_by(parser, 2));
StmtsResult body = parse_stmts_until_end(parser_advance(iter.parser));
return ExprResult{std::make_shared<Expr>(ExprFor(var_name, iter.expr, body.stmts)), body.parser};
}
ExprResult parse_match_expr(Parser parser) noexcept{
ExprResult subject = parse_expr(parser);
ArmsResult arms = parse_arms(parser_advance(subject.parser));
return ExprResult{std::make_shared<Expr>(ExprMatch(subject.expr, arms.arms)), arms.parser};
}
ArmsResult parse_arms(Parser parser) noexcept{
mlc::Array<std::shared_ptr<MatchArm>> arms = {};
Parser state = std::move(parser);
while (!is_right_brace(parser_kind(state)) && !parser_at_eof(state)){
{
PatResult pat_result = parse_pat(state);
ExprResult body_result = parse_expr(parser_advance(pat_result.parser));
arms.push_back(std::make_shared<MatchArm>(MatchArm{pat_result.pat, body_result.expr}));
state = body_result.parser;
if (is_comma(parser_kind(state))){
{
state = parser_advance(state);
}
}
}
}
return ArmsResult{arms, parser_advance(state)};
}
ExprResult parse_record_expr(Parser parser, mlc::String record_name) noexcept{
mlc::Array<std::shared_ptr<FieldVal>> field_vals = {};
Parser state = std::move(parser);
while (!is_right_brace(parser_kind(state)) && !parser_at_eof(state)){
{
mlc::String field_name = get_ident(parser_kind(state));
ExprResult value_result = parse_expr(parser_advance_by(state, 2));
field_vals.push_back(std::make_shared<FieldVal>(FieldVal{field_name, value_result.expr}));
state = value_result.parser;
if (is_comma(parser_kind(state))){
{
state = parser_advance(state);
}
}
}
}
return ExprResult{std::make_shared<Expr>(ExprRecord(record_name, field_vals)), parser_advance(state)};
}
StmtsResult parse_stmts_until_end(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Stmt>> stmts = {};
Parser state = std::move(parser);
while (!is_keyword_end(parser_kind(state)) && !parser_at_eof(state)){
{
StmtResult result = parse_stmt(state);
stmts.push_back(result.stmt);
state = parser_skip_semi(result.parser);
}
}
return StmtsResult{stmts, parser_advance(state)};
}
StmtsResult parse_stmts_until_else_end(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Stmt>> stmts = {};
Parser state = std::move(parser);
while (!is_keyword_end(parser_kind(state)) && !is_keyword_else(parser_kind(state)) && !parser_at_eof(state)){
{
StmtResult result = parse_stmt(state);
stmts.push_back(result.stmt);
state = parser_skip_semi(result.parser);
}
}
return StmtsResult{stmts, state};
}
StmtResult parse_stmt(Parser parser) noexcept{
TKind kind = parser_kind(parser);
return is_keyword_let(kind) ? [&]() -> StmtResult { 
  bool is_mutable = is_keyword_mut(parser_kind(parser_advance(parser)));
  Parser name_pos = is_mutable ? parser_advance_by(parser, 2) : parser_advance(parser);
  mlc::String var_name = get_ident(parser_kind(name_pos));
  Parser value_pos = is_colon(parser_kind(parser_advance(name_pos))) ? parser_advance(parse_type(parser_advance_by(name_pos, 2)).parser) : parser_advance_by(name_pos, 2);
  ExprResult value_result = parse_expr(value_pos);
  return StmtResult{std::make_shared<Stmt>(StmtLet(var_name, is_mutable, value_result.expr)), value_result.parser};
 }() : is_keyword_break(kind) ? StmtResult{std::make_shared<Stmt>(StmtBreak{}), parser_advance(parser)} : is_keyword_continue(kind) ? StmtResult{std::make_shared<Stmt>(StmtContinue{}), parser_advance(parser)} : is_keyword_return(kind) ? [&]() -> StmtResult { 
  ExprResult value_result = parse_expr(parser_advance(parser));
  return StmtResult{std::make_shared<Stmt>(StmtReturn(value_result.expr)), value_result.parser};
 }() : [&]() -> StmtResult { 
  ExprResult expr_result = parse_expr(parser);
  return is_equal(parser_kind(expr_result.parser)) ? [&]() -> StmtResult { 
  ExprResult rhs = parse_expr(parser_advance(expr_result.parser));
  return StmtResult{std::make_shared<Stmt>(StmtExpr(std::make_shared<Expr>(ExprBin(mlc::String("="), expr_result.expr, rhs.expr)))), rhs.parser};
 }() : StmtResult{std::make_shared<Stmt>(StmtExpr(expr_result.expr)), expr_result.parser};
 }();
}
DeclResult parse_decl(Parser parser) noexcept{
TKind kind = parser_kind(parser);
return is_keyword_fn(kind) || is_keyword_extern(kind) && is_keyword_fn(parser_kind(parser_advance(parser))) ? parse_fn_decl(parser) : is_keyword_type(kind) ? parse_type_decl(parser_advance(parser)) : is_keyword_extend(kind) ? [&]() -> DeclResult { 
  Parser state = parser_advance(parser);
  while (!is_keyword_end(parser_kind(state)) && !parser_at_eof(state)){
{
state = parser_advance(state);
}
}
  return DeclResult{std::make_shared<Decl>(DeclFn(mlc::String("__ext__"), {}, std::make_shared<TypeExpr>(TyUnit{}), std::make_shared<Expr>(ExprUnit{}))), parser_advance(state)};
 }() : DeclResult{std::make_shared<Decl>(DeclFn(mlc::String("__skip__"), {}, std::make_shared<TypeExpr>(TyUnit{}), std::make_shared<Expr>(ExprUnit{}))), parser_advance(parser)};
}
DeclResult parse_fn_decl(Parser parser) noexcept{
Parser fn_start = is_keyword_extern(parser_kind(parser)) ? parser_advance(parser) : parser;
mlc::String fn_name = get_ident(parser_kind(parser_advance(fn_start)));
ParamsResult params_result = parse_params(parser_advance_by(fn_start, 3));
TypeResult ret_type_result = parse_type(parser_advance(params_result.parser));
ExprResult body_result = parse_expr(parser_advance(ret_type_result.parser));
return DeclResult{std::make_shared<Decl>(DeclFn(fn_name, params_result.params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}
ParamsResult parse_params(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Param>> params = {};
return is_right_paren(parser_kind(parser)) ? ParamsResult{params, parser_advance(parser)} : [&]() -> ParamsResult { 
  ParamResult first = parse_param(parser);
  params.push_back(first.param);
  Parser state = first.parser;
  while (is_comma(parser_kind(state))){
{
ParamResult next = parse_param(parser_advance(state));
params.push_back(next.param);
state = next.parser;
}
}
  return ParamsResult{params, parser_advance(state)};
 }();
}
ParamResult parse_param(Parser parser) noexcept{
bool is_mutable = is_keyword_mut(parser_kind(parser));
Parser name_pos = is_mutable ? parser_advance(parser) : parser;
TypeResult type_result = parse_type(parser_advance_by(name_pos, 2));
return ParamResult{std::make_shared<Param>(Param{get_ident(parser_kind(name_pos)), is_mutable, type_result.type_expr}), type_result.parser};
}
DeclResult parse_type_decl(Parser parser) noexcept{
mlc::String type_name = get_ident(parser_kind(parser));
Parser after_eq = parser_advance_by(parser, 2);
TKind first_kind = parser_kind(after_eq);
return is_ident(first_kind) && is_ctor_name(get_ident(first_kind)) && is_left_brace(parser_kind(parser_advance(after_eq))) ? [&]() -> DeclResult { 
  FieldDefsResult field_defs_result = parse_field_defs(parser_advance_by(after_eq, 2));
  mlc::Array<std::shared_ptr<TypeVariant>> variants = {};
  variants.push_back(std::make_shared<TypeVariant>(VarRecord(get_ident(first_kind), field_defs_result.field_defs)));
  return DeclResult{std::make_shared<Decl>(DeclType(type_name, variants)), field_defs_result.parser};
 }() : [&]() -> DeclResult { 
  VariantsResult variants_result = parse_variants(after_eq);
  return DeclResult{std::make_shared<Decl>(DeclType(type_name, variants_result.variants)), variants_result.parser};
 }();
}
VariantsResult parse_variants(Parser parser) noexcept{
mlc::Array<std::shared_ptr<TypeVariant>> variants = {};
Parser state = is_bar(parser_kind(parser)) ? parser_advance(parser) : parser;
while (!parser_at_eof(state) && is_bar(parser_kind(state)) || is_ident(parser_kind(state))){
{
VariantResult variant_result = parse_variant(state);
variants.push_back(variant_result.variant);
state = variant_result.parser;
if (is_bar(parser_kind(state))){
{
state = parser_advance(state);
}
}
}
}
return VariantsResult{variants, state};
}
VariantResult parse_variant(Parser parser) noexcept{
mlc::String variant_name = get_ident(parser_kind(parser));
Parser after_name = parser_advance(parser);
return is_left_paren(parser_kind(after_name)) ? [&]() -> VariantResult { 
  TypesResult types_result = parse_tuple_types(parser_advance(after_name));
  return VariantResult{std::make_shared<TypeVariant>(VarTuple(variant_name, types_result.types)), types_result.parser};
 }() : is_left_brace(parser_kind(after_name)) ? [&]() -> VariantResult { 
  FieldDefsResult field_defs_result = parse_field_defs(parser_advance(after_name));
  return VariantResult{std::make_shared<TypeVariant>(VarRecord(variant_name, field_defs_result.field_defs)), field_defs_result.parser};
 }() : VariantResult{std::make_shared<TypeVariant>(VarUnit(variant_name)), after_name};
}
TypesResult parse_tuple_types(Parser parser) noexcept{
mlc::Array<std::shared_ptr<TypeExpr>> types = {};
TypeResult first = parse_type(parser);
types.push_back(first.type_expr);
Parser state = first.parser;
while (is_comma(parser_kind(state))){
{
TypeResult next = parse_type(parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
return TypesResult{types, parser_advance(state)};
}
FieldDefsResult parse_field_defs(Parser parser) noexcept{
mlc::Array<std::shared_ptr<FieldDef>> field_defs = {};
Parser state = std::move(parser);
while (!is_right_brace(parser_kind(state)) && !parser_at_eof(state)){
{
mlc::String field_name = get_ident(parser_kind(state));
TypeResult type_result = parse_type(parser_advance_by(state, 2));
field_defs.push_back(std::make_shared<FieldDef>(FieldDef{field_name, type_result.type_expr}));
state = type_result.parser;
if (is_comma(parser_kind(state))){
{
state = parser_advance(state);
}
}
}
}
return FieldDefsResult{field_defs, parser_advance(state)};
}
Program parse_program(mlc::Array<Token> tokens) noexcept{
mlc::Array<std::shared_ptr<Decl>> decls = {};
Parser state = parser_new(tokens);
while (!parser_at_eof(state)){
{
DeclResult result = parse_decl(state);
decls.push_back(result.decl);
state = result.parser;
}
}
return Program{decls};
}
bool scope_has(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
bool found = false;
int i = 0;
while (i < scope.size()){
{
if (scope[i] == name){
{
found = true;
}
}
i = i + 1;
}
}
return found;
}
mlc::Array<mlc::String> pat_bindings(std::shared_ptr<Pat> pat) noexcept{
mlc::Array<mlc::String> acc = {};
return collect_pat_binds(pat, acc);
}
mlc::Array<mlc::String> collect_pat_binds(std::shared_ptr<Pat> pat, mlc::Array<mlc::String> acc) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<PatIdent>((*pat))) { auto _v_patident = std::get<PatIdent>((*pat)); auto [name] = _v_patident; return [&]() -> mlc::Array<mlc::String> { 
  acc.push_back(name);
  return acc;
 }(); } if (std::holds_alternative<PatCtor>((*pat))) { auto _v_patctor = std::get<PatCtor>((*pat)); auto [_w0, sub_pats] = _v_patctor; return [&]() -> mlc::Array<mlc::String> { 
  int i = 0;
  while (i < sub_pats.size()){
{
acc = collect_pat_binds(sub_pats[i], acc);
i = i + 1;
}
}
  return acc;
 }(); } return acc; }();
}
mlc::Array<mlc::String> collect_globals(Program prog) noexcept{
mlc::Array<mlc::String> names = {};
names.push_back(mlc::String("true"));
names.push_back(mlc::String("false"));
names.push_back(mlc::String("exit"));
names.push_back(mlc::String("print"));
names.push_back(mlc::String("println"));
names.push_back(mlc::String("args"));
names.push_back(mlc::String("File"));
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [name, _w0, _w1, _w2] = declfn; return names.push_back(name); },
  [&](const DeclType& decltype_) { auto [name, variants] = decltype_; return [&]() -> void { 
  names.push_back(name);
  int j = 0;
  return [&]() -> void { 
  while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [variant_name] = varunit; return names.push_back(variant_name); },
  [&](const VarTuple& vartuple) { auto [variant_name, _w0] = vartuple; return names.push_back(variant_name); },
  [&](const VarRecord& varrecord) { auto [variant_name, _w0] = varrecord; return names.push_back(variant_name); }
}, (*variants[j]));
j = j + 1;
}
}
 }();
 }(); }
}, (*prog.decls[i]));
i = i + 1;
}
}
return names;
}
mlc::Array<mlc::String> check_names_expr(std::shared_ptr<Expr> expr, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ExprIdent>((*expr))) { auto _v_exprident = std::get<ExprIdent>((*expr)); auto [name] = _v_exprident; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  if (!scope_has(locals, name) && !scope_has(globals, name)){
{
errors.push_back(mlc::String("undefined: ") + name);
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprBin>((*expr))) { auto _v_exprbin = std::get<ExprBin>((*expr)); auto [_w0, left, right] = _v_exprbin; return errs_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals)); } if (std::holds_alternative<ExprUn>((*expr))) { auto _v_exprun = std::get<ExprUn>((*expr)); auto [_w0, inner] = _v_exprun; return check_names_expr(inner, locals, globals); } if (std::holds_alternative<ExprCall>((*expr))) { auto _v_exprcall = std::get<ExprCall>((*expr)); auto [func, args] = _v_exprcall; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(func, locals, globals);
  int i = 0;
  while (i < args.size()){
{
errors = errs_append(errors, check_names_expr(args[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprMethod>((*expr))) { auto _v_exprmethod = std::get<ExprMethod>((*expr)); auto [obj, _w0, args] = _v_exprmethod; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(obj, locals, globals);
  int i = 0;
  while (i < args.size()){
{
errors = errs_append(errors, check_names_expr(args[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprField>((*expr))) { auto _v_exprfield = std::get<ExprField>((*expr)); auto [obj, _w0] = _v_exprfield; return check_names_expr(obj, locals, globals); } if (std::holds_alternative<ExprIndex>((*expr))) { auto _v_exprindex = std::get<ExprIndex>((*expr)); auto [obj, idx] = _v_exprindex; return errs_append(check_names_expr(obj, locals, globals), check_names_expr(idx, locals, globals)); } if (std::holds_alternative<ExprIf>((*expr))) { auto _v_exprif = std::get<ExprIf>((*expr)); auto [cond, then_expr, else_expr] = _v_exprif; return errs_append(errs_append(check_names_expr(cond, locals, globals), check_names_expr(then_expr, locals, globals)), check_names_expr(else_expr, locals, globals)); } if (std::holds_alternative<ExprBlock>((*expr))) { auto _v_exprblock = std::get<ExprBlock>((*expr)); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::Array<mlc::String> { 
  NameCheckResult check_result = check_names_stmts(stmts, locals, globals);
  return errs_append(check_result.errors, check_names_expr(result, check_result.scope, globals));
 }(); } if (std::holds_alternative<ExprWhile>((*expr))) { auto _v_exprwhile = std::get<ExprWhile>((*expr)); auto [cond, stmts] = _v_exprwhile; return [&]() -> mlc::Array<mlc::String> { 
  NameCheckResult check_result = check_names_stmts(stmts, locals, globals);
  return errs_append(check_names_expr(cond, locals, globals), check_result.errors);
 }(); } if (std::holds_alternative<ExprFor>((*expr))) { auto _v_exprfor = std::get<ExprFor>((*expr)); auto [var, iter, stmts] = _v_exprfor; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> inner = locals;
  inner.push_back(var);
  NameCheckResult check_result = check_names_stmts(stmts, inner, globals);
  return errs_append(check_names_expr(iter, locals, globals), check_result.errors);
 }(); } if (std::holds_alternative<ExprMatch>((*expr))) { auto _v_exprmatch = std::get<ExprMatch>((*expr)); auto [subject, arms] = _v_exprmatch; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(subject, locals, globals);
  int i = 0;
  while (i < arms.size()){
{
mlc::Array<mlc::String> binds = pat_bindings(arms[i]->pat);
mlc::Array<mlc::String> arm_scope = locals;
int j = 0;
while (j < binds.size()){
{
arm_scope.push_back(binds[j]);
j = j + 1;
}
}
errors = errs_append(errors, check_names_expr(arms[i]->body, arm_scope, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprRecord>((*expr))) { auto _v_exprrecord = std::get<ExprRecord>((*expr)); auto [_w0, field_vals] = _v_exprrecord; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  int i = 0;
  while (i < field_vals.size()){
{
errors = errs_append(errors, check_names_expr(field_vals[i]->val, locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprArray>((*expr))) { auto _v_exprarray = std::get<ExprArray>((*expr)); auto [exprs] = _v_exprarray; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  int i = 0;
  while (i < exprs.size()){
{
errors = errs_append(errors, check_names_expr(exprs[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprQuestion>((*expr))) { auto _v_exprquestion = std::get<ExprQuestion>((*expr)); auto [inner] = _v_exprquestion; return check_names_expr(inner, locals, globals); } return {}; }();
}
NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept{
mlc::Array<mlc::String> errors = {};
mlc::Array<mlc::String> scope = locals;
int i = 0;
while (i < stmts.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) { auto [name, _w0, value] = stmtlet; return [&]() -> void { 
  errors = errs_append(errors, check_names_expr(value, scope, globals));
  return scope.push_back(name);
 }(); },
  [&](const StmtExpr& stmtexpr) { auto [expr] = stmtexpr; return [&]() -> void { 
  errors = errs_append(errors, check_names_expr(expr, scope, globals));
 }(); },
  [&](const StmtReturn& stmtreturn) { auto [expr] = stmtreturn; return [&]() -> void { 
  errors = errs_append(errors, check_names_expr(expr, scope, globals));
 }(); },
  [&](const StmtBreak& stmtbreak) { return ; },
  [&](const StmtContinue& stmtcontinue) { return ; }
}, (*stmts[i]));
i = i + 1;
}
}
return NameCheckResult{errors, scope};
}
TypeRegistry empty_registry() noexcept{return TypeRegistry{mlc::HashMap<mlc::String, std::shared_ptr<Type>>(), mlc::HashMap<mlc::String, std::shared_ptr<Type>>(), mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<Type>>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<Type>>>()};}
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<TypeExpr> type_expr) noexcept{return [&]() -> std::shared_ptr<Type> { if (std::holds_alternative<TyI32>((*type_expr))) {  return std::make_shared<Type>(TI32{}); } if (std::holds_alternative<TyString>((*type_expr))) {  return std::make_shared<Type>(TString{}); } if (std::holds_alternative<TyBool>((*type_expr))) {  return std::make_shared<Type>(TBool{}); } if (std::holds_alternative<TyUnit>((*type_expr))) {  return std::make_shared<Type>(TUnit{}); } if (std::holds_alternative<TyNamed>((*type_expr))) { auto _v_tynamed = std::get<TyNamed>((*type_expr)); auto [name] = _v_tynamed; return std::make_shared<Type>(TNamed(name)); } if (std::holds_alternative<TyArray>((*type_expr))) { auto _v_tyarray = std::get<TyArray>((*type_expr)); auto [inner] = _v_tyarray; return std::make_shared<Type>(TArray(type_from_annotation(inner))); } if (std::holds_alternative<TyShared>((*type_expr))) { auto _v_tyshared = std::get<TyShared>((*type_expr)); auto [inner] = _v_tyshared; return std::make_shared<Type>(TShared(type_from_annotation(inner))); } return std::make_shared<Type>(TUnknown{}); }();}
TypeRegistry build_registry(Program prog) noexcept{
TypeRegistry registry = empty_registry();
int i = 0;
while (i < prog.decls.size()){
{
registry = register_decl(registry, prog.decls[i]);
i = i + 1;
}
}
return registry;
}
TypeRegistry register_decl(TypeRegistry registry, std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [name, params, return_type, _w0] = declfn; return [&]() -> TypeRegistry { 
  mlc::Array<std::shared_ptr<Type>> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_from_annotation(params[i]->typ));
i = i + 1;
}
}
  registry.fn_types.set(name, std::make_shared<Type>(TFn(param_types, type_from_annotation(return_type))));
  return registry;
 }(); },
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return [&]() -> TypeRegistry { 
  int i = 0;
  while (i < variants.size()){
{
registry = register_variant(registry, type_name, variants[i]);
i = i + 1;
}
}
  return registry;
 }(); }
}, (*decl));
}
TypeRegistry register_variant(TypeRegistry registry, mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept{
std::shared_ptr<Type> result_type = std::make_shared<Type>(TNamed(type_name));
return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [variant_name] = varunit; return [&]() -> TypeRegistry { 
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  return registry;
 }(); },
  [&](const VarTuple& vartuple) { auto [variant_name, field_types] = vartuple; return [&]() -> TypeRegistry { 
  mlc::Array<std::shared_ptr<Type>> field_type_list = {};
  int i = 0;
  while (i < field_types.size()){
{
field_type_list.push_back(type_from_annotation(field_types[i]));
i = i + 1;
}
}
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, field_type_list);
  return registry;
 }(); },
  [&](const VarRecord& varrecord) { auto [variant_name, field_defs] = varrecord; return [&]() -> TypeRegistry { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> field_map = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
  int i = 0;
  while (i < field_defs.size()){
{
field_map.set(field_defs[i]->name, type_from_annotation(field_defs[i]->typ));
i = i + 1;
}
}
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  registry.field_types.set(variant_name, field_map);
  registry.field_types.set(type_name, field_map);
  return registry;
 }(); }
}, (*variant));
}
InferResult infer_ok(std::shared_ptr<Type> type_val) noexcept{return InferResult{type_val, {}};}
std::shared_ptr<Type> binary_op_type(mlc::String op, std::shared_ptr<Type> left_type) noexcept{return op == mlc::String("+") || op == mlc::String("-") || op == mlc::String("*") || op == mlc::String("/") || op == mlc::String("%") ? [&]() { if (std::holds_alternative<TString>((*left_type))) {  return std::make_shared<Type>(TString{}); } return std::make_shared<Type>(TI32{}); }() : op == mlc::String("=") ? std::make_shared<Type>(TUnit{}) : std::make_shared<Type>(TBool{});}
std::shared_ptr<Type> method_return_type(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("size") || method_name == mlc::String("to_i") ? std::make_shared<Type>(TI32{}) : method_name == mlc::String("push") || method_name == mlc::String("set") ? std::make_shared<Type>(TUnit{}) : method_name == mlc::String("char_at") || method_name == mlc::String("join") || method_name == mlc::String("to_string") || method_name == mlc::String("substring") || method_name == mlc::String("to_lower") ? std::make_shared<Type>(TString{}) : method_name == mlc::String("has") ? std::make_shared<Type>(TBool{}) : std::make_shared<Type>(TUnknown{});}
std::shared_ptr<Type> field_lookup_type(std::shared_ptr<Type> obj_type, mlc::String field_name, TypeRegistry registry) noexcept{
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<TNamed>((*obj_type))) { auto _v_tnamed = std::get<TNamed>((*obj_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return type_name != mlc::String("") && registry.field_types.has(type_name) ? [&]() -> std::shared_ptr<Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> field_map = registry.field_types.get(type_name);
  return field_map.has(field_name) ? field_map.get(field_name) : std::make_shared<Type>(TUnknown{});
 }() : std::make_shared<Type>(TUnknown{});
}
InferResult infer_expr(std::shared_ptr<Expr> expr, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
return std::visit(overloaded{
  [&](const ExprInt& exprint) { auto [_w0] = exprint; return infer_ok(std::make_shared<Type>(TI32{})); },
  [&](const ExprStr& exprstr) { auto [_w0] = exprstr; return infer_ok(std::make_shared<Type>(TString{})); },
  [&](const ExprBool& exprbool) { auto [_w0] = exprbool; return infer_ok(std::make_shared<Type>(TBool{})); },
  [&](const ExprUnit& exprunit) { return infer_ok(std::make_shared<Type>(TUnit{})); },
  [&](const ExprIdent& exprident) { auto [name] = exprident; return type_env.has(name) ? infer_ok(type_env.get(name)) : registry.fn_types.has(name) ? infer_ok(registry.fn_types.get(name)) : registry.ctor_types.has(name) ? infer_ok(registry.ctor_types.get(name)) : infer_ok(std::make_shared<Type>(TUnknown{})); },
  [&](const ExprBin& exprbin) { auto [op, left, right] = exprbin; return [&]() -> InferResult { 
  InferResult left_result = infer_expr(left, type_env, registry);
  InferResult right_result = infer_expr(right, type_env, registry);
  return InferResult{binary_op_type(op, left_result.inferred_type), errs_append(left_result.errors, right_result.errors)};
 }(); },
  [&](const ExprUn& exprun) { auto [op, inner] = exprun; return [&]() -> InferResult { 
  InferResult inner_result = infer_expr(inner, type_env, registry);
  auto result_type = op == mlc::String("!") ? std::make_shared<Type>(TBool{}) : inner_result.inferred_type;
  return InferResult{result_type, inner_result.errors};
 }(); },
  [&](const ExprCall& exprcall) { auto [func, args] = exprcall; return [&]() -> InferResult { 
  InferResult func_result = infer_expr(func, type_env, registry);
  mlc::Array<mlc::String> errors = func_result.errors;
  int i = 0;
  while (i < args.size()){
{
InferResult arg_result = infer_expr(args[i], type_env, registry);
errors = errs_append(errors, arg_result.errors);
i = i + 1;
}
}
  std::shared_ptr<Type> return_type = [&]() -> std::shared_ptr<Type> { if (std::holds_alternative<TFn>((*func_result.inferred_type))) { auto _v_tfn = std::get<TFn>((*func_result.inferred_type)); auto [_w0, ret] = _v_tfn; return ret; } return std::make_shared<Type>(TUnknown{}); }();
  return InferResult{return_type, errors};
 }(); },
  [&](const ExprMethod& exprmethod) { auto [obj, method_name, args] = exprmethod; return [&]() -> InferResult { 
  InferResult obj_result = infer_expr(obj, type_env, registry);
  mlc::Array<mlc::String> errors = obj_result.errors;
  int i = 0;
  while (i < args.size()){
{
InferResult arg_result = infer_expr(args[i], type_env, registry);
errors = errs_append(errors, arg_result.errors);
i = i + 1;
}
}
  return InferResult{method_return_type(method_name), errors};
 }(); },
  [&](const ExprField& exprfield) { auto [obj, field_name] = exprfield; return [&]() -> InferResult { 
  InferResult obj_result = infer_expr(obj, type_env, registry);
  return InferResult{field_lookup_type(obj_result.inferred_type, field_name, registry), obj_result.errors};
 }(); },
  [&](const ExprIndex& exprindex) { auto [obj, idx] = exprindex; return [&]() -> InferResult { 
  InferResult obj_result = infer_expr(obj, type_env, registry);
  InferResult idx_result = infer_expr(idx, type_env, registry);
  std::shared_ptr<Type> elem_type = [&]() -> std::shared_ptr<Type> { if (std::holds_alternative<TArray>((*obj_result.inferred_type))) { auto _v_tarray = std::get<TArray>((*obj_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<Type>(TUnknown{}); }();
  return InferResult{elem_type, errs_append(obj_result.errors, idx_result.errors)};
 }(); },
  [&](const ExprIf& exprif) { auto [cond, then_expr, else_expr] = exprif; return [&]() -> InferResult { 
  InferResult cond_result = infer_expr(cond, type_env, registry);
  InferResult then_result = infer_expr(then_expr, type_env, registry);
  InferResult else_result = infer_expr(else_expr, type_env, registry);
  return InferResult{then_result.inferred_type, errs_append(errs_append(cond_result.errors, then_result.errors), else_result.errors)};
 }(); },
  [&](const ExprBlock& exprblock) { auto [stmts, result] = exprblock; return [&]() -> InferResult { 
  StmtInferResult stmts_result = infer_stmts(stmts, type_env, registry);
  InferResult result_infer = infer_expr(result, stmts_result.type_env, registry);
  return InferResult{result_infer.inferred_type, errs_append(stmts_result.errors, result_infer.errors)};
 }(); },
  [&](const ExprWhile& exprwhile) { auto [cond, stmts] = exprwhile; return [&]() -> InferResult { 
  InferResult cond_result = infer_expr(cond, type_env, registry);
  StmtInferResult stmts_result = infer_stmts(stmts, type_env, registry);
  return InferResult{std::make_shared<Type>(TUnit{}), errs_append(cond_result.errors, stmts_result.errors)};
 }(); },
  [&](const ExprFor& exprfor) { auto [var, iter, stmts] = exprfor; return [&]() -> InferResult { 
  InferResult iter_result = infer_expr(iter, type_env, registry);
  std::shared_ptr<Type> elem_type = [&]() -> std::shared_ptr<Type> { if (std::holds_alternative<TArray>((*iter_result.inferred_type))) { auto _v_tarray = std::get<TArray>((*iter_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<Type>(TUnknown{}); }();
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> inner_env = type_env;
  inner_env.set(var, elem_type);
  StmtInferResult stmts_result = infer_stmts(stmts, inner_env, registry);
  return InferResult{std::make_shared<Type>(TUnit{}), errs_append(iter_result.errors, stmts_result.errors)};
 }(); },
  [&](const ExprMatch& exprmatch) { auto [subject, arms] = exprmatch; return [&]() -> InferResult { 
  InferResult subject_result = infer_expr(subject, type_env, registry);
  mlc::Array<mlc::String> errors = subject_result.errors;
  std::shared_ptr<Type> arm_type = std::make_shared<Type>(TUnknown{});
  int i = 0;
  while (i < arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<Type>> arm_env = env_for_pat(type_env, arms[i]->pat, registry);
InferResult arm_result = infer_expr(arms[i]->body, arm_env, registry);
errors = errs_append(errors, arm_result.errors);
if (i == 0){
{
arm_type = arm_result.inferred_type;
}
}
i = i + 1;
}
}
  return InferResult{arm_type, errors};
 }(); },
  [&](const ExprRecord& exprrecord) { auto [name, field_vals] = exprrecord; return [&]() -> InferResult { 
  mlc::Array<mlc::String> errors = {};
  int i = 0;
  while (i < field_vals.size()){
{
InferResult field_result = infer_expr(field_vals[i]->val, type_env, registry);
errors = errs_append(errors, field_result.errors);
i = i + 1;
}
}
  return InferResult{std::make_shared<Type>(TNamed(name)), errors};
 }(); },
  [&](const ExprArray& exprarray) { auto [exprs] = exprarray; return [&]() -> InferResult { 
  mlc::Array<mlc::String> errors = {};
  std::shared_ptr<Type> elem_type = std::make_shared<Type>(TUnknown{});
  int i = 0;
  while (i < exprs.size()){
{
InferResult elem_result = infer_expr(exprs[i], type_env, registry);
errors = errs_append(errors, elem_result.errors);
if (i == 0){
{
elem_type = elem_result.inferred_type;
}
}
i = i + 1;
}
}
  return InferResult{std::make_shared<Type>(TArray(elem_type)), errors};
 }(); },
  [&](const ExprQuestion& exprquestion) { auto [inner] = exprquestion; return infer_expr(inner, type_env, registry); }
}, (*expr));
}
StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
mlc::Array<mlc::String> errors = {};
mlc::HashMap<mlc::String, std::shared_ptr<Type>> current_env = type_env;
int i = 0;
while (i < stmts.size()){
{
StmtInferResult result = infer_stmt(stmts[i], current_env, registry);
errors = errs_append(errors, result.errors);
current_env = result.type_env;
i = i + 1;
}
}
return StmtInferResult{current_env, errors};
}
StmtInferResult infer_stmt(std::shared_ptr<Stmt> stmt, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
return std::visit(overloaded{
  [&](const StmtLet& stmtlet) { auto [name, _w0, value] = stmtlet; return [&]() -> StmtInferResult { 
  InferResult value_result = infer_expr(value, type_env, registry);
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> new_env = type_env;
  new_env.set(name, value_result.inferred_type);
  return StmtInferResult{new_env, value_result.errors};
 }(); },
  [&](const StmtExpr& stmtexpr) { auto [expr] = stmtexpr; return [&]() -> StmtInferResult { 
  InferResult result = infer_expr(expr, type_env, registry);
  return StmtInferResult{type_env, result.errors};
 }(); },
  [&](const StmtReturn& stmtreturn) { auto [expr] = stmtreturn; return [&]() -> StmtInferResult { 
  InferResult result = infer_expr(expr, type_env, registry);
  return StmtInferResult{type_env, result.errors};
 }(); },
  [&](const StmtBreak& stmtbreak) { return StmtInferResult{type_env, {}}; },
  [&](const StmtContinue& stmtcontinue) { return StmtInferResult{type_env, {}}; }
}, (*stmt));
}
mlc::HashMap<mlc::String, std::shared_ptr<Type>> env_for_pat(mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { if (std::holds_alternative<PatIdent>((*pat))) { auto _v_patident = std::get<PatIdent>((*pat)); auto [name] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> env = type_env;
  env.set(name, std::make_shared<Type>(TUnknown{}));
  return env;
 }(); } if (std::holds_alternative<PatCtor>((*pat))) { auto _v_patctor = std::get<PatCtor>((*pat)); auto [ctor_name, sub_pats] = _v_patctor; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { 
  mlc::Array<std::shared_ptr<Type>> param_types = registry.ctor_params.has(ctor_name) ? registry.ctor_params.get(ctor_name) : mlc::Array<std::shared_ptr<Type>>{};
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> current_env = type_env;
  int i = 0;
  while (i < sub_pats.size()){
{
std::shared_ptr<Type> sub_type = i < param_types.size() ? param_types[i] : std::make_shared<Type>(TUnknown{});
current_env = env_for_pat_with_type(current_env, sub_pats[i], sub_type, registry);
i = i + 1;
}
}
  return current_env;
 }(); } return type_env; }();
}
mlc::HashMap<mlc::String, std::shared_ptr<Type>> env_for_pat_with_type(mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, std::shared_ptr<Type> type_val, TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { if (std::holds_alternative<PatIdent>((*pat))) { auto _v_patident = std::get<PatIdent>((*pat)); auto [name] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> env = type_env;
  env.set(name, type_val);
  return env;
 }(); } return env_for_pat(type_env, pat, registry); }();
}
CheckOut check(Program prog) noexcept{
mlc::Array<mlc::String> globals = collect_globals(prog);
TypeRegistry registry = build_registry(prog);
mlc::Array<mlc::String> all_errors = {};
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) {
auto [_w0, params, _w1, body] = declfn;
{
mlc::Array<mlc::String> locals = {};
mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
int j = 0;
while (j < params.size()){
{
locals.push_back(params[j]->name);
type_env.set(params[j]->name, type_from_annotation(params[j]->typ));
j = j + 1;
}
}
all_errors = errs_append(all_errors, check_names_expr(body, locals, globals));
InferResult infer_result = infer_expr(body, type_env, registry);
all_errors = errs_append(all_errors, infer_result.errors);
}
},
  [&](const DeclType& decltype_) {
auto [_w0, _w1] = decltype_;
{
}
}
}, (*prog.decls[i]));
i = i + 1;
}
}
return CheckOut{all_errors, registry};
}
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(Program prog) noexcept{
mlc::Array<std::shared_ptr<FieldOrder>> orders = {};
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return [&]() -> void { 
  int j = 0;
  return [&]() -> void { 
  while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) { auto [variant_name, field_defs] = varrecord; return [&]() -> void { 
  mlc::Array<mlc::String> field_names = {};
  int k = 0;
  while (k < field_defs.size()){
{
field_names.push_back(field_defs[k]->name);
k = k + 1;
}
}
  orders.push_back(std::make_shared<FieldOrder>(FieldOrder{variant_name, field_names}));
  if (variants.size() == 1){
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{type_name, field_names}));
}
 }(); },
  [&](const VarTuple& vartuple) { auto [_w0, _w1] = vartuple; return ; },
  [&](const VarUnit& varunit) { auto [_w0] = varunit; return ; }
}, (*variants[j]));
j = j + 1;
}
}
 }();
 }(); },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return ; }
}, (*prog.decls[i]));
i = i + 1;
}
}
return orders;
}
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
bool found = false;
while (i < orders.size() && !found){
{
if (orders[i]->type_name == name){
{
result = orders[i]->fields;
found = true;
}
}
i = i + 1;
}
}
return result;
}
mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}
mlc::String type_to_cpp(std::shared_ptr<TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) { return mlc::String("int"); },
  [&](const TyString& tystring) { return mlc::String("mlc::String"); },
  [&](const TyBool& tybool) { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) { return mlc::String("void"); },
  [&](const TyNamed& tynamed) { auto [name] = tynamed; return name; },
  [&](const TyArray& tyarray) { auto [inner] = tyarray; return mlc::String("mlc::Array<") + type_to_cpp(inner) + mlc::String(">"); },
  [&](const TyShared& tyshared) { auto [inner] = tyshared; return mlc::String("std::shared_ptr<") + type_to_cpp(inner) + mlc::String(">"); },
  [&](const TyGeneric& tygeneric) { auto [name, args] = tygeneric; return args.size() == 0 ? name : args.size() == 1 ? name + mlc::String("<") + type_to_cpp(args[0]) + mlc::String(">") : name + mlc::String("<") + type_to_cpp(args[0]) + mlc::String(", ") + type_to_cpp(args[1]) + mlc::String(">"); }
}, (*type_expr));}
mlc::String escape_str(mlc::String input) noexcept{
mlc::String output = mlc::String("");
int i = 0;
while (i < input.length()){
{
mlc::String c = input.char_at(i);
if (c == mlc::String("\\")){
{
output = output + mlc::String("\\\\");
}
} else {
{
if (c == mlc::String("\"")){
output = output + mlc::String("\\\"");
} else {
if (c == mlc::String("\n")){
output = output + mlc::String("\\n");
} else {
if (c == mlc::String("\t")){
output = output + mlc::String("\\t");
} else {
output = output + c;
}
}
}
}
}
i = i + 1;
}
}
return output;
}
mlc::String gen_args(mlc::Array<std::shared_ptr<Expr>> args, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < args.size()){
{
parts.push_back(gen_expr(args[i], field_orders));
i = i + 1;
}
}
return mlc::collections::join(parts, mlc::String(", "));
}
mlc::String map_method(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("len") ? mlc::String("size") : method_name == mlc::String("push") ? mlc::String("push_back") : method_name;}
mlc::String map_builtin(mlc::String name) noexcept{return name == mlc::String("print") ? mlc::String("mlc::io::print") : name == mlc::String("println") ? mlc::String("mlc::io::println") : name == mlc::String("exit") ? mlc::String("mlc::io::exit") : name == mlc::String("args") ? mlc::String("mlc::io::args") : name;}
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
mlc::String output = mlc::String("");
int i = 0;
while (i < stmts.size()){
{
output = output + gen_stmt(stmts[i], field_orders);
i = i + 1;
}
}
return output;
}
mlc::String gen_block_body(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ExprBlock>((*expr))) { auto _v_exprblock = std::get<ExprBlock>((*expr)); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, field_orders);
  mlc::String result_code = gen_expr(result, field_orders);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } return gen_expr(expr, field_orders) + mlc::String(";\n"); }();}
mlc::String gen_expr(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) { auto [n] = exprint; return mlc::to_string(n); },
  [&](const ExprStr& exprstr) { auto [s] = exprstr; return mlc::String("mlc::String(\"") + escape_str(s) + mlc::String("\")"); },
  [&](const ExprBool& exprbool) { auto [b] = exprbool; return b ? mlc::String("true") : mlc::String("false"); },
  [&](const ExprUnit& exprunit) { return mlc::String("/* unit */"); },
  [&](const ExprIdent& exprident) { auto [name] = exprident; return map_builtin(name); },
  [&](const ExprBin& exprbin) { auto [op, left, right] = exprbin; return mlc::String("(") + gen_expr(left, field_orders) + mlc::String(" ") + op + mlc::String(" ") + gen_expr(right, field_orders) + mlc::String(")"); },
  [&](const ExprUn& exprun) { auto [op, inner] = exprun; return mlc::String("(") + op + gen_expr(inner, field_orders) + mlc::String(")"); },
  [&](const ExprCall& exprcall) { auto [func, args] = exprcall; return gen_expr(func, field_orders) + mlc::String("(") + gen_args(args, field_orders) + mlc::String(")"); },
  [&](const ExprMethod& exprmethod) { auto [obj, method_name, args] = exprmethod; return [&]() -> mlc::String { 
  mlc::String obj_code = gen_expr(obj, field_orders);
  return obj_code == mlc::String("File") ? [&]() -> mlc::String { 
  mlc::String cpp_fn = method_name == mlc::String("read") ? mlc::String("mlc::file::read_to_string") : mlc::String("mlc::file::") + method_name;
  return cpp_fn + mlc::String("(") + gen_args(args, field_orders) + mlc::String(")");
 }() : [&]() -> mlc::String { 
  mlc::String base = obj_code + mlc::String(".") + map_method(method_name) + mlc::String("(");
  return args.size() == 0 ? base + mlc::String(")") : base + gen_args(args, field_orders) + mlc::String(")");
 }();
 }(); },
  [&](const ExprField& exprfield) { auto [obj, field_name] = exprfield; return gen_expr(obj, field_orders) + mlc::String(".") + field_name; },
  [&](const ExprIndex& exprindex) { auto [obj, idx] = exprindex; return gen_expr(obj, field_orders) + mlc::String("[") + gen_expr(idx, field_orders) + mlc::String("]"); },
  [&](const ExprIf& exprif) { auto [cond, then_expr, else_expr] = exprif; return mlc::String("(") + gen_expr(cond, field_orders) + mlc::String(" ? ") + gen_expr(then_expr, field_orders) + mlc::String(" : ") + gen_expr(else_expr, field_orders) + mlc::String(")"); },
  [&](const ExprBlock& exprblock) { auto [stmts, result] = exprblock; return [&]() -> mlc::String { 
  mlc::String body = gen_stmts_str(stmts, field_orders) + mlc::String("return ") + gen_expr(result, field_orders) + mlc::String(";\n");
  return mlc::String("[&]() {\n") + body + mlc::String("}()");
 }(); },
  [&](const ExprWhile& exprwhile) { auto [cond, stmts] = exprwhile; return mlc::String("[&]() {\nwhile (") + gen_expr(cond, field_orders) + mlc::String(") {\n") + gen_stmts_str(stmts, field_orders) + mlc::String("}\n}()"); },
  [&](const ExprFor& exprfor) { auto [var, iter, stmts] = exprfor; return mlc::String("[&]() {\nfor (auto ") + var + mlc::String(" : ") + gen_expr(iter, field_orders) + mlc::String(") {\n") + gen_stmts_str(stmts, field_orders) + mlc::String("}\n}()"); },
  [&](const ExprMatch& exprmatch) { auto [subject, arms] = exprmatch; return gen_match(subject, arms, field_orders); },
  [&](const ExprRecord& exprrecord) { auto [name, field_vals] = exprrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = lookup_fields(field_orders, name);
  mlc::String vals = field_order.size() > 0 ? gen_record_ordered(field_vals, field_order, field_orders) : gen_record_unordered(field_vals, field_orders);
  return name + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprArray& exprarray) { auto [exprs] = exprarray; return mlc::String("mlc::Array<auto>{") + gen_args(exprs, field_orders) + mlc::String("}"); },
  [&](const ExprQuestion& exprquestion) { auto [inner] = exprquestion; return gen_expr(inner, field_orders); }
}, (*expr));}
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<mlc::String> field_order, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_order.size()){
{
std::shared_ptr<Expr> value = find_field_val(field_vals, field_order[i]);
parts.push_back(gen_expr(value, field_orders));
i = i + 1;
}
}
return mlc::collections::join(parts, mlc::String(", "));
}
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_vals.size()){
{
parts.push_back(gen_expr(field_vals[i]->val, field_orders));
i = i + 1;
}
}
return mlc::collections::join(parts, mlc::String(", "));
}
std::shared_ptr<Expr> find_field_val(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::String name) noexcept{
std::shared_ptr<Expr> result = std::make_shared<Expr>(ExprUnit{});
int i = 0;
while (i < field_vals.size()){
{
if (field_vals[i]->name == name){
{
result = field_vals[i]->val;
}
}
i = i + 1;
}
}
return result;
}
mlc::String gen_match(std::shared_ptr<Expr> subject, mlc::Array<std::shared_ptr<MatchArm>> arms, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < arms.size()){
{
parts.push_back(gen_arm(arms[i], field_orders));
i = i + 1;
}
}
return mlc::String("std::visit(overloaded{") + mlc::collections::join(parts, mlc::String(",\n")) + mlc::String("\n}, ") + gen_expr(subject, field_orders) + mlc::String(")");
}
mlc::String gen_arm(std::shared_ptr<MatchArm> arm, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{return gen_pat_header(arm->pat) + mlc::String("{ return ") + gen_expr(arm->body, field_orders) + mlc::String("; }");}
mlc::String gen_pat_header(std::shared_ptr<Pat> pat) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) { return mlc::String("[&](auto& __v) "); },
  [&](const PatUnit& patunit) { return mlc::String("[&](auto& __v) "); },
  [&](const PatBool& patbool) { auto [_w0] = patbool; return mlc::String("[&](auto& __v) "); },
  [&](const PatInt& patint) { auto [_w0] = patint; return mlc::String("[&](auto& __v) "); },
  [&](const PatStr& patstr) { auto [_w0] = patstr; return mlc::String("[&](auto& __v) "); },
  [&](const PatIdent& patident) { auto [name] = patident; return mlc::String("[&](auto ") + name + mlc::String(") "); },
  [&](const PatCtor& patctor) { auto [name, sub_pats] = patctor; return [&]() -> mlc::String { 
  mlc::String lower_name = lower_first(name);
  mlc::String binding = sub_pats.size() == 0 ? mlc::String("") : mlc::String("{ auto [") + pat_bind_names(sub_pats) + mlc::String("] = ") + lower_name + mlc::String("; } ");
  return mlc::String("[&](const ") + name + mlc::String("& ") + lower_name + mlc::String(") ") + binding;
 }(); }
}, (*pat));}
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<Pat>> sub_pats) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < sub_pats.size()){
{
mlc::String bind_name = [&]() -> mlc::String { if (std::holds_alternative<PatIdent>((*sub_pats[i]))) { auto _v_patident = std::get<PatIdent>((*sub_pats[i])); auto [name] = _v_patident; return name; } return mlc::String("__") + mlc::to_string(i); }();
parts.push_back(bind_name);
i = i + 1;
}
}
return mlc::collections::join(parts, mlc::String(", "));
}
mlc::String gen_stmt(std::shared_ptr<Stmt> stmt, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) { auto [name, _w0, value] = stmtlet; return mlc::String("auto ") + name + mlc::String(" = ") + gen_expr(value, field_orders) + mlc::String(";\n"); },
  [&](const StmtExpr& stmtexpr) { auto [expr] = stmtexpr; return gen_stmt_expr(expr, field_orders); },
  [&](const StmtBreak& stmtbreak) { return mlc::String("break;\n"); },
  [&](const StmtContinue& stmtcontinue) { return mlc::String("continue;\n"); },
  [&](const StmtReturn& stmtreturn) { auto [expr] = stmtreturn; return mlc::String("return ") + gen_expr(expr, field_orders) + mlc::String(";\n"); }
}, (*stmt));}
mlc::String gen_stmt_expr(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ExprBin>((*expr))) { auto _v_exprbin = std::get<ExprBin>((*expr)); auto [op, lhs, rhs] = _v_exprbin; return op == mlc::String("=") ? gen_expr(lhs, field_orders) + mlc::String(" = ") + gen_expr(rhs, field_orders) + mlc::String(";\n") : mlc::String("(") + gen_expr(lhs, field_orders) + mlc::String(" ") + op + mlc::String(" ") + gen_expr(rhs, field_orders) + mlc::String(");\n"); } if (std::holds_alternative<ExprIf>((*expr))) { auto _v_exprif = std::get<ExprIf>((*expr)); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, field_orders) + mlc::String(") {\n") + gen_block_body(then_expr, field_orders) + mlc::String("}");
  mlc::String else_code = gen_expr(else_expr, field_orders);
  if (else_code != mlc::String("/* unit */")){
{
output = output + mlc::String(" else {\n") + gen_block_body(else_expr, field_orders) + mlc::String("}");
}
}
  return output + mlc::String("\n");
 }(); } if (std::holds_alternative<ExprWhile>((*expr))) { auto _v_exprwhile = std::get<ExprWhile>((*expr)); auto [cond, stmts] = _v_exprwhile; return mlc::String("while (") + gen_expr(cond, field_orders) + mlc::String(") {\n") + gen_stmts_str(stmts, field_orders) + mlc::String("}\n"); } if (std::holds_alternative<ExprFor>((*expr))) { auto _v_exprfor = std::get<ExprFor>((*expr)); auto [var, iter, stmts] = _v_exprfor; return mlc::String("for (auto ") + var + mlc::String(" : ") + gen_expr(iter, field_orders) + mlc::String(") {\n") + gen_stmts_str(stmts, field_orders) + mlc::String("}\n"); } if (std::holds_alternative<ExprBlock>((*expr))) { auto _v_exprblock = std::get<ExprBlock>((*expr)); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, field_orders);
  mlc::String result_code = gen_expr(result, field_orders);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } return gen_expr(expr, field_orders) + mlc::String(";\n"); }();}
mlc::String gen_fn_body(std::shared_ptr<Expr> body, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ExprBlock>((*body))) { auto _v_exprblock = std::get<ExprBlock>((*body)); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, field_orders);
  mlc::String result_code = gen_expr(result, field_orders);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + mlc::String("return ") + result_code + mlc::String(";\n");
 }(); } return mlc::String("return ") + gen_expr(body, field_orders) + mlc::String(";\n"); }();}
mlc::String gen_params(mlc::Array<std::shared_ptr<Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < params.size()){
{
parts.push_back(type_to_cpp(params[i]->typ) + mlc::String(" ") + params[i]->name);
i = i + 1;
}
}
return mlc::collections::join(parts, mlc::String(", "));
}
mlc::String gen_type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{return variants.size() == 1 ? gen_single_variant(type_name, variants[0]) : gen_adt(type_name, variants);}
mlc::String gen_single_variant(mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) { auto [_w0, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("struct ") + type_name + mlc::String(" {");
  int i = 0;
  while (i < field_defs.size()){
{
output = output + type_to_cpp(field_defs[i]->typ) + mlc::String(" ") + field_defs[i]->name + mlc::String(";");
i = i + 1;
}
}
  return output + mlc::String("};\n");
 }(); },
  [&](const VarTuple& vartuple) { auto [_w0, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("struct ") + type_name + mlc::String(" {");
  int i = 0;
  while (i < field_types.size()){
{
output = output + type_to_cpp(field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";");
i = i + 1;
}
}
  return output + mlc::String("};\n");
 }(); },
  [&](const VarUnit& varunit) { auto [_w0] = varunit; return mlc::String("struct ") + type_name + mlc::String(" {};\n"); }
}, (*variant));}
mlc::String gen_adt(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{
mlc::String output = mlc::String("");
mlc::Array<mlc::String> variant_names = {};
int i = 0;
while (i < variants.size()){
{
variant_names.push_back(variant_ctor_name(variants[i]));
output = output + gen_variant_struct(variants[i]);
i = i + 1;
}
}
return output + mlc::String("using ") + type_name + mlc::String(" = std::variant<") + mlc::collections::join(variant_names, mlc::String(", ")) + mlc::String(">;\n");
}
mlc::String variant_ctor_name(std::shared_ptr<TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [name] = varunit; return name; },
  [&](const VarTuple& vartuple) { auto [name, _w0] = vartuple; return name; },
  [&](const VarRecord& varrecord) { auto [name, _w0] = varrecord; return name; }
}, (*variant));}
mlc::String gen_variant_struct(std::shared_ptr<TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [name] = varunit; return mlc::String("struct ") + name + mlc::String(" {};\n"); },
  [&](const VarTuple& vartuple) { auto [name, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("struct ") + name + mlc::String(" {");
  int i = 0;
  while (i < field_types.size()){
{
output = output + type_to_cpp(field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";");
i = i + 1;
}
}
  return output + mlc::String("};\n");
 }(); },
  [&](const VarRecord& varrecord) { auto [name, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("struct ") + name + mlc::String(" {");
  int i = 0;
  while (i < field_defs.size()){
{
output = output + type_to_cpp(field_defs[i]->typ) + mlc::String(" ") + field_defs[i]->name + mlc::String(";");
i = i + 1;
}
}
  return output + mlc::String("};\n");
 }(); }
}, (*variant));}
mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type, std::shared_ptr<Expr> body, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{return name == mlc::String("main") && params.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");
  return type_to_cpp(return_type) + mlc::String(" ") + name + mlc::String("(int argc, char** argv) noexcept{\n") + preamble + gen_fn_body(body, field_orders) + mlc::String("}\n");
 }() : type_to_cpp(return_type) + mlc::String(" ") + name + mlc::String("(") + gen_params(params) + mlc::String(") noexcept{\n") + gen_fn_body(body, field_orders) + mlc::String("}\n");}
mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type) noexcept{return name == mlc::String("main") && params.size() == 0 ? type_to_cpp(return_type) + mlc::String(" ") + name + mlc::String("(int argc, char** argv) noexcept;\n") : type_to_cpp(return_type) + mlc::String(" ") + name + mlc::String("(") + gen_params(params) + mlc::String(") noexcept;\n");}
mlc::String gen_decl(std::shared_ptr<Decl> decl, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return gen_type_decl(type_name, variants); },
  [&](const DeclFn& declfn) { auto [name, params, return_type, body] = declfn; return gen_fn_decl(name, params, return_type, body, field_orders); }
}, (*decl));}
mlc::String gen_proto(std::shared_ptr<Decl> decl) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return mlc::String(""); },
  [&](const DeclFn& declfn) { auto [name, params, ret, _w0] = declfn; return gen_fn_proto(name, params, ret); }
}, (*decl));}
mlc::String gen_program(Program prog) noexcept{
mlc::Array<std::shared_ptr<FieldOrder>> field_orders = build_field_orders(prog);
mlc::String output = mlc::String("#include \"mlc/core/string.hpp\"\n");
output = output + mlc::String("#include \"mlc/core/array.hpp\"\n");
output = output + mlc::String("#include \"mlc/core/hashmap.hpp\"\n");
output = output + mlc::String("#include \"mlc/core/match.hpp\"\n");
output = output + mlc::String("#include \"mlc/io/io.hpp\"\n");
output = output + mlc::String("#include \"mlc/io/file.hpp\"\n");
output = output + mlc::String("#include <variant>\n");
output = output + mlc::String("#include <vector>\n\n");
int i = 0;
mlc::String s = mlc::String("");
while (i < prog.decls.size()){
{
s = std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return gen_decl(prog.decls[i], field_orders); },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return mlc::String(""); }
}, (*prog.decls[i]));
output = output + s;
i = i + 1;
}
}
i = 0;
while (i < prog.decls.size()){
{
s = std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return gen_proto(prog.decls[i]); },
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return mlc::String(""); }
}, (*prog.decls[i]));
output = output + s;
i = i + 1;
}
}
output = output + mlc::String("\n");
i = 0;
while (i < prog.decls.size()){
{
s = std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return gen_decl(prog.decls[i], field_orders); },
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return mlc::String(""); }
}, (*prog.decls[i]));
output = output + s;
i = i + 1;
}
}
return output;
}
CppTKind cpp_keyword_kind(mlc::String s) noexcept{return s == mlc::String("auto") ? CppTKind(CKAuto{}) : CppTKind(s == mlc::String("const") ? CppTKind(CKConst{}) : CppTKind(s == mlc::String("constexpr") ? CppTKind(CKConstexpr{}) : CppTKind(s == mlc::String("if") ? CppTKind(CKIf{}) : CppTKind(s == mlc::String("else") ? CppTKind(CKElse{}) : CppTKind(s == mlc::String("while") ? CppTKind(CKWhile{}) : CppTKind(s == mlc::String("for") ? CppTKind(CKFor{}) : CppTKind(s == mlc::String("return") ? CppTKind(CKReturn{}) : CppTKind(s == mlc::String("break") ? CppTKind(CKBreak{}) : CppTKind(s == mlc::String("struct") ? CppTKind(CKStruct{}) : CppTKind(s == mlc::String("using") ? CppTKind(CKUsing{}) : CppTKind(s == mlc::String("noexcept") ? CppTKind(CKNoexcept{}) : CppTKind(s == mlc::String("inline") ? CppTKind(CKInline{}) : CppTKind(s == mlc::String("static") ? CppTKind(CKStatic{}) : CppTKind(s == mlc::String("void") ? CppTKind(CKVoid{}) : CppTKind(s == mlc::String("int") ? CppTKind(CKInt{}) : CppTKind(s == mlc::String("bool") ? CppTKind(CKBool{}) : CppTKind(s == mlc::String("char") ? CppTKind(CKChar{}) : CppTKind(s == mlc::String("true") ? CppTKind(CKTrue{}) : CppTKind(s == mlc::String("false") ? CppTKind(CKFalse{}) : CppTKind(s == mlc::String("nullptr") ? CppTKind(CKNullptr{}) : CppTKind(s == mlc::String("template") ? CppTKind(CKTemplate{}) : CppTKind(s == mlc::String("typename") ? CppTKind(CKTypename{}) : CppTKind(s == mlc::String("namespace") ? CppTKind(CKNamespace{}) : CppTKind(s == mlc::String("new") ? CppTKind(CKNew{}) : CppTKind(s == mlc::String("delete") ? CppTKind(CKDelete{}) : CppTKind(s == mlc::String("operator") ? CppTKind(CKOperator{}) : CppTKind(CIdent(s))))))))))))))))))))))))))));}
CppScanIdR cpp_scan_ident(LexState st) noexcept{
int start = st.pos;
mlc::String src = st.src;
int tok_ln = st.line;
int tok_cl = st.col;
LexState s = std::move(st);
while (!ls_eof(s) && is_alnum(ls_cur(s))){
{
s = ls_advance(s);
}
}
mlc::String word = src.substring(start, s.pos - start);
return CppScanIdR{s, CppToken{cpp_keyword_kind(word), tok_ln, tok_cl}};
}
CppScanIntR cpp_scan_int(LexState st) noexcept{
int tok_ln = st.line;
int tok_cl = st.col;
LexState s = std::move(st);
int v = 0;
while (!ls_eof(s) && is_digit(ls_cur(s))){
{
v = v * 10 + ls_cur(s).to_i();
s = ls_advance(s);
}
}
return CppScanIntR{s, CppToken{CLInt(v), tok_ln, tok_cl}};
}
CppScanStrR cpp_scan_string(LexState st) noexcept{
int tok_ln = st.line;
int tok_cl = st.col;
LexState s = ls_advance(st);
mlc::Array<mlc::String> parts = {};
mlc::String err = mlc::String("");
while (!ls_eof(s) && ls_cur(s) != mlc::String("\"")){
{
if (ls_cur(s) == mlc::String("\\")){
{
s = ls_advance(s);
mlc::String mapped = map_escape(ls_cur(s));
if (mapped != mlc::String("")){
{
parts.push_back(mapped);
}
} else {
{
parts.push_back(mlc::String("\\"));
parts.push_back(ls_cur(s));
}
}
s = ls_advance(s);
}
} else {
{
parts.push_back(ls_cur(s));
s = ls_advance(s);
}
}
}
}
if (ls_eof(s)){
{
err = mlc::String("unterminated string");
}
} else {
{
s = ls_advance(s);
}
}
return CppScanStrR{s, CppToken{CLStr(mlc::collections::join(parts, mlc::String(""))), tok_ln, tok_cl}, err};
}
CppScanOpR cpp_scan_op(LexState st) noexcept{
int tok_ln = st.line;
int tok_cl = st.col;
mlc::String c = ls_cur(st);
mlc::String c2 = ls_peek(st, 1);
return c == mlc::String("-") && c2 == mlc::String(">") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CArrow{}, tok_ln, tok_cl}} : c == mlc::String(":") && c2 == mlc::String(":") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CScope{}, tok_ln, tok_cl}} : c == mlc::String("=") && c2 == mlc::String("=") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CEq{}, tok_ln, tok_cl}} : c == mlc::String("!") && c2 == mlc::String("=") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CNe{}, tok_ln, tok_cl}} : c == mlc::String("<") && c2 == mlc::String("=") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CLe{}, tok_ln, tok_cl}} : c == mlc::String(">") && c2 == mlc::String("=") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CGe{}, tok_ln, tok_cl}} : c == mlc::String("&") && c2 == mlc::String("&") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CAnd{}, tok_ln, tok_cl}} : c == mlc::String("|") && c2 == mlc::String("|") ? CppScanOpR{ls_advance_n(st, 2), CppToken{COr{}, tok_ln, tok_cl}} : c == mlc::String("+") && c2 == mlc::String("+") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CInc{}, tok_ln, tok_cl}} : c == mlc::String("-") && c2 == mlc::String("-") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CDec{}, tok_ln, tok_cl}} : c == mlc::String("<") && c2 == mlc::String("<") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CLShift{}, tok_ln, tok_cl}} : c == mlc::String(">") && c2 == mlc::String(">") ? CppScanOpR{ls_advance_n(st, 2), CppToken{CRShift{}, tok_ln, tok_cl}} : c == mlc::String("(") ? CppScanOpR{ls_advance(st), CppToken{CLParen{}, tok_ln, tok_cl}} : c == mlc::String(")") ? CppScanOpR{ls_advance(st), CppToken{CRParen{}, tok_ln, tok_cl}} : c == mlc::String("{") ? CppScanOpR{ls_advance(st), CppToken{CLBrace{}, tok_ln, tok_cl}} : c == mlc::String("}") ? CppScanOpR{ls_advance(st), CppToken{CRBrace{}, tok_ln, tok_cl}} : c == mlc::String("[") ? CppScanOpR{ls_advance(st), CppToken{CLBracket{}, tok_ln, tok_cl}} : c == mlc::String("]") ? CppScanOpR{ls_advance(st), CppToken{CRBracket{}, tok_ln, tok_cl}} : c == mlc::String("<") ? CppScanOpR{ls_advance(st), CppToken{CLAngle{}, tok_ln, tok_cl}} : c == mlc::String(">") ? CppScanOpR{ls_advance(st), CppToken{CRAngle{}, tok_ln, tok_cl}} : CppScanOpR{ls_advance(st), CppToken{COp(c), tok_ln, tok_cl}};
}
CppLexOut cpp_tokenize(mlc::String src) noexcept{
LexState st = LexState{src, 0, 1, 1};
mlc::Array<CppToken> tokens = {};
mlc::Array<mlc::String> errors = {};
while (!ls_eof(st)){
{
st = skip_ws(st);
if (ls_eof(st)){
{
break;
}
}
mlc::String c = ls_cur(st);
if (is_alpha(c)){
{
CppScanIdR r = cpp_scan_ident(st);
tokens.push_back(r.tok);
st = r.st;
}
} else {
{
if (is_digit(c)){
CppScanIntR r = cpp_scan_int(st);
tokens.push_back(r.tok);
st = r.st;
} else {
if (c == mlc::String("\"")){
CppScanStrR r = cpp_scan_string(st);
tokens.push_back(r.tok);
st = r.st;
if (r.err != mlc::String("")){
errors.push_back(r.err);
}
} else {
CppScanOpR r = cpp_scan_op(st);
tokens.push_back(r.tok);
st = r.st;
}
}
}
}
}
}
tokens.push_back(CppToken{CEof{}, st.line, st.col});
return CppLexOut{tokens, errors};
}
mlc::String compile_source(mlc::String src) noexcept{
LexOut lex = tokenize(src);
return lex.errors.size() > 0 ? format_errs(mlc::String("lex"), lex.errors) : [&]() -> mlc::String { 
  Program prog = parse_program(lex.tokens);
  CheckOut chk = check(prog);
  return chk.errors.size() > 0 ? format_errs(mlc::String("check"), chk.errors) : gen_program(prog);
 }();
}
mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept{
mlc::String output = mlc::String("");
int i = 0;
while (i < errors.size()){
{
output = output + label + mlc::String(": ") + errors[i] + mlc::String("\n");
i = i + 1;
}
}
return output;
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::Array<mlc::String> cli_args = mlc::io::args();
if (cli_args.size() == 0){
{
mlc::io::println(mlc::String("Usage: mlcc <source.mlc>"));
mlc::io::exit(1);
}
}
mlc::String src = mlc::file::read_to_string(cli_args[0]);
mlc::io::print(compile_source(src));
return 0;
}

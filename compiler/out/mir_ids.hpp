#ifndef MIR_IDS_HPP
#define MIR_IDS_HPP

#include "mlc.hpp"
#include <variant>

#include "mir_types.hpp"

namespace mir_ids {

struct Expr;
struct Stmt;
struct SemanticExpression;
struct SemanticStatement;
struct CppStatement;
struct CppExpression;

struct MirIdPool {int next_local_index;int next_block_index;};

struct MirIdLocalStep {mir_ids::MirIdPool pool;mir_types::LocalId local_id;};

struct MirIdBlockStep {mir_ids::MirIdPool pool;mir_types::BlockId block_id;};

mir_ids::MirIdPool mir_id_pool_new() noexcept;

mir_ids::MirIdPool mir_id_pool_locals_reserved(mir_ids::MirIdPool pool, int local_count) noexcept;

bool mir_local_id_equals(mir_types::LocalId left, mir_types::LocalId right) noexcept;

bool mir_block_id_equals(mir_types::BlockId left, mir_types::BlockId right) noexcept;

int mir_local_id_index(mir_types::LocalId local_id) noexcept;

int mir_block_id_index(mir_types::BlockId block_id) noexcept;

mir_ids::MirIdLocalStep mir_id_pool_allocate_local(mir_ids::MirIdPool pool) noexcept;

mir_ids::MirIdBlockStep mir_id_pool_allocate_block(mir_ids::MirIdPool pool) noexcept;

} // namespace mir_ids

#endif // MIR_IDS_HPP

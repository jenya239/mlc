#ifndef MIR_IDS_HPP
#define MIR_IDS_HPP

#include "mlc.hpp"
#include <variant>
#include "mir_types.hpp"
namespace mir_ids {

struct MirIdPool {
  int next_local_index;
  int next_block_index;
};
struct MirIdLocalStep {
  MirIdPool pool;
  mir_types::LocalId local_id;
};
struct MirIdBlockStep {
  MirIdPool pool;
  mir_types::BlockId block_id;
};
MirIdPool mir_id_pool_new() noexcept;
MirIdPool mir_id_pool_locals_reserved(MirIdPool pool, int local_count) noexcept;
bool mir_local_id_equals(mir_types::LocalId left, mir_types::LocalId right) noexcept;
bool mir_block_id_equals(mir_types::BlockId left, mir_types::BlockId right) noexcept;
int mir_local_id_index(mir_types::LocalId local_id) noexcept;
int mir_block_id_index(mir_types::BlockId block_id) noexcept;
MirIdLocalStep mir_id_pool_allocate_local(MirIdPool pool) noexcept;
MirIdBlockStep mir_id_pool_allocate_block(MirIdPool pool) noexcept;

} // namespace mir_ids

#endif // MIR_IDS_HPP

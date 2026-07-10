#define main mlc_user_main
#include "mir_ids.hpp"

#include "mir_types.hpp"

namespace mir_ids {

using namespace mir_types;

MirIdPool mir_id_pool_new() noexcept{
  return MirIdPool{0, 1};
}
MirIdPool mir_id_pool_locals_reserved(MirIdPool pool, int local_count) noexcept{
  return MirIdPool{local_count, pool.next_block_index};
}
bool mir_local_id_equals(mir_types::LocalId left, mir_types::LocalId right) noexcept{
  return (left.index == right.index);
}
bool mir_block_id_equals(mir_types::BlockId left, mir_types::BlockId right) noexcept{
  return (left.index == right.index);
}
int mir_local_id_index(mir_types::LocalId local_id) noexcept{
  return local_id.index;
}
int mir_block_id_index(mir_types::BlockId block_id) noexcept{
  return block_id.index;
}
MirIdLocalStep mir_id_pool_allocate_local(MirIdPool pool) noexcept{
  auto local_id = mir_types::mir_local_id(pool.next_local_index);
  return MirIdLocalStep{MirIdPool{mlc::arith::checked_add(pool.next_local_index, 1), pool.next_block_index}, local_id};
}
MirIdBlockStep mir_id_pool_allocate_block(MirIdPool pool) noexcept{
  auto block_id = mir_types::mir_block_id(pool.next_block_index);
  return MirIdBlockStep{MirIdPool{pool.next_local_index, mlc::arith::checked_add(pool.next_block_index, 1)}, block_id};
}

} // namespace mir_ids

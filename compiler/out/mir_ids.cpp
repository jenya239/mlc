#include "mir_ids.hpp"

#include "mir_types.hpp"

namespace mir_ids {

using namespace mir_types;

mir_ids::MirIdPool mir_id_pool_new() noexcept;

mir_ids::MirIdPool mir_id_pool_locals_reserved(mir_ids::MirIdPool pool, int local_count) noexcept;

bool mir_local_id_equals(mir_types::LocalId left, mir_types::LocalId right) noexcept;

bool mir_block_id_equals(mir_types::BlockId left, mir_types::BlockId right) noexcept;

int mir_local_id_index(mir_types::LocalId local_id) noexcept;

int mir_block_id_index(mir_types::BlockId block_id) noexcept;

mir_ids::MirIdLocalStep mir_id_pool_allocate_local(mir_ids::MirIdPool pool) noexcept;

mir_ids::MirIdBlockStep mir_id_pool_allocate_block(mir_ids::MirIdPool pool) noexcept;

mir_ids::MirIdPool mir_id_pool_new() noexcept{return mir_ids::MirIdPool{0, 1};}

mir_ids::MirIdPool mir_id_pool_locals_reserved(mir_ids::MirIdPool pool, int local_count) noexcept{return mir_ids::MirIdPool{local_count, pool.next_block_index};}

bool mir_local_id_equals(mir_types::LocalId left, mir_types::LocalId right) noexcept{return left.index == right.index;}

bool mir_block_id_equals(mir_types::BlockId left, mir_types::BlockId right) noexcept{return left.index == right.index;}

int mir_local_id_index(mir_types::LocalId local_id) noexcept{return local_id.index;}

int mir_block_id_index(mir_types::BlockId block_id) noexcept{return block_id.index;}

mir_ids::MirIdLocalStep mir_id_pool_allocate_local(mir_ids::MirIdPool pool) noexcept{
mir_types::LocalId local_id = mir_types::mir_local_id(pool.next_local_index);
return mir_ids::MirIdLocalStep{mir_ids::MirIdPool{pool.next_local_index + 1, pool.next_block_index}, local_id};
}

mir_ids::MirIdBlockStep mir_id_pool_allocate_block(mir_ids::MirIdPool pool) noexcept{
mir_types::BlockId block_id = mir_types::mir_block_id(pool.next_block_index);
return mir_ids::MirIdBlockStep{mir_ids::MirIdPool{pool.next_local_index, pool.next_block_index + 1}, block_id};
}

} // namespace mir_ids

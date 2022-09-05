#include "foundation/details/delegate_handle.hpp"

namespace Engine
{
    std::atomic<uint64> DelegateHandle::NextID = 1;
}
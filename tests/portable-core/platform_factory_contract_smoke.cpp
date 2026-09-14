#include <cstdlib>
#include <type_traits>

#include "VirtualHost/ICorePlatformFactory.h"

static_assert(std::has_virtual_destructor_v<ICorePlatformFactory>);
static_assert(std::is_abstract_v<ICorePlatformFactory>);

int main()
{
  return EXIT_SUCCESS;
}

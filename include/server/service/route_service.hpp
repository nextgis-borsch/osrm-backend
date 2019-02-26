#ifndef SERVER_SERVICE_ROUTE_SERVICE_HPP
#define SERVER_SERVICE_ROUTE_SERVICE_HPP

#include "server/service/base_service.hpp"

#include "engine/status.hpp"
#include "osrm/osrm.hpp"
#include "util/coordinate.hpp"

#include <string>
#include <vector>

namespace osrm
{
namespace server
{
namespace service
{

class RouteService final : public BaseService
{
  public:
    RouteService(OSRM &routing_machine) : BaseService(routing_machine) {}

    engine::Status
    RunQuery(std::size_t prefix_length, std::string &query, ResultT &result) final override;

    unsigned GetVersion() final override { return 1; }
};
}
}
}

#endif

// Utility for avral project.
// Finds the best path between two points.

#include "osrm/route_parameters.hpp"

#include "osrm/coordinate.hpp"
#include "osrm/engine_config.hpp"
#include "osrm/json_container.hpp"
#include "util/json_renderer.hpp"

#include "osrm/osrm.hpp"
#include "osrm/status.hpp"

#include <iostream>
#include <fstream>

/**
 *  Save osrm::json object into file.
 */
void saveJsonObject (std::string file_path, osrm::json::Object json)
{
    std::filebuf fb;
    fb.open(file_path, std::ios::out);
    std::ostream os(&fb);
    osrm::util::json::render(os, json);
    fb.close();
}

/**
 *  Get osrm::json object as a string.
 */
std::string getJsonObject (osrm::json::Object json)
{
    std::ostringstream sb;
    osrm::util::json::render(sb, json);
    return sb.str();
}


int main(int argc, const char *argv[])
{
    if (argc != 6)
    {
        std::cerr << "Usage: " << argv[0] << " lon_start lat_start lon_end lat_end osrm_file \n";
        return EXIT_FAILURE;
    }

    double d_lon_start;
    double d_lat_start;
    double d_lon_end;
    double d_lat_end;
    try
    {
        d_lon_start = std::stod(argv[1]);
        d_lat_start = std::stod(argv[2]);
        d_lon_end = std::stod(argv[3]);
        d_lat_end = std::stod(argv[4]);
    }
    catch (...)
    {
        std::cerr << "[avral] Failure: incorrect input coordinates. \n";
        return EXIT_FAILURE;
    }

    osrm::util::FloatLongitude lon_start {d_lon_start};
    osrm::util::FloatLatitude lat_start {d_lat_start};
    osrm::util::FloatLongitude lon_end {d_lon_end};
    osrm::util::FloatLatitude lat_end {d_lat_end};

    std::string graph_dir = argv[5];
    std::string result_file = ""; // TEMP

    osrm::engine::EngineConfig engine_config;
    engine_config.storage_config = {graph_dir};
    engine_config.use_shared_memory = true;
    engine_config.algorithm = osrm::engine::EngineConfig::Algorithm::MLD;

    osrm::engine::api::RouteParameters route_params;
    route_params.coordinates.push_back({lon_start, lat_start});
    route_params.coordinates.push_back({lon_end, lat_end});
    //route_params.radiuses.push_back(0.0); // this will prevent "jumping" from/to terminals to/from lines
    //route_params.radiuses.push_back(0.0);
    route_params.geometries = osrm::engine::api::RouteParameters::GeometriesType::GeoJSON;
    route_params.overview = osrm::engine::api::RouteParameters::OverviewType::Full;
    route_params.steps = false;
    route_params.annotations = false;
    route_params.alternatives = false;

    const osrm::OSRM osrm {engine_config};

    osrm::json::Object j_routing_result;
    osrm::engine::Status status = osrm.Route(route_params, j_routing_result);

    if (status != osrm::engine::Status::Ok)
    {
        const auto code = j_routing_result.values["code"].get<osrm::json::String>().value;
        const auto message = j_routing_result.values["message"].get<osrm::json::String>().value;

        std::cout << "[avral] Route was not found!\n";
        std::cout << "        Code: " << code << "\n";
        std::cout << "        Message: " << message << "\n";
        return EXIT_FAILURE;
    }

    osrm::json::Array j_routes = j_routing_result.values["routes"].get<osrm::json::Array>();
    osrm::json::Object j_route = j_routes.values.at(0).get<osrm::json::Object>(); // always the single route
    //const double summ_distance = j_route.values["distance"].get<osrm::json::Number>().value;
    //const double summ_duration = j_route.values["duration"].get<osrm::json::Number>().value;

/*
    std::cout << "[avral] Route was successfuly found: \n";
    std::cout << "        Distance = " << summ_distance << " m\n";
    std::cout << "        Duration = " << summ_duration << " s\n";

    saveJsonObject(result_file, j_route);
*/

    std::cout << getJsonObject(j_route) << std::endl;

    return EXIT_SUCCESS;
}

#pragma once
#include "json_reader.h"
#include "map_renderer.h"
#include "domain.h"
#include "transport_router.h"
#include <optional>


class RequestHandler
{
public:    

    explicit RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer); 

    RequestHandler(const transport_catalogue::TransportCatalogue& db
        , const renderer::MapRenderer& renderer
        , const transport_router::TransportRouter& tr);

    std::optional<transport_catalogue::BusStat> GetBusStat(const std::string_view& bus_name) const noexcept;

    transport_catalogue::StopInfo GetBusesByStop(const std::string_view& stop_name) const noexcept;

    const transport_router::TransportRouter& GetTransportRouter() const noexcept;

    std::optional<std::string> GetMap() const noexcept;
    
private:
    const transport_catalogue::TransportCatalogue* db_;

    const renderer::MapRenderer* renderer_ = nullptr;

    const transport_router::TransportRouter* tr_ = nullptr;
};

namespace stat_request
{
    inline json::Node MessageErrore(const domain::StatRequest& stat) noexcept;

    inline json::Node RepareReportRouter(const domain::StatRequest& stat, const std::optional<transport_router::TransportRouter::RouteInfo>& reports) noexcept;

    inline json::Node RepareMap(const RequestHandler& rh, const domain::StatRequest& stat) noexcept;

    inline json::Node RepareReportStop(const transport_catalogue::StopInfo& stop, const domain::StatRequest& stat) noexcept;

    inline json::Node RepareReportBus(const transport_catalogue::BusStat& bus, const domain::StatRequest& stat) noexcept;

    inline json::Document PrepareDocument(const RequestHandler& rh, const std::vector<domain::StatRequest>& stat_requests);

    inline void StatRequestStop(const RequestHandler& rh, const domain::StatRequest& stat, json::Array& answers) noexcept;

    inline void StatRequestBus(const RequestHandler& rh, const domain::StatRequest& stat, json::Array& answers) noexcept;

    inline void StatRequestRoute(const RequestHandler& rh, const domain::StatRequest& stat, json::Array& answers) noexcept;

	void PrintStatDoc(const RequestHandler& rh, const std::vector<domain::StatRequest>& stat_requests, std::ostream& out = std::cout) noexcept;
}//namespace
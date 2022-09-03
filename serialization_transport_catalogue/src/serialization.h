#pragma once
#include "transport_catalogue.pb.h"
#include "transport_router.h"
#include "map_renderer.h"
#include "json_reader.h"
#include "request_handler.h"

#include <fstream>
#include <iostream>
#include <unordered_map>


namespace serialization
{
    class Serialization final
    {
    public:
        using StopName = std::string;
        using StopsVertexId = std::unordered_map<StopName, graph::VertexId>;
        using Vertices = std::vector<StopName>;
        using StopPtr = transport_catalogue::StopPtr;
        using StopHasher = transport_catalogue::StopHasher;
        using StopPtrEqual = transport_catalogue::StopPtrEqual;
        using MapDistanceTransportCatalogue = std::unordered_map<std::pair<StopPtr, StopPtr>, int, StopHasher, StopPtrEqual>;
        using RoutesInternalData = std::vector<std::vector<std::optional<graph::Router<double>::RouteInternalData>>>;

        Serialization() = delete;

        Serialization(const request::RequestReader& rr, const transport_catalogue::TransportCatalogue& tc);

        void AddProtoBus(transport_catalogue_proto::TransportCatalogue& tc_proto
            , const std::deque<transport_catalogue::Bus>& buses);

        void AddProtoStop(transport_catalogue_proto::TransportCatalogue& tc_proto
            , const std::deque<transport_catalogue::Stop>& stops);

        void AddProtoDistanceFromTo(transport_catalogue_proto::TransportCatalogue& tc_proto
            , const MapDistanceTransportCatalogue& distances);

        transport_catalogue_proto::Color SaveColor(const svg::Color& color);

        void SaveMap(transport_catalogue_proto::TransportCatalogue& tc_proto
            , const renderer::RenderSettings& settings);

        void AddProtoRouterGraphEdges(transport_catalogue_proto::TransportRouter& router
            , std::vector<graph::Edge<double>> edges);

        void AddProtoRouterGraphIncidenceLists(transport_catalogue_proto::TransportRouter& router
            , const std::vector<std::vector<graph::EdgeId>>& incidence_lsts);

        void AddProtoRouterStopVertexId(transport_catalogue_proto::TransportRouter& router
            , const StopsVertexId& stop_vertex_id);

        void AddProtoRouterVertexInfo(transport_catalogue_proto::TransportRouter& router
            , const Vertices& vertex_info);

        void AddProtoRouterEdgeInfo(transport_catalogue_proto::TransportRouter& router
            , const transport_router::TransportRouter::EdgeInfo& edge_info);

        void AddProtoRouterEdgesInfo(transport_catalogue_proto::TransportRouter& router
            , const std::vector<transport_router::TransportRouter::EdgeInfo>& edges_info);

        void AddProtoRouterRoutingSettings(transport_catalogue_proto::TransportRouter& router
            , const domain::RoutingSettings& routing_settings);

        transport_catalogue_proto::RouterInternalData AddProtoRouterInternalData
        (const std::optional<transport_router::TransportRouter::Router::RouteInternalData>& one_data);

        transport_catalogue_proto::VectorRouterInternalData AddProtoRouterVectorRouterInternalData
        (const std::vector<std::optional<graph::Router<double>::RouteInternalData>>& data);

        void AddProtoRouterData(transport_catalogue_proto::TransportRouter& router, const RoutesInternalData& all_data);

        void GreateProtoTransportRouter(transport_catalogue_proto::TransportCatalogue& tc_proto
            , const transport_catalogue::TransportCatalogue& db, const domain::RoutingSettings& routing_settings);

    private:
        const request::RequestReader& rr_;
        const transport_catalogue::TransportCatalogue& tc_;
    };
}

namespace deserialization
{
    struct Hasher
    {
        size_t operator()(const std::pair<std::string, std::string>& stops) const noexcept;
    };

    class Deserialization final
    {
    public:

        using Bus = transport_catalogue::Bus;
        using Stop = transport_catalogue::Stop;
        using StopName = std::string;
        using MapDistanceBetwinStops = std::unordered_map<std::pair<StopName, StopName>, int, Hasher>;      
       
        Deserialization() = delete;

        explicit Deserialization(const request::RequestReader& rr);

        std::vector<Bus> CreateBuses(const transport_catalogue_proto::TransportCatalogue& tc_proto);

        std::vector<Stop> CreateStops(const transport_catalogue_proto::TransportCatalogue& tc_proto);

        MapDistanceBetwinStops CreateMapDistanceBetwinStops(const transport_catalogue_proto::TransportCatalogue& tc_proto);

        void CreateTransportCatalogue(std::vector<Bus>&& buses, std::vector<Stop>&& stops, const MapDistanceBetwinStops& dist_betw_stops);

        svg::Color LoadColor(const transport_catalogue_proto::Color& color);

        void CreateRenderSettings(const transport_catalogue_proto::Map& map);

        std::vector<graph::Edge<double>> AddEdges(const transport_catalogue_proto::TransportRouter& router);       

        std::vector<std::vector<graph::EdgeId>> AddInclidenceLists(const transport_catalogue_proto::TransportRouter& router);        

        std::unordered_map<StopName, graph::VertexId> AddStopsVertexId(const transport_catalogue_proto::TransportRouter& router);       

        std::vector<StopName> AddVertexInfo(const transport_catalogue_proto::TransportRouter& router);       

        std::vector<transport_router::TransportRouter::EdgeInfo> AddEdgesInfo(const transport_catalogue_proto::TransportRouter& router);        

        std::vector<std::optional<graph::Router<double>::RouteInternalData>> AddRouteInternalData(const transport_catalogue_proto::VectorRouterInternalData& data_proto);        

        graph::Router<double>::RoutesInternalData AddRoutersInternalData(const transport_catalogue_proto::TransportRouter& router);
       
        domain::RoutingSettings AddRoutinSettings(const transport_catalogue_proto::TransportRouter& router);
        
        void CreateTransportRouter(const transport_catalogue_proto::TransportRouter& router_proto);

        void PrintStatRequest() const noexcept;       
        
    private:
        const request::RequestReader& rr_;
        transport_catalogue::TransportCatalogue tc_;
        renderer::RenderSettings settings_;
        renderer::MapRenderer renderer_;
        std::unique_ptr<transport_router::TransportRouter> tr_ ;
    };
}
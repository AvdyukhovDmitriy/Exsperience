#include "serialization.h"

namespace serialization
{

    Serialization::Serialization(const request::RequestReader& rr, const transport_catalogue::TransportCatalogue& tc)
        : rr_(rr), tc_(tc)
    {
        std::ofstream out(rr_.GetPath(), std::ios::binary);
        if (out.is_open())
        {
            transport_catalogue_proto::TransportCatalogue tc_proto;
            AddProtoBus(tc_proto, tc_.GetRoute());
            AddProtoStop(tc_proto, tc_.GetStop());
            AddProtoDistanceFromTo(tc_proto, tc_.GetMapDistance());
            SaveMap(tc_proto, rr.GetRendereSettings());
            GreateProtoTransportRouter(tc_proto, tc_, rr.GetRoutingSettings());

            tc_proto.SerializePartialToOstream(&out);
        }
    }

    void Serialization::AddProtoBus(transport_catalogue_proto::TransportCatalogue& tc_proto
        , const std::deque<transport_catalogue::Bus>& buses)
    {
        for (const auto& bus : buses)
        {
            transport_catalogue_proto::Bus b;
            b.set_name(bus.name);
            b.set_is_roundtrip(bus.is_roundtrip);
            b.set_name_last_stop(bus.name_last_stop);
            for (const auto& stop : bus.stops)
            {
                b.add_stops(stop->name);
            }

            tc_proto.mutable_buses()->Add(std::move(b));
        }
    }

    void Serialization::AddProtoStop(transport_catalogue_proto::TransportCatalogue& tc_proto
        , const std::deque<transport_catalogue::Stop>& stops)
    {
        for (const auto& stop : stops)
        {
            transport_catalogue_proto::Stop s;
            s.set_name(stop.name);
            s.mutable_coor()->set_latitude(stop.coordinates.lat);
            s.mutable_coor()->set_longitude(stop.coordinates.lng);
            tc_proto.mutable_stops()->Add(std::move(s));
        }
    }

    void Serialization::AddProtoDistanceFromTo(transport_catalogue_proto::TransportCatalogue& tc_proto
        , const MapDistanceTransportCatalogue& distances)
    {
        for (const auto& [stops, dis] : distances)
        {
            transport_catalogue_proto::DistanceFromTo dist;
            dist.set_from_stop(stops.first->name);
            dist.set_to_stop(stops.second->name);
            dist.set_distance(dis);
            tc_proto.mutable_distance()->Add(std::move(dist));
        }
    }

    transport_catalogue_proto::Color Serialization::SaveColor(const svg::Color& color)
    {
        transport_catalogue_proto::Color col;
        if (std::holds_alternative<std::string>(color))
        {
            col.mutable_color_string()->set_color(std::get<std::string>(color));
        }
        else if (std::holds_alternative<svg::Rgb>(color))
        {
            col.mutable_rgb()->set_red(std::get<svg::Rgb>(color).red);
            col.mutable_rgb()->set_green(std::get<svg::Rgb>(color).green);
            col.mutable_rgb()->set_blue(std::get<svg::Rgb>(color).blue);
        }
        else if (std::holds_alternative<svg::Rgba>(color))
        {
            col.mutable_rgba()->set_red(std::get<svg::Rgba>(color).red);
            col.mutable_rgba()->set_green(std::get<svg::Rgba>(color).green);
            col.mutable_rgba()->set_blue(std::get<svg::Rgba>(color).blue);
            col.mutable_rgba()->set_opacity(std::get<svg::Rgba>(color).opacity);
        }
        return col;
    }

    void Serialization::SaveMap(transport_catalogue_proto::TransportCatalogue& tc_proto
        , const renderer::RenderSettings& settings)
    {
        transport_catalogue_proto::Map map;
        map.set_width(settings.width);
        map.set_height(settings.height);
        map.set_padding(settings.padding);
        map.set_line_width(settings.line_width);
        map.set_stop_radius(settings.stop_radius);

        map.set_bus_label_font_size(settings.bus_label_font_size);
        map.mutable_bus_label_offset()->set_latitude(settings.bus_label_offset.lat);
        map.mutable_bus_label_offset()->set_longitude(settings.bus_label_offset.lng);

        map.set_stop_label_font_size(settings.stop_label_font_size);
        map.mutable_stop_lable_offset()->set_latitude(settings.stop_label_offset.lat);
        map.mutable_stop_lable_offset()->set_longitude(settings.stop_label_offset.lng);

        transport_catalogue_proto::Color color = SaveColor(settings.underlayer_color);
        map.mutable_underlayer_color()->Swap(&color);
        map.set_underlayer_width(settings.underlayer_width);

        for (const auto& colors : settings.color_palette)
        {
            transport_catalogue_proto::Color color = SaveColor(colors);
            map.mutable_color_palette()->Add(std::move(color));
        }

        tc_proto.mutable_map()->Swap(&map);
    }

    void Serialization::AddProtoRouterGraphEdges(transport_catalogue_proto::TransportRouter& router
        , std::vector<graph::Edge<double>> edges)
    {
        for (auto edge : edges)
        {
            transport_catalogue_proto::Edge ed;
            ed.set_from(std::move(edge.from));
            ed.set_to(std::move(edge.to));
            ed.set_weight(std::move(edge.weight));
            router.mutable_graph()->mutable_edges()->Add(std::move(ed));
        }
    }

    void Serialization::AddProtoRouterGraphIncidenceLists(transport_catalogue_proto::TransportRouter& router
        , const std::vector<std::vector<size_t>>& incidence_lsts)
    {
        for (const auto& lists : incidence_lsts)
        {
            transport_catalogue_proto::IncidenceList list_proto;
            for (auto list : lists)
            {
                list_proto.add_edge_id(std::move(list));
            }
            router.mutable_graph()->mutable_inclidence_lists()->Add(std::move(list_proto));
        }
    }

    void Serialization::AddProtoRouterStopVertexId(transport_catalogue_proto::TransportRouter& router
        , const StopsVertexId& stop_vertex_id)
    {
        for (const auto& [key, value] : stop_vertex_id)
        {
            transport_catalogue_proto::MapStopVertex map;
            map.set_key(key);
            map.set_value(value);
            router.mutable_stop_vertex_id()->Add(std::move(map));
        }
    }

    void Serialization::AddProtoRouterVertexInfo(transport_catalogue_proto::TransportRouter& router
        , const Vertices& vertex_info)
    {
        for (auto stop : vertex_info)
        {
            router.mutable_vertex_info()->Add(std::move(stop));
        }
    }

    void Serialization::AddProtoRouterEdgeInfo(transport_catalogue_proto::TransportRouter& router
        , const transport_router::TransportRouter::EdgeInfo& edge_info)
    {
        transport_catalogue_proto::EdgeInfo var;
        if (std::holds_alternative<transport_router::TransportRouter::BusEdgeInfo>(edge_info))
        {
            var.mutable_bus_edge_info()->set_name_bus(std::get<transport_router::TransportRouter::BusEdgeInfo>(edge_info).bus_name);
            var.mutable_bus_edge_info()->set_span_count(std::get<transport_router::TransportRouter::BusEdgeInfo>(edge_info).span_count);
            var.mutable_bus_edge_info()->set_number_edge(std::get<transport_router::TransportRouter::BusEdgeInfo>(edge_info).number_edge);
        }
        else if (std::holds_alternative<transport_router::TransportRouter::WaitAdgeInfo>(edge_info))
        {
            var.mutable_wait_edge_info();
        }
        router.mutable_edges_info()->Add(std::move(var));
    }

    void Serialization::AddProtoRouterEdgesInfo(transport_catalogue_proto::TransportRouter& router
        , const std::vector<transport_router::TransportRouter::EdgeInfo>& edges_info)
    {
        for (const auto& edge_info : edges_info)
        {
            AddProtoRouterEdgeInfo(router, edge_info);
        }
    }

    void Serialization::AddProtoRouterRoutingSettings(transport_catalogue_proto::TransportRouter& router
        , const domain::RoutingSettings& routing_settings)
    {
        router.mutable_routing_settings()->set_bus_wait_time(routing_settings.bus_wait_time);
        router.mutable_routing_settings()->set_bus_velocity(routing_settings.bus_velocity);
    }

    transport_catalogue_proto::RouterInternalData Serialization::AddProtoRouterInternalData
    (const std::optional<transport_router::TransportRouter::Router::RouteInternalData>& one_data)
    {
        transport_catalogue_proto::RouterInternalData data_proto;
        if (one_data.has_value())
        {
            data_proto.set_flag(true);
            data_proto.set_weigth(one_data.value().weight);
            if (one_data.value().prev_edge.has_value())
            {
                data_proto.mutable_prev()->set_prev_dge(one_data.value().prev_edge.value());
                data_proto.mutable_prev()->set_flag(true);
            }
        }
        return data_proto;
    }

    transport_catalogue_proto::VectorRouterInternalData Serialization::AddProtoRouterVectorRouterInternalData
    (const std::vector<std::optional<graph::Router<double>::RouteInternalData>>& data)
    {
        transport_catalogue_proto::VectorRouterInternalData v_data;
        for (const auto& one_data : data)
        {
            v_data.mutable_data()->Add(std::move(AddProtoRouterInternalData(one_data)));
        }
        return v_data;
    }

    void Serialization::AddProtoRouterData(transport_catalogue_proto::TransportRouter& router
        , const RoutesInternalData& all_data)
    {
        for (const auto& data : all_data)
        {
            router.mutable_graph_router()->mutable_router_data()->Add(std::move(AddProtoRouterVectorRouterInternalData(data)));
        }
    }

    void Serialization::GreateProtoTransportRouter(transport_catalogue_proto::TransportCatalogue& tc_proto
        , const transport_catalogue::TransportCatalogue& db, const domain::RoutingSettings& routing_settings)
    {
        transport_catalogue_proto::TransportRouter router;
        transport_router::TransportRouter tr(db, routing_settings);

        AddProtoRouterGraphEdges(router, tr.GetGraph().GetEdges());
        AddProtoRouterGraphIncidenceLists(router, tr.GetGraph().GetIncidenceLists());
        AddProtoRouterStopVertexId(router, tr.GetStopVertexId());
        AddProtoRouterVertexInfo(router, tr.GetVertexInfo());
        AddProtoRouterEdgesInfo(router, tr.GetVectorEdgeInfo());
        AddProtoRouterRoutingSettings(router, routing_settings);
        AddProtoRouterData(router, tr.GetRouter().GetRouterData());
        tc_proto.mutable_router()->Swap(&router);
    }
}// ---------------------------------end namespace serialization

namespace deserialization 
{
    //-----------------------------------Hasher

    size_t Hasher::operator()(const std::pair<std::string, std::string>& stops) const noexcept
    {
        std::hash<std::string>hstr;
        return static_cast<size_t>(hstr(stops.first))
            + static_cast<size_t>(hstr(stops.second));
    }

    //----------------------class Deserialization

    Deserialization::Deserialization(const request::RequestReader& rr) : rr_(rr)
    {
        std::ifstream in(rr_.GetPath(), std::ios::binary);
        transport_catalogue_proto::TransportCatalogue tc_proto;
        if (tc_proto.ParseFromIstream(&in))
        {
            CreateTransportCatalogue(CreateBuses(tc_proto), CreateStops(tc_proto)
                , CreateMapDistanceBetwinStops(tc_proto));
            CreateRenderSettings(tc_proto.map());
            renderer_(settings_, tc_);
            CreateTransportRouter(tc_proto.router());
        }
    }

    std::vector<Deserialization::Bus> Deserialization::CreateBuses
    (const transport_catalogue_proto::TransportCatalogue& tc_proto)
    {
        std::vector<Deserialization::Bus> buses;
        for (const auto& bus : tc_proto.buses())
        {
            transport_catalogue::Bus bus1;
            bus1.name = bus.name();
            bus1.is_roundtrip = bus.is_roundtrip();
            bus1.name_last_stop = bus.name_last_stop();
            for (const auto& stop : bus.stops())
            {
                bus1.stops_for_bus.push_back(stop);
            }
            buses.push_back(std::move(bus1));
        }
        return buses;
    }

    std::vector<Deserialization::Stop> Deserialization::CreateStops
    (const transport_catalogue_proto::TransportCatalogue& tc_proto)
    {
        std::vector<Deserialization::Stop> stops;
        for (const auto& stop : tc_proto.stops())
        {
            transport_catalogue::Stop temp;
            temp.name = stop.name();
            temp.coordinates.lat = stop.coor().latitude();
            temp.coordinates.lng = stop.coor().longitude();
            stops.push_back(std::move(temp));
        }
        return stops;
    }

    Deserialization::MapDistanceBetwinStops Deserialization::CreateMapDistanceBetwinStops
    (const transport_catalogue_proto::TransportCatalogue& tc_proto)
    {
        Deserialization::MapDistanceBetwinStops dist_betw_stops;
        for (const auto& dist : tc_proto.distance())
        {
            dist_betw_stops.insert({ { dist.from_stop(), dist.to_stop() }, static_cast<int>(dist.distance()) });
        }
        return dist_betw_stops;
    }

    void Deserialization::CreateTransportCatalogue(std::vector<Bus>&& buses, std::vector<Stop>&& stops
        , const MapDistanceBetwinStops& dist_betw_stops)
    {
        for (auto stop : stops)
        {
            tc_.AddStop({ stop.coordinates.lat, stop.coordinates.lng }, std::move(stop.name));
        }
        for (const auto& [stops, dist] : dist_betw_stops)
        {
            transport_catalogue::StopPtr stop1 = tc_.FindStop(stops.first);
            transport_catalogue::StopPtr stop2 = tc_.FindStop(stops.second);
            tc_.CreateDistBetweenStops(stop1, stop2, dist);
        }
        for (auto bus : buses)
        {
            tc_.AddBus(std::move(bus.name), std::move(bus.stops_for_bus)
                , bus.is_roundtrip, std::move(bus.name_last_stop));
        }
    }

    svg::Color Deserialization::LoadColor(const transport_catalogue_proto::Color& color)
    {
        svg::Color result;
        if (color.has_color_string())
        {
            result = color.color_string().color();
        }
        else if (color.has_rgb())
        {
            svg::Rgb rgb;
            rgb.red = color.rgb().red();
            rgb.green = color.rgb().green();
            rgb.blue = color.rgb().blue();
            result = rgb;
        }
        else if (color.has_rgba())
        {
            svg::Rgba rgba;
            rgba.red = color.rgba().red();
            rgba.green = color.rgba().green();
            rgba.blue = color.rgba().blue();
            rgba.opacity = color.rgba().opacity();
            result = rgba;
        }
        return result;
    }

    void Deserialization::CreateRenderSettings(const transport_catalogue_proto::Map& map)
    {
        settings_.width = map.width();
        settings_.height = map.height();
        settings_.padding = map.padding();
        settings_.line_width = map.line_width();
        settings_.stop_radius = map.stop_radius();

        settings_.bus_label_font_size = map.bus_label_font_size();
        settings_.bus_label_offset.lat = map.bus_label_offset().latitude();
        settings_.bus_label_offset.lng = map.bus_label_offset().longitude();

        settings_.stop_label_font_size = map.stop_label_font_size();
        settings_.stop_label_offset.lat = map.stop_lable_offset().latitude();
        settings_.stop_label_offset.lng = map.stop_lable_offset().longitude();

        settings_.underlayer_color = LoadColor(map.underlayer_color());
        settings_.underlayer_width = map.underlayer_width();
        for (auto color : map.color_palette())
        {
            settings_.color_palette.push_back(std::move(LoadColor(color)));
        }
    }

    std::vector<graph::Edge<double>> Deserialization::AddEdges
    (const transport_catalogue_proto::TransportRouter& router)
    {
        std::vector<graph::Edge<double>> edges;
        {
            for (const auto& edge : router.graph().edges())
            {
                edges.push_back({ edge.from(), edge.to(), edge.weight() });
            }
        }
        return edges;
    }

    std::vector<std::vector<graph::EdgeId>> Deserialization::AddInclidenceLists
    (const transport_catalogue_proto::TransportRouter& router)
    {
        std::vector<std::vector<graph::EdgeId>> incidence_lsts;
        {
            for (const auto& lists : router.graph().inclidence_lists())
            {
                incidence_lsts.push_back({ lists.edge_id().begin(), lists.edge_id().end() });
            }
        }
        return incidence_lsts;
    }

    std::unordered_map<Deserialization::StopName, graph::VertexId> Deserialization::AddStopsVertexId
    (const transport_catalogue_proto::TransportRouter& router)
    {
        std::unordered_map<Deserialization::StopName, graph::VertexId> stops_vertex_id;
        {
            for (const auto& map : router.stop_vertex_id())
            {
                stops_vertex_id.insert({ map.key(), map.value() });
            }
        }
        return stops_vertex_id;
    }

    std::vector<Deserialization::StopName> Deserialization::AddVertexInfo
    (const transport_catalogue_proto::TransportRouter& router)
    {
        return { router.vertex_info().begin(), router.vertex_info().end() };
    }

    std::vector<transport_router::TransportRouter::EdgeInfo> Deserialization::AddEdgesInfo
    (const transport_catalogue_proto::TransportRouter& router)
    {
        std::vector<transport_router::TransportRouter::EdgeInfo> edges_info;
        for (const auto& edge : router.edges_info())
        {
            if (edge.has_bus_edge_info())
            {
                transport_router::TransportRouter::BusEdgeInfo bus_info;
                bus_info.bus_name = edge.bus_edge_info().name_bus();
                bus_info.span_count = edge.bus_edge_info().span_count();
                bus_info.number_edge = edge.bus_edge_info().number_edge();
                edges_info.push_back(std::move(bus_info));
            }
            else if (edge.has_wait_edge_info())
            {
                edges_info.push_back(transport_router::TransportRouter::WaitAdgeInfo());
            }
        }
        return edges_info;
    }

    std::vector<std::optional<graph::Router<double>::RouteInternalData>> Deserialization::AddRouteInternalData
    (const transport_catalogue_proto::VectorRouterInternalData& data_proto)
    {
        std::vector<std::optional<graph::Router<double>::RouteInternalData>> result;
        for (const auto& all_data : data_proto.data())
        {
            if (all_data.flag())
            {
                graph::Router<double>::RouteInternalData one_data;
                one_data.weight = all_data.weigth();
                if (all_data.prev().flag())
                {
                    one_data.prev_edge = all_data.prev().prev_dge();
                }
                else
                {
                    one_data.prev_edge = std::nullopt;
                }
                result.push_back(std::move(one_data));
            }
            else
            {
                result.push_back(std::nullopt);
            }
        }
        return result;
    }

    graph::Router<double>::RoutesInternalData Deserialization::AddRoutersInternalData
    (const transport_catalogue_proto::TransportRouter& router)
    {
        graph::Router<double>::RoutesInternalData data;
        for (const auto& data_proto : router.graph_router().router_data())
        {
            data.push_back(std::move(AddRouteInternalData(data_proto)));
        }
        return data;
    }

    domain::RoutingSettings Deserialization::AddRoutinSettings
    (const transport_catalogue_proto::TransportRouter& router)
    {
        domain::RoutingSettings settings;
        settings.bus_wait_time = router.routing_settings().bus_wait_time();
        settings.bus_velocity = router.routing_settings().bus_velocity();
        return settings;
    }

    void Deserialization::CreateTransportRouter(const transport_catalogue_proto::TransportRouter& router_proto)
    {
        tr_ = std::make_unique< transport_router::TransportRouter>(tc_
            , std::move(AddRoutinSettings(router_proto))
            , std::move(AddStopsVertexId(router_proto))
            , std::move(AddVertexInfo(router_proto))
            , std::move(AddEdgesInfo(router_proto))
            , std::move(AddEdges(router_proto))
            , std::move(AddInclidenceLists(router_proto))
            , std::move(AddRoutersInternalData(router_proto)));
    }

    void Deserialization::PrintStatRequest() const noexcept
    {
        RequestHandler rh(tc_, renderer_, *tr_);
        stat_request::PrintStatDoc(rh, rr_.GetStatRequest());
    }

}//----------------------end namespace deserialization


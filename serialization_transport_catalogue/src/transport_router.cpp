#include "transport_router.h"

namespace transport_router
{
	TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue& tc, const domain::RoutingSettings& settings)
		: transport_catalogue_(tc)
		, routing_settings_(settings)
		, graph_(tc.GetStop().size())
	{		
		CreateGraph();
	}

	TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue& transport_catalogue
		, domain::RoutingSettings&& routing_settings
		, StopsVertexId&& stops_vertex_id
		, Vertices&& vertices_info
		, std::vector<EdgeInfo>&& adges_info
		, std::vector<graph::Edge<double>>&& edges
		, std::vector<std::vector<size_t>>&& incidence_lists
		, graph::Router<double>::RoutesInternalData&& data)
		: transport_catalogue_(transport_catalogue), routing_settings_(routing_settings)
	{
		graph_(std::move(edges), std::move(incidence_lists));
		stops_vertex_id_ = { stops_vertex_id.begin(), stops_vertex_id.end() };
		vertices_info_ = { vertices_info.begin(), vertices_info.end() };
		edges_info_ = { adges_info.begin(), adges_info.end() };
		static  Router router(graph_, std::move(data));
		router_ = std::make_unique<Router>(router);
	}

	inline void TransportRouter::CreateGraph() noexcept
	{
		for (const auto& bus : transport_catalogue_.GetRoute())
		{
			const auto& stops = bus.stops;
			FillGraph(stops.begin(), stops.end(), bus.name);
		}
		static  Router router(graph_);
		router_ = std::make_unique<Router>(router);
	}
	
	std::optional<transport_router::TransportRouter::RouteInfo> TransportRouter::FindRoute(const std::string& stop1, const std::string& stop2) const noexcept
	{
		using namespace graph;
		if (std::count(vertices_info_.begin(), vertices_info_.end(), stop1) == 0
			|| std::count(vertices_info_.begin(), vertices_info_.end(), stop2) == 0)
		{
			return {};
		}
		VertexId start = stops_vertex_id_.at(stop1);
		VertexId finish = stops_vertex_id_.at(stop2);
		const auto& info_route = router_->BuildRoute(start, finish);
		if (info_route)
		{
			RouteInfo result;
			result.total_time = info_route->weight;
			const std::vector<EdgeId>& edges = info_route->edges;
			for (const auto& edge : edges)
			{
				RouteInfo::WaitItem wait_item = CreateWaitItem(edge);				
				result.items.push_back(std::move(wait_item));				
				result.items.push_back(std::move(CreateBusItem(edge, wait_item.time)));
			}
			return result;
		}
		else
		{
			return {};
		}
	}

	inline TransportRouter::RouteInfo::WaitItem TransportRouter::CreateWaitItem(const graph::EdgeId& edge) const noexcept
	{
		TransportRouter::RouteInfo::WaitItem wait_item;
		wait_item.stop_name = vertices_info_.at(graph_.GetEdge(edge).from);
		wait_item.time = routing_settings_.bus_wait_time;
		return wait_item;
	}

	inline TransportRouter::RouteInfo::BusItem TransportRouter::CreateBusItem(const graph::EdgeId& edge , double time) const noexcept
	{
		RouteInfo::BusItem bus_item;
		BusEdgeInfo bus_info = std::get<BusEdgeInfo>(edges_info_.at(edge));
		bus_item.bus_name = bus_info.bus_name;
		bus_item.span_count = bus_info.span_count;
		bus_item.time = graph_.GetEdge(edge).weight -time;
		return bus_item;
	}

	TransportRouter::StopsVertexId TransportRouter::GetStopVertexId() const noexcept
	{
		return stops_vertex_id_;
	}

	TransportRouter::Vertices TransportRouter::GetVertexInfo() const noexcept
	{
		return vertices_info_;
	}

	std::vector<TransportRouter::EdgeInfo> TransportRouter::GetVectorEdgeInfo() const noexcept
	{
		return edges_info_;
	}

	TransportRouter::BusGraph TransportRouter::GetGraph() const noexcept
	{
		return graph_;
	}

	TransportRouter::Router TransportRouter::GetRouter() const noexcept
	{
		return *router_;
	}	

	inline graph::VertexId TransportRouter::MakeVertexId(const transport_catalogue::Stop& stop) noexcept
	{
		using namespace graph;
		if (stops_vertex_id_.count(stop.name) != 0)
		{
			return stops_vertex_id_.at(stop.name);
		}
		stops_vertex_id_[stop.name] = {};
		stops_vertex_id_[stop.name] = stops_vertex_id_.size() - 1;
		return stops_vertex_id_[stop.name];
	}

	double TransportRouter::CalculateWeightEdge(const transport_catalogue::Stop& from, const transport_catalogue::Stop& to) const noexcept
	{
		return  60.0 * transport_catalogue_.GetDistanceBetweenStops(from, to) / (1000.0 * routing_settings_.bus_velocity);
	}

}//namespace transport_router
#pragma once
#include "graph.h"
#include "router.h"
#include "domain.h"
#include "transport_catalogue.h"

#include <variant>
#include <memory>
#include <unordered_map>
#include <cmath>

namespace transport_router
{
	class TransportRouter
	{
	public:
		
		using BusGraph = graph::DirectedWeightedGraph<double>;
		using Router = graph::Router<double>;
		using StopName = std::string;
		using StopsVertexId = std::unordered_map<StopName, graph::VertexId>;
		using Vertices = std::vector<StopName>;
		using BusName = std::string;

		struct VertexInfo
		{
			std::string stop_name;
		};
		struct BusEdgeInfo
		{
			std::string bus_name;
			size_t span_count = 0;
			size_t number_edge = 0;
		};

		struct WaitAdgeInfo {};
		using EdgeInfo = std::variant<BusEdgeInfo, WaitAdgeInfo>;

		struct RouteInfo
		{
			double total_time = 0.;

			struct BusItem
			{
				std::string bus_name;
				double time = 0.;
				int span_count = 0;
			};

			struct WaitItem
			{
				std::string stop_name;
				double time = 0.;
			};

			using Item = std::variant<BusItem, WaitItem>;

			std::vector<Item> items;
		};

		explicit TransportRouter(const transport_catalogue::TransportCatalogue& tc
			, const domain::RoutingSettings& settings);		
		
		explicit TransportRouter(const transport_catalogue::TransportCatalogue& transport_catalogue
			, domain::RoutingSettings&& routing_settings
			, StopsVertexId&& stops_vertex_id, Vertices&& vertices_info
			, std::vector<EdgeInfo>&& adges_info, std::vector<graph::Edge<double>>&& edges
			, std::vector<std::vector<size_t>>&& incidence_lists
			, graph::Router<double>::RoutesInternalData&& data);		

		inline void CreateGraph() noexcept;

		std::optional<transport_router::TransportRouter::RouteInfo> FindRoute(const std::string& stop1, const std::string& stop2) const noexcept;	

		inline RouteInfo::WaitItem CreateWaitItem(const graph::EdgeId& edge) const noexcept;

		inline RouteInfo::BusItem CreateBusItem(const graph::EdgeId& edge, double time) const noexcept;

		StopsVertexId GetStopVertexId() const noexcept;
		
		Vertices GetVertexInfo()const noexcept;
		
		std::vector<EdgeInfo> GetVectorEdgeInfo() const noexcept;
		
		BusGraph GetGraph() const noexcept;
		
		Router GetRouter() const noexcept;			

	private:		

		inline graph::VertexId MakeVertexId(const transport_catalogue::Stop& stop) noexcept;		

		double CalculateWeightEdge(const transport_catalogue::Stop& from,
			const transport_catalogue::Stop& to) const noexcept;		

		template<class It>
		void FillGraph(It begin, It end, const std::string& bus_name) noexcept;	

		
		const transport_catalogue::TransportCatalogue& transport_catalogue_;
		domain::RoutingSettings routing_settings_;
		BusGraph graph_;
		std::shared_ptr<Router> router_ = nullptr;

		StopsVertexId stops_vertex_id_;
		Vertices vertices_info_;
		std::vector<EdgeInfo> edges_info_;		
	};

	template<class It>
	inline void TransportRouter::FillGraph(It begin, It end, const std::string& bus_name) noexcept
	{
		for (auto it_stop = begin; it_stop != end; ++it_stop)
		{
			graph::VertexId from = MakeVertexId(**it_stop);
			graph_.AddEdge({ from, from, { static_cast<double>(routing_settings_.bus_wait_time)} });
			const StopName& stop = (**it_stop).name;
			if (!std::count(vertices_info_.begin(), vertices_info_.end(), stop))
			{
				vertices_info_.push_back(stop);
			}
			stops_vertex_id_[stop] = from;
			size_t span_count = 0;
			const BusEdgeInfo bus_edge_info = { bus_name , span_count, edges_info_.size() };
			edges_info_.push_back(std::move(bus_edge_info));
			for (auto it = it_stop; it != end; ++it)
			{
				if (it + 1 != end)
				{
					graph::VertexId to{};
					double minutes_to_route = graph_.GetEdge(graph_.GetEdgeCount() - 1).weight;
					to = MakeVertexId(**(it + 1));
					minutes_to_route += CalculateWeightEdge(**it, **(it + 1));
					graph_.AddEdge({ from, to, minutes_to_route });

					const BusEdgeInfo bus_edge_info = { bus_name , ++span_count, edges_info_.size() };
					edges_info_.push_back(bus_edge_info);
				}
			}
		}
	}
}
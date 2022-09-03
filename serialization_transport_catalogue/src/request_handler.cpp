#include "request_handler.h"
#include "json_builder.h"
#include <fstream>

using namespace std::literals;
using namespace transport_catalogue;
using namespace renderer;
using namespace geo;

RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer) : db_(&db), renderer_(&renderer)
{}

RequestHandler::RequestHandler(const TransportCatalogue& db
	, const MapRenderer& renderer
	, const transport_router::TransportRouter& tr)
	:db_(&db)
	, renderer_(&renderer)
	, tr_(&tr)
{}

inline std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const noexcept
{
	std::optional<BusStat> result;
	BusStat stat;
	BusPtr bus = db_->FindBus(bus_name);
	if (bus)
	{

		std::unordered_set<std::string_view> seen_stops;

		std::optional<Coordinates> prev_pos;

		for (const auto& stop : bus->stops)
		{
			++stat.total_stops;

			if (seen_stops.count(stop->name) == 0)
			{
				++stat.unique_stops;
				seen_stops.insert(stop->name);
			}

			if (prev_pos)
			{
				stat.route_length += ComputeDistance(*prev_pos, stop->coordinates);
			}

			prev_pos = stop->coordinates;
		}

		for (auto it = bus->stops.begin(); it != bus->stops.end() - 1; ++it)
		{
			std::pair<StopPtr, StopPtr> temp{ *it, *(it + 1) };
			stat.distance += db_->FindDistanceBetweenStops(std::move(temp));
		}
		result = std::move(stat);
	}
	return result;
}

inline StopInfo RequestHandler::GetBusesByStop(const std::string_view& stop_name) const noexcept
{
	StopInfo result;
	std::unordered_set<transport_catalogue::BusPtr> temp;
	if (!db_->FindStop(stop_name))
	{
		return StopInfo();
	}

	if (db_->GetStopToBuses().find(db_->FindStop(stop_name))->second.size())
	{
		result.buses_for_stop = db_->GetStopToBuses().find(db_->FindStop(stop_name))->second;
		result.about = "Ok"s;
	}
	else
	{
		result.buses_for_stop = std::move(temp);
		result.about = "no buses"s;
	}
	return result;
}

const transport_router::TransportRouter& RequestHandler::GetTransportRouter() const noexcept
{
	return *tr_;
}

std::optional<std::string> RequestHandler::GetMap() const noexcept
{
	return renderer_->DocumentMapToString();
}


namespace stat_request
{

	inline json::Node MessageErrore(const domain::StatRequest& stat) noexcept
	{
		return json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(stat.id_request)
			.Key("error_message"s).Value("not found"s)
			.EndDict()
			.Build();
	}

	inline json::Node RepareReportRouter(const domain::StatRequest& stat, const std::optional<transport_router::TransportRouter::RouteInfo>& reports) noexcept
	{
		json::Builder report;
		report.StartDict().Key("items"s).StartArray();
		for (const auto& route : reports->items)
		{
			if (std::holds_alternative<transport_router::TransportRouter::RouteInfo::WaitItem>(route))
			{
				transport_router::TransportRouter::RouteInfo::WaitItem wait = std::get<transport_router::TransportRouter::RouteInfo::WaitItem>(route);
				report.StartDict().Key("stop_name"s).Value(std::move(std::string(wait.stop_name)))
					.Key("time"s).Value(wait.time)
					.Key("type"s).Value("Wait"s)
					.EndDict().StartDict();
			}
			else if (std::holds_alternative<transport_router::TransportRouter::RouteInfo::BusItem>(route))
			{
				transport_router::TransportRouter::RouteInfo::BusItem bus = std::get<transport_router::TransportRouter::RouteInfo::BusItem>(route);
				report.Key("bus"s).Value(std::move(std::string(bus.bus_name)))
					.Key("span_count"s).Value(bus.span_count)
					.Key("time"s).Value(bus.time)
					.Key("type"s).Value("Bus"s)
					.EndDict();
			}

		}
		return report.EndArray()
			.Key("request_id"s).Value(stat.id_request)
			.Key("total_time"s).Value(reports->total_time).EndDict().Build();
	}

	inline json::Node RepareMap(const RequestHandler& rh, const domain::StatRequest& stat) noexcept
	{
		return json::Builder{}.StartDict()
			.Key("map"s).Value(std::move(*rh.GetMap()))
			.Key("request_id"s).Value(stat.id_request).EndDict().Build();
	}

	inline json::Node RepareReportStop(const transport_catalogue::StopInfo& stop, const domain::StatRequest& stat) noexcept
	{
		std::set<std::string>name_bus;
		for (const auto& buses : stop.buses_for_stop)
		{
			name_bus.insert(buses->name);
		}

		json::Builder answer_stop;
		answer_stop.StartDict()
			.Key("buses"s).StartArray();

		for (auto name : name_bus)
		{
			answer_stop.Value(std::move(name));
		}

		return answer_stop.EndArray()
			.Key("request_id"s).Value(stat.id_request)
			.EndDict().Build();
	}

	inline json::Node RepareReportBus(const transport_catalogue::BusStat& bus, const domain::StatRequest& stat) noexcept
	{
		return json::Builder{}
			.StartDict()
			.Key("curvature"s).Value(bus.distance / bus.route_length)
			.Key("request_id"s).Value(stat.id_request)
			.Key("route_length"s).Value(bus.distance)
			.Key("stop_count"s).Value(bus.total_stops)
			.Key("unique_stop_count"s).Value(bus.unique_stops)
			.EndDict()
			.Build();
	}

	inline void StatRequestStop(const RequestHandler& rh, const domain::StatRequest& stat, json::Array& answers) noexcept
	{
		const transport_catalogue::StopInfo& stop = rh.GetBusesByStop(stat.name_type);
		if (stop.about == "not found"s)
		{
			answers.emplace_back(std::move(MessageErrore(stat)));
		}
		else
		{
			answers.emplace_back(std::move(RepareReportStop(stop, stat)));
		}
	}

	inline void StatRequestBus(const RequestHandler& rh, const domain::StatRequest& stat, json::Array& answers) noexcept
	{
		const std::optional<transport_catalogue::BusStat>& bus_stat = rh.GetBusStat(stat.name_type);
		if (bus_stat)
		{
			answers.emplace_back(std::move(RepareReportBus(bus_stat.value(), stat)));
		}
		else
		{
			answers.emplace_back(std::move(MessageErrore(stat)));
		}
	}
	
	inline void StatRequestRoute(const RequestHandler& rh, const domain::StatRequest& stat, json::Array& answers) noexcept
	{
		const std::optional<transport_router::TransportRouter::RouteInfo>& reports = rh.GetTransportRouter().FindRoute(stat.from, stat.to);
		if (reports)
		{
			answers.emplace_back(std::move(RepareReportRouter(stat, reports)));
		}
		else
		{
			answers.emplace_back(std::move(MessageErrore(stat)));
		}
	}

	inline json::Document PrepareDocument(const RequestHandler& rh, const std::vector<domain::StatRequest>& stat_requests)
	{
		json::Array answers;
		for (const auto& stat : stat_requests)
		{
			if (stat.type == "Bus"s)
			{
				StatRequestBus(rh, stat, answers);
			}
			else if (stat.type == "Stop"s)
			{
				StatRequestStop(rh, stat, answers);
			}
			else if (stat.type == "Map"s)
			{				
				answers.emplace_back(std::move(RepareMap(rh, stat)));
			}
			else if (stat.type == "Route"s)
			{
				StatRequestRoute(rh, stat, answers);
			}
			else
			{
				std::string file = __FILE__;
				std::string line = std::to_string(__LINE__);
				std::string function = __FUNCTION__;
				std::string error = "Incorrect input stat request in file: "s + file
					+ " in fuction "s + function + " in line: "s + line;
				throw ErrorMessage(error);
			}
		}		
		
		return json::Document(std::move(json::Node(std::move(answers))));
	}

	void PrintStatDoc(const RequestHandler& rh, const std::vector<domain::StatRequest>& stat_requests, std::ostream& out) noexcept
	{			
		json::Print(PrepareDocument(rh, stat_requests), out);		
	}
}//namespace stat_request

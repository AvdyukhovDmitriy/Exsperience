#include "json_reader.h"

using namespace std::literals;
using namespace request;

std::string ErrorMessage::what()
{
	return what_;
}

domain::BaseRequest RequestReader::ParseStop(const json::Dict& dict)
{
	domain::BaseRequest temp;
	temp.type = dict.at("type"s).AsString();
	temp.name_stop = dict.at("name"s).AsString();
	temp.latitude = dict.at("latitude"s).AsDouble();
	temp.longitude = dict.at("longitude"s).AsDouble();
	json::Dict distance_road = dict.at("road_distances"s).AsDict();
	for (const auto& [name_stop, distance] : distance_road)
	{
		domain::NearestStop ns = { name_stop, distance.AsInt() };
		temp.distance_to_nearest_stops.push_back(std::move(ns));
	}
	return temp;
}

domain::BaseRequest RequestReader::ParseBus(const json::Dict& dict)
{
	domain::BaseRequest temp;
	temp.type = dict.at("type"s).AsString();
	temp.name_bus = dict.at("name"s).AsString();
	temp.is_roundtrip = dict.at("is_roundtrip"s).AsBool();
	json::Array stops = dict.at("stops"s).AsArray();

	for (auto it = stops.begin(); it != stops.end(); ++it)
	{
		temp.stops_for_bus.push_back(it->AsString());
		temp.name_last_stop = temp.stops_for_bus.back();
	}
	if (!temp.is_roundtrip)
	{

		temp.name_last_stop = temp.stops_for_bus.back();
		temp.stops_for_bus.reserve(temp.stops_for_bus.size() * 2 - 1);
		temp.stops_for_bus.insert(temp.stops_for_bus.end(),
			std::next(temp.stops_for_bus.rbegin()), temp.stops_for_bus.rend());
	}
	return temp;
}

void RequestReader::CreateBaseRequest(const json::Document& doc)
{
	if (doc.GetRoot().AsDict().count("base_requests"s))
	{
		json::Array base_requests = doc.GetRoot().AsDict().at("base_requests"s).AsArray();

		for (auto it = base_requests.begin(); it != base_requests.end(); ++it)
		{
			if (it->AsDict().at("type"s).AsString() == "Stop"s)
			{
				base_request_.push_back(std::move(ParseStop(it->AsDict())));
			}
			else if (it->AsDict().at("type"s).AsString() == "Bus"s)
			{
				base_request_.push_back(std::move(ParseBus(it->AsDict())));
			}
		}
	}
}

void RequestReader::CreateStatRequest(const json::Document& doc)
{
	if (doc.GetRoot().AsDict().count("stat_requests"s))
	{
		json::Array stat_requests = doc.GetRoot().AsDict().at("stat_requests"s).AsArray();
		for (auto it = stat_requests.begin(); it != stat_requests.end(); ++it)
		{
			if (it->AsDict().at("type"s).AsString() == "Stop"s || it->AsDict().at("type").AsString() == "Bus"s)
			{
				stat_request_.push_back({ it->AsDict().at("id"s).AsInt()
					, it->AsDict().at("type"s).AsString()
					, it->AsDict().at("name"s).AsString() });
			}
			else if (it->AsDict().at("type"s).AsString() == "Map"s)
			{
				stat_request_.push_back({ it->AsDict().at("id").AsInt()
					, it->AsDict().at("type"s).AsString() });
			}
			else if (it->AsDict().at("type").AsString() == "Route"s)
			{
				stat_request_.push_back({ it->AsDict().at("id"s).AsInt()
					, it->AsDict().at("type"s).AsString()
					, {}
					, it->AsDict().at("from"s).AsString()
					, it->AsDict().at("to"s).AsString() });
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
	}
}

svg::Color RequestReader::AddColor(const json::Node& node)
{
	{
		svg::Color color;
		if (node.IsArray())
		{
			if (node.AsArray().size() == 3)
			{
				svg::Rgb rgb;
				rgb.red = node.AsArray()[0].AsInt();
				rgb.green = node.AsArray()[1].AsInt();
				rgb.blue = node.AsArray()[2].AsInt();
				color = rgb;
			}
			if (node.AsArray().size() == 4)
			{
				svg::Rgba rgba;
				rgba.red = node.AsArray()[0].AsInt();
				rgba.green = node.AsArray()[1].AsInt();
				rgba.blue = node.AsArray()[2].AsInt();
				rgba.opacity = node.AsArray()[3].AsDouble();
				color = rgba;
			}
		}
		if (node.IsString())
		{
			color = node.AsString();
		}
		return color;
	}
}

void request::RequestReader::CreatePath(const json::Document& doc)
{
	if (doc.GetRoot().AsDict().count("serialization_settings"s))
	{
		json::Dict serialization = doc.GetRoot().AsDict().at("serialization_settings"s).AsDict();
		to_file_ = serialization.at("file").AsString();
	}
}

const request::RequestReader::Path request::RequestReader::GetPath() const noexcept
{
	return to_file_;
}

void RequestReader::CreateRenderSettings(const json::Document& doc)
{
	if (doc.GetRoot().AsDict().count("render_settings"s))
	{
		json::Dict render_settings = doc.GetRoot().AsDict().at("render_settings"s).AsDict();
		render_settings_.width = render_settings.at("width").AsDouble();
		render_settings_.height = render_settings.at("height").AsDouble();
		render_settings_.padding = render_settings.at("padding").AsDouble();
		render_settings_.line_width = render_settings.at("line_width").AsDouble();
		render_settings_.stop_radius = render_settings.at("stop_radius").AsDouble();
		render_settings_.bus_label_font_size = render_settings.at("bus_label_font_size").AsDouble();
		render_settings_.bus_label_offset.lat = render_settings.at("bus_label_offset").AsArray().front().AsDouble();
		render_settings_.bus_label_offset.lng = render_settings.at("bus_label_offset").AsArray().back().AsDouble();
		render_settings_.stop_label_font_size = render_settings.at("stop_label_font_size").AsDouble();
		render_settings_.stop_label_offset.lat = render_settings.at("stop_label_offset").AsArray().front().AsDouble();
		render_settings_.stop_label_offset.lng = render_settings.at("stop_label_offset").AsArray().back().AsDouble();
		render_settings_.underlayer_width = render_settings.at("underlayer_width").AsDouble();
		render_settings_.underlayer_color = AddColor(render_settings.at("underlayer_color"));
		json::Array palette = render_settings.at("color_palette").AsArray();
		for (auto it = palette.begin(); it != palette.end(); ++it)
		{
			render_settings_.color_palette.push_back(AddColor(*it));
		}
	}
}

const std::vector<domain::BaseRequest>& RequestReader::GetBaseRequest() const noexcept
{
	return base_request_;
}

const std::vector<domain::StatRequest>& RequestReader::GetStatRequest() const noexcept
{
	return stat_request_;
}

const domain::RoutingSettings& RequestReader::GetRoutingSettings() const noexcept
{
	return routing_settings_;
}

const renderer::RenderSettings& RequestReader::GetRendereSettings() const noexcept
{
	return render_settings_;
}

RequestReader::RequestReader(std::istream& in)
{
	json::Document doc = json::Load(in);
	CreatePath(doc);
	CreateBaseRequest(doc);
	CreateStatRequest(doc);
	CreateRenderSettings(doc);
	CreateRoutingSettings(doc);
}

void RequestReader::CreateRoutingSettings(const json::Document& doc)
{
	if (doc.GetRoot().AsDict().count("routing_settings"s))
	{
		json::Dict routing_settings = doc.GetRoot().AsDict().at("routing_settings"s).AsDict();
		routing_settings_.bus_wait_time = routing_settings.at("bus_wait_time"s).AsInt();
		routing_settings_.bus_velocity = routing_settings.at("bus_velocity"s).AsDouble();
	}
}


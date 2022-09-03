#pragma once
#include "json.h"
#include "map_renderer.h"
#include "domain.h"

#include <exception>
#include <filesystem>

using namespace std::literals;
class ErrorMessage : public std::exception
{
public:
	ErrorMessage(const std::string& w) : what_(w) {}

	std::string what();

private:
	std::string what_;
};

namespace request
{
	class RequestReader
	{
	public:
		using Path = std::filesystem::path;

		RequestReader() = default;

		explicit RequestReader(std::istream& in);

		void CreateRoutingSettings(const json::Document& doc);						

		void CreateBaseRequest(const json::Document& doc);		

		void CreateStatRequest(const json::Document& doc);					

		void CreateRenderSettings(const json::Document& doc);

		inline domain::BaseRequest ParseStop(const json::Dict& dict);

		inline domain::BaseRequest ParseBus(const json::Dict& dict);

		const std::vector<domain::BaseRequest>& GetBaseRequest() const noexcept;

		const std::vector<domain::StatRequest>& GetStatRequest() const noexcept;

		const domain::RoutingSettings& GetRoutingSettings() const noexcept;

		const renderer::RenderSettings& GetRendereSettings() const noexcept;

		svg::Color AddColor(const json::Node& node);

		void CreatePath(const json::Document& doc);
		
		const Path GetPath() const noexcept;		

	private:
		Path to_file_;
		std::vector<domain::BaseRequest> base_request_;
		std::vector<domain::StatRequest> stat_request_;
		domain::RoutingSettings routing_settings_;
		renderer::RenderSettings render_settings_;
	};
}

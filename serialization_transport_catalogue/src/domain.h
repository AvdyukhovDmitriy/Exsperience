#pragma once
#include <string>
#include <vector>


namespace domain
{
	using NameStop = std::string;
	struct NearestStop
	{
		std::string name_nearest_stop{};
		int distance_to_nearest_stop = 0;
	};

	struct BaseRequest
	{
		std::string type{};

		std::string name_stop{};
		double latitude = 0.;
		double longitude = 0;
		std::vector<domain::NearestStop> distance_to_nearest_stops{};

		std::string name_bus{};
		std::vector<NameStop> stops_for_bus{};
		std::string name_last_stop{};
		bool is_roundtrip = false;
	};

	struct StatRequest
	{
		int id_request = 0;
		std::string type{};
		std::string name_type{};
		NameStop from{};
		NameStop to{};
	};

	struct RoutingSettings
	{
		int bus_wait_time = 0;
		double bus_velocity = 0;
	};
}












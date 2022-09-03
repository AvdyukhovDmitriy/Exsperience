#include <fstream>
#include <iostream>
#include <string_view>
#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "serialization.h"

#include <string>
#include "domain.h"

using namespace std;
using namespace transport_catalogue;

void PrintUsage(std::ostream& stream = std::cerr)
{
	stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[])
{
	if (argc != 2) 
	{
		PrintUsage();
		return 1;
	}

	const std::string_view mode(argv[1]);

	if (mode == "make_base"sv) 
	{
		std::unique_ptr<request::RequestReader> rr = std::make_unique<request::RequestReader>(std::cin);
		std::unique_ptr<transport_catalogue::TransportCatalogue> tc
			= std::make_unique<transport_catalogue::TransportCatalogue>(rr->GetBaseRequest());
		std::unique_ptr<serialization::Serialization>serializ = std::make_unique< serialization::Serialization>(*rr, *tc);
	}
	else if (mode == "process_requests"sv)
	{
		std::unique_ptr<request::RequestReader> rr = std::make_unique<request::RequestReader>(std::cin);
		std::unique_ptr<deserialization::Deserialization>deserializ = std::make_unique< deserialization::Deserialization>(*rr);
		deserializ->PrintStatRequest();
	}
	else
	{
		PrintUsage();
		return 1;
	}
	return 0;
}
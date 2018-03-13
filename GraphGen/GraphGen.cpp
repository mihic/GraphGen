
#include "stdafx.h" //precompiled header

//global vars
bool debug = false;

int main(int argc, const char* argv[])
{
	cxxopts::Options options("GraphGen", "Random Graph Generator");

	options.add_options()
		("d,debug", "Enable debugging")
		;
	auto result = options.parse(argc, argv);
	if (result.count("debug") > 0) {
		debug = true;
		std::cout << "debuging enabled" << std::endl;
	}
}


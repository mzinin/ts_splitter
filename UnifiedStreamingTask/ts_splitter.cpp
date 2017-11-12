#include "ts_splitter.hpp"

#include <iostream>


void TsSplitter::init(int argc, char** argv)
{
	programOptions_.reset(new ProgramOptions(argv[0]));

	try
	{
		programOptions_->init(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to parse command line arguments:\n" << e.what() << "\n" << std::endl;
	}
}

bool TsSplitter::run()
{
	if (programOptions_->helpRequested())
	{
		std::cout << programOptions_->usage() << std::endl;
		return false;
	}

	return true;
}

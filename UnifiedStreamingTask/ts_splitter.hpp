#pragma once

#include "program_options.hpp"

#include <memory>


class TsSplitter
{
public:
	void init(int argc, char** argv);
	bool run();

private:
	std::unique_ptr<ProgramOptions> programOptions_;
};

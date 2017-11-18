#include "error.hpp"
#include "output_name_generator.hpp"
#include "output_writer.hpp"
#include "payload_parser.hpp"
#include "ts_reader.hpp"
#include "ts_splitter.hpp"

#include <iostream>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif // _WIN32


void TsSplitter::init(int argc, char** argv)
{
    programOptions_.reset(new ProgramOptions(argv[0]));

    try
    {
        programOptions_->init(argc, argv);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: TsSplitter, failed to parse command line arguments:\n" << e.what() << "\n" << std::endl;
    }
}

bool TsSplitter::run()
{
    if (programOptions_->helpRequested())
    {
        std::cout << programOptions_->usage() << std::endl;
        return false;
    }

    try
    {
        openInput();
        splitInput();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what();
        return false;
    }

    return true;
}

void TsSplitter::openInput()
{
    const auto& fileName = programOptions_->inputName();

    // read from std::cin
    if (fileName.empty())
    {
#ifdef _WIN32
        // reopen stdin in binary mode for Windows
        _setmode(_fileno(stdin), _O_BINARY);
#endif
        return;
    }

    input_.reset(new std::ifstream(fileName, std::ifstream::in | std::ifstream::binary));
    if (!input_ || !input_->good())
        throw Error(Error::CONSTRUCTION_ERROR, "TsSplitter, failed to open input file '" + fileName + "'");
}

void TsSplitter::splitInput()
{
    OutputNameGenerator audioNameGenerator(programOptions_->audioOutputName());
    OutputNameGenerator videoNameGenerator(programOptions_->videoOutputName());

    using namespace std::placeholders;

    OutputWriter writer(std::clog, audioNameGenerator, videoNameGenerator);
    PayloadParser parser(std::clog, std::bind(&OutputWriter::write, std::ref(writer), _1));
    TsReader reader(input_ ? *input_ : std::cin,
                    std::clog,
                    std::bind(&PayloadParser::parse, std::ref(parser), _1));

    reader.readAll();
}

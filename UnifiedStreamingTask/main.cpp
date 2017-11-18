#include "ts_splitter.hpp"

#include <cstdlib>


int main(int argc, char** argv)
{
    TsSplitter splitter;
    splitter.init(argc, argv);
    const bool result = splitter.run();

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

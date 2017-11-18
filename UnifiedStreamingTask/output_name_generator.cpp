#include "output_name_generator.hpp"

#include <sstream>


OutputNameGenerator::OutputNameGenerator(const std::string& nameExample)
{
    // split name template into base name and extension
    const auto pos = nameExample.rfind('.');
    if (pos == std::string::npos)
    {
        baseName_ = nameExample;
    }
    else
    {
        baseName_ = nameExample.substr(0, pos);
        extension_ = nameExample.substr(pos + 1, nameExample.size() - pos - 1);
    }

    // check if name template requires first file name with number, i.e. ends with '_1'
    const auto size = baseName_.size();
    if (size > 2 &&
        baseName_[size - 2] == '_' &&
        baseName_[size - 1] == '1')
    {
        baseName_.resize(size - 2);
        firstFileWithNumber_ = true;
    }
}

std::string OutputNameGenerator::name(uint16_t fileNumber) const
{
    if (baseName_.empty())
        return std::string();

    std::ostringstream buffer;

    buffer << baseName_;
    if (fileNumber != 1 || firstFileWithNumber_)
        buffer << '_' << fileNumber;
    if (!extension_.empty())
        buffer << '.' << extension_;

    return buffer.str();
}
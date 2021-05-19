#pragma once
#include <filesystem>
#include "PagePresets.h"

static std::string RelativeURLToLocalPath(const std::string &inFileURL, const std::string &inRelativeURL)
{
    return std::filesystem::path(inFileURL) / inRelativeURL;
}

#pragma once
#include "PagePresets.h"
#include <filesystem>

static std::string RelativeURLToLocalPath(const std::string &inFileURL, const std::string &inRelativeURL)
{
    return std::filesystem::path(inFileURL) / inRelativeURL;
}

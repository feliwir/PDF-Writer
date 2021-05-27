/*
   Source File : JPEGImageInformation.h


   Copyright 2011 Gal Kahana PDFWriter

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


*/
#pragma once
#include <stdint.h>

namespace charta
{
struct JPEGImageInformation
{
    // basic image information
    long long SamplesWidth = 0;
    long long SamplesHeight = 0;
    int ColorComponentsCount = 0;

    // JFIF Information
    bool JFIFInformationExists = false;
    uint32_t JFIFUnit = 0;
    double JFIFXDensity = 0;
    double JFIFYDensity = 0;

    // Exif Information
    bool ExifInformationExists = false;
    uint32_t ExifUnit = 0;
    double ExifXDensity = 0;
    double ExifYDensity = 0;

    // Photoshop Information
    bool PhotoshopInformationExists = 0;
    // photoshop density is always pixel per inch
    double PhotoshopXDensity = 0;
    double PhotoshopYDensity = 0;
};
} // namespace charta
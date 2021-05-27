#pragma once
#include "PDFRectangle.h"

// This presets assume 72 pixel per inch
namespace charta::PagePresets
{
const PDFRectangle A4_Portrait(0, 0, 595, 842);
const PDFRectangle A4_Landscape(0, 0, 842, 595);

const PDFRectangle A5_Portrait(0, 0, 420, 595);
const PDFRectangle A5_Landscape(0, 0, 595, 420);
} // namespace charta::PagePresets
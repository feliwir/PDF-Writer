#pragma once

// allowing myself a shortcut here for the color type and date type. real life sample should use only model
// classes...and not of the PDF writing implementation...
#include "CMYKRGBColor.h"
#include "PDFDate.h"

#include <string>

class PDFComment
{
  public:
    PDFComment()
    {
        Time.SetToCurrentTime();
        ReplyTo = nullptr;
        FrameBoundings[0] = FrameBoundings[1] = FrameBoundings[2] = FrameBoundings[3] = 0.0;
    }

    std::string Text;
    std::string CommentatorName;
    double FrameBoundings[4];
    CMYKRGBColor Color;
    PDFDate Time;
    PDFComment *ReplyTo;
};

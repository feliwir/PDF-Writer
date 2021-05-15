#include "PDFComment.h"

PDFComment::PDFComment()
{
    Time.SetToCurrentTime();
    ReplyTo = nullptr;
    FrameBoundings[0] = FrameBoundings[1] = FrameBoundings[2] = FrameBoundings[3] = 0.0;
}

PDFComment::~PDFComment()
{
}

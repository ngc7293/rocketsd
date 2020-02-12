#include "modules/curse_logstream.h"

#include "modules/curse_window.h"

namespace modules { namespace curses {

CurseLogStream::CurseLogStream(CurseOutputWindow* window)
    : std::ostream(this)
    , window_(window)
{
}
    
CurseLogStream::~CurseLogStream() {}

int CurseLogStream::overflow(int c)
{
    window_->write(c);
    return 0;
}

}} // namespaces
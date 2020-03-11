#include "curse_logstream.h"

#include "curse_window.h"

namespace modules::curses {

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

} // namespaces
#ifndef CURSE_LOGSTREAM_H_
#define CURSE_LOGSTREAM_H_

#include <ostream>

#include <ncurses.h>

namespace modules { namespace curses {

class CurseOutputWindow;

class CurseLogStream : std::streambuf, public std::ostream {
private:
    CurseOutputWindow* window_;

public:
    CurseLogStream(CurseOutputWindow* window);
    ~CurseLogStream();

    int overflow(int c) override;
};

}} // namespaces

#endif
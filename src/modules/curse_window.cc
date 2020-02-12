#include "modules/curse_window.h"

#include <thread>

#include <ncurses.h>

namespace modules { namespace curses {

std::mutex mutex;

CurseOutputWindow::CurseOutputWindow(std::string name, int y, int x, int h, int w)
{
    window_ = (void*)newwin(h, w, y, x);
    name_ = name;

    getmaxyx((WINDOW*)window_, rows_, cols_);
    rows_ -= 2;
    cols_ -= 2;

    lines_.push_back("");
    refresh();
}

CurseOutputWindow::~CurseOutputWindow() {}

void CurseOutputWindow::write(char c)
{
    std::string& line = lines_.back();

    if (c != '\n') {
        line.push_back(c);
    }

    if (line.size() == cols_ || c == '\n') {
        lines_.push_back("");
        lines_.back().reserve(cols_);

        if (lines_.size() > rows_ + 1) {
            lines_.erase(lines_.begin());
        }

        refresh();
    }
}

void CurseOutputWindow::write(std::string str)
{
    if (lines_.back() == "") {
        lines_.back() = str;
    } else {
        lines_.push_back(str);
        lines_.push_back("");
        lines_.back().reserve(cols_);
    }

    if (lines_.size() > rows_) {
        lines_.erase(lines_.begin());
    }
    refresh();
}

void CurseOutputWindow::refresh()
{
    const std::lock_guard<std::mutex> lock(mutex);
    WINDOW* window = (WINDOW*)window_;

    werase(window);
    box(window, 0, 0);
    wmove(window, 0, 2);
    waddstr(window, name_.c_str());

    int i = 1;
    for (const std::string& line : lines_) {
        wmove(window, i, 1);
        waddstr(window, line.c_str());
        i++;
    }

    wrefresh(window);
}

CurseInputWindow::CurseInputWindow(int y, int x, int h, int w, std::function<std::string(std::string)> callback)
    : callback_(callback)
{
    window_ = (void*)newwin(h, w, y, x);

    getmaxyx((WINDOW*)window_, rows_, cols_);
    rows_ -= 2;
    cols_ -= 2;

    refresh();

    quit_ = false;
    new std::thread(&CurseInputWindow::run, this);
}

CurseInputWindow::~CurseInputWindow()
{
    quit_ = true;
}

void CurseInputWindow::run()
{
    chtype c;

    timeout(100);
    while (true) {
        if (quit_) {
            return;
        }

        if ((c = getch()) != EOF) {
            {
                const std::lock_guard<std::mutex> lock_line(line_mutex_);

                if (c == '\n') {
                    lines_.push_back(line_);
                    lines_.push_back(callback_(line_));
                    line_ = "";

                    if (lines_.size() > rows_ - 1) {
                        lines_.erase(lines_.begin());
                    }
                } else if (c == KEY_BACKSPACE) {
                    if (line_.size()) {
                        line_.pop_back();
                    }
                } else {
                    line_.push_back(c);
                }
            }
            refresh();
        }

    }
}

void CurseInputWindow::refresh()
{
    const std::lock_guard<std::mutex> lock(mutex);
    const std::lock_guard<std::mutex> lock_line(line_mutex_);

    WINDOW* window = (WINDOW*)window_;

    werase(window);
    box(window, 0, 0);
    wmove(window, 0, 2);
    waddstr(window, "Input");

    int i = 1;
    for (const std::string& line : lines_) {
        wmove(window, i + rows_ - lines_.size() - 1, 1);

        if (i % 2 == 0) {
            wattron(window, COLOR_PAIR(2));
            waddstr(window, "> ");
        } else {
            waddstr(window, "< ");
        }

        waddstr(window, line.c_str());
        wattroff(window, COLOR_PAIR(2));
        i++;
    }

    wmove(window, rows_, 1);
    waddstr(window, "> ");
    wattron(window, COLOR_PAIR(1));
    waddstr(window, line_.c_str());
    wattroff(window, COLOR_PAIR(1));
    wrefresh(window);
}

void init()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
}

void deinit()
{
    endwin();
}

void maxsize(int& y, int& x)
{
    getmaxyx(stdscr, y, x);
}

}} // namespaces
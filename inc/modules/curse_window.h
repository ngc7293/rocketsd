#ifndef CURSE_WINDOW_H_
#define CURSE_WINDOW_H_

#include <string>
#include <deque>
#include <mutex>
#include <functional>

namespace modules { namespace curses {

class CurseOutputWindow {
private:
    void* window_;
    std::string name_;
    int rows_, cols_;

    std::deque<std::string> lines_;

public:
    CurseOutputWindow(std::string name, int y, int x, int h, int w);
    ~CurseOutputWindow();

    void write(char c);
    void write(std::string str);

private:
    void refresh();
};

class CurseInputWindow {
private:
    void* window_;
    int rows_, cols_;

    std::deque<std::string> lines_;
    std::string line_;
    std::mutex line_mutex_;

    std::function<std::string(std::string)> callback_;
    bool quit_;

public:
    CurseInputWindow(int y, int x, int h, int w, std::function<std::string(std::string)> callback);
    ~CurseInputWindow();

    void run();

private:
    void refresh();
};

void init();
void deinit();
void maxsize(int& y, int& x);

}}

#endif
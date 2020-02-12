#ifndef CURSE_MODULE_H_
#define CURSE_MODULE_H_

#include "module.h"

#include <string>

#include "curse_window.h"
#include "curse_logstream.h"

namespace modules {

class CurseModule : public Module {
private:
    curses::CurseOutputWindow *log_, *output_;
    curses::CurseLogStream* logstream_;
    curses::CurseInputWindow *input_;

public:
    CurseModule(QObject* parent, ProtocolSP protocol);
    ~CurseModule();

    bool init(json& config) override;

    std::string type() const override { return "nCurses"; }

public slots:
    void onPacket(radio_packet_t) override;

private:
    std::pair<std::string, bool> onInput(std::string input);
};

} // namespaces

#endif
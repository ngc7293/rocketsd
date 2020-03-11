#ifndef CURSE_MODULE_H_
#define CURSE_MODULE_H_

#include "module.h"

#include <string>

namespace modules::curses {

class CurseOutputWindow;
class CurseInputWindow;
class CurseLogStream;

class CurseModule : public Module {
private:
    CurseOutputWindow *log_, *output_;
    CurseLogStream* logstream_;
    CurseInputWindow *input_;

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
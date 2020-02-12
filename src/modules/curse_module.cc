#include "modules/curse_module.h"

#include <string>
#include <sstream>

#include "log.h"

namespace modules {

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

CurseModule::CurseModule(QObject* parent, ProtocolSP protocol)
    : Module(parent, protocol)
{
}

CurseModule::~CurseModule()
{
    curses::deinit();
    delete log_;
    delete output_;
    delete logstream_;

}

bool CurseModule::init(json& config)
{
    int rows, cols, extra;

    curses::init();
    curses::maxsize(rows, cols);
    extra = rows % 3;

    log_ = new curses::CurseOutputWindow("Log", 0, 0, (rows - extra) / 3, cols);
    logstream_ = new curses::CurseLogStream(log_);
    Log::get().setStream(*logstream_);

    output_ = new curses::CurseOutputWindow("Packets", (rows - extra) / 3, 0, (rows - extra) / 3, cols);
    input_ = new curses::CurseInputWindow(2 * (rows - extra) / 3, 0, (rows - extra) / 3 + extra, cols, [this](std::string input) {
        return onInput(input);
    });

    return true;
}

void CurseModule::onPacket(radio_packet_t packet)
{
    std::stringstream ss;
    Node* node;
    Message* message;

    if ((node = (*protocol_)[packet.node]) == nullptr) {
        Log::warn("CuteModule") << "Could not find Node with id=" << packet.node << ": ignoring" << std::endl;
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        Log::warn("CuteModule") << "Could not find Message with id=" << packet.node << "for Node '" << node->name() << "': ignoring" << std::endl;
        return;
    }

    ss << "anirniq." << node->name() << "." << message->name() << " " << packet.payload.FLOAT;
    output_->write(ss.str());
}

std::string CurseModule::onInput(std::string input)
{
    trim(input);

    std::size_t pos = input.find_first_of(" ");
    if (pos == -1) {
        return "missing parameter";
    }

    if (input.find_last_of(" ") != pos) {
        return "too many parameters";
    }

    std::string name = input.substr(0, pos);
    std::string param = input.substr(pos + 1);

    return name + "=" + param;
}

} // namespaces
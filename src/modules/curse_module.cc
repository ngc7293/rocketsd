#include "modules/curse_module.h"

#include <string>
#include <sstream>

#include "log.h"
#include "util.h"

namespace modules {

namespace {

template <class T, class U>
T* find(U* container, std::string& name)
{
    auto iter = std::find_if(container->begin(), container->end(), [name](auto item) { return item.second->name() == name; });

    if (iter != container->end()) {
        return iter->second;
    }
    return nullptr;
}

bool getline_trim(std::stringstream& input, std::string& result)
{
    while (std::getline(input, result, ' ') && result == "");
    return result != "";
}

}

CurseModule::CurseModule(QObject* parent, ProtocolSP protocol)
    : Module(parent, protocol)
    , log_(nullptr)
    , output_(nullptr)
    , input_(nullptr)
{
}

CurseModule::~CurseModule()
{
    curses::deinit();
    if (log_) {
        delete log_;
        delete logstream_;
    }

    if (output_) {
        delete output_;
    }

    if (input_) {
        delete input_;
    }
}

bool CurseModule::init(json& config)
{
    bool log, output, input;

    int rows, cols, extra;
    int count = 0;

    curses::init();
    curses::maxsize(rows, cols);

    log = !has<bool>(config, "log") || config["log"].get<bool>();
    output = !has<bool>(config, "output") || config["output"].get<bool>();
    input = !has<bool>(config, "input") || config["input"].get<bool>();

    count = log + output + input;
    extra = rows % count;

    if (log) {
        log_ = new curses::CurseOutputWindow("Log", 0, 0, (rows - extra) / count, cols);
        logstream_ = new curses::CurseLogStream(log_);
        Log::get().setStream(*logstream_);
    }

    if (output) {
        output_ = new curses::CurseOutputWindow("Packets", (rows - extra) / count, 0, (rows - extra) / count, cols);
    }

    if (input) {
        input_ = new curses::CurseInputWindow((count - 1) * (rows - extra) / count, 0, (rows - extra) / count + extra, cols, [this](std::string input) {
            return onInput(input);
        });
    }

    Log::info("CurseModule") << "Successfully init'd CurseModule" << std::endl;
    curses::forceRefresh();
    return true;
}

void CurseModule::onPacket(radio_packet_t packet)
{
    if (!output_) {
        return;
    }

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

std::pair<std::string, bool> CurseModule::onInput(std::string input)
{
    std::stringstream ss(input);
    std::string name, param, temp;

    auto error = [](std::string str) { return std::make_pair(str, false); };

    if (!getline_trim(ss, name)) {
        return error("missing message id");
    } else if (!getline_trim(ss, param)) {
        return error("missing message id");
    } else if (getline_trim(ss, temp)) {
        return error("missing message id");
    }

    ss.str(name);
    ss.clear();
    if (!std::getline(ss, temp, '.')) {
        return error("unknown message id (" + name + ")");
    }

    if (temp == "anirniq") {
        std::getline(ss, temp, '.');
    }

    Node* node = find<Node, Protocol>(protocol_.get(), temp);
    if (!node) {
        return error("no such node (" + temp + ")");
    }

    temp = name.substr(ss.tellg());
    Message* message = find<Message, Node>(node, temp);
    if (!message) {
        return error("no such message (" + temp + ")");
    }

    radio_packet_t packet;
    packet.node = node->id();
    packet.message_id = message->id();

    if (message->type() == "float") {
        packet.payload.FLOAT = std::stod(param);
    } else if (message->type() == "int") {
        packet.payload.INT = std::stoi(param);
    } else {
        return error("unhandled format for message (" + temp + ")");
    }

    packet.checksum = radio_compute_crc(&packet);
    packetReady(packet);

    ss.str(std::string());
    ss.clear();
    ss << node->name() << " " << message->name() << " = " << param << " (CRC " << +packet.checksum << ")";
    return std::make_pair(ss.str(), true);
}

} // namespaces
#include <iostream>

#include "protocol_parser.h"

int main(int argc, char* argv[])
{
    ProtocolParser parser;
    Protocol* protocol = parser.parse("protocol.xml");

    for (const auto& node : *protocol) {
        std::cout << "<Node id=" << node.second->id() << " name=" << node.second->name() << ">" << std::endl;

        for (const auto& message : *node.second) {
            std::cout << "<Message id=" << message.second->id()
                << " name=" << message.second->name()
                << " type=" << message.second->type()
                << ">" << std::endl;
        }
    }

    return 0;
}

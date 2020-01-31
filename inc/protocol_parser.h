#ifndef PROTOCOL_PARSER_H_
#define PROTOCOL_PARSER_H_

#include "protocol.h"

#include <string>
#include <vector>

#include <QDomElement>
#include <QStringList>

class ProtocolParser {
private:
    std::string file_;
    Protocol* protocol_;
    Node* current_node_;
    std::vector<std::string> current_name_tokens_;

public:
    ProtocolParser();
    ~ProtocolParser();

    Protocol* parse(std::string file);

private:
    void parseChildren(QDomElement xml, QStringList validTags);
    bool requireAttributes(QDomElement xml, QStringList keys);

    void parseNode(QDomElement xml);
    void parseMessage(QDomElement xml);
    void parseMessageGroup(QDomElement xml);
    void parseProtocol(QDomElement xml);
};

#endif
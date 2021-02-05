#include <gtest/gtest.h>

#include <string>

#include <protocol/protocol.hh>
#include <protocol/protocol_parser.hh>
#include <protocol/name.hh>

const std::string test_xml = R"(
<protocol name="prot1">
    <node name="node1" id="1">
        <message_group name="group1">
            <message id="1" name="msg1" type="int" />
        </message_group>

        <message id="2" name="msg2" type="float" />
    </node>
    <node name="node2" id="2">
        <message id="1" name="msg1" type="bool" command="yes" />
        <message id="2" name="msg2" type="string" />
    </node>
</protocol>
)";

TEST(protocol, parse_succeeds)
{
    rocketsd::protocol::ProtocolParser parser;
    rocketsd::protocol::Protocol* protocol;

    protocol = parser.parse(test_xml);

    EXPECT_NE(protocol, nullptr);
}

TEST(protocol, parse_creates_all_documented_elements)
{
    rocketsd::protocol::ProtocolParser parser;
    rocketsd::protocol::Protocol* protocol;

    protocol = parser.parse(test_xml);

    EXPECT_EQ(protocol->size(), 2);
    EXPECT_EQ((*protocol)[1]->size(), 2);
    EXPECT_EQ((*protocol)[2]->size(), 2);
}

TEST(protocol, parse_populates_fields_correctly)
{
    rocketsd::protocol::ProtocolParser parser;
    rocketsd::protocol::Protocol* protocol;

    protocol = parser.parse(test_xml);

    EXPECT_EQ(protocol->name(), "prot1");
    EXPECT_EQ((*protocol)[1]->name(), "node1");
    EXPECT_EQ((*(*protocol)[1])[1]->name(), "group1.msg1");
    EXPECT_EQ((*(*protocol)[1])[1]->type(), "int");
}

TEST(protocol, to_cute_name_generates_correct_names)
{
    rocketsd::protocol::ProtocolParser parser;
    rocketsd::protocol::Protocol* protocol;

    protocol = parser.parse(test_xml);
    EXPECT_EQ("prot1.node1.group1.msg1", rocketsd::protocol::to_cute_name(protocol, 1, 1));
    EXPECT_EQ("prot1.node2.msg2", rocketsd::protocol::to_cute_name(protocol, 2, 2));
}

TEST(protocol, from_cute_name_finds_correct_nodes)
{
    rocketsd::protocol::ProtocolParser parser;
    rocketsd::protocol::Protocol* protocol;

    rocketsd::protocol::Node* node;
    rocketsd::protocol::Message* message;

    protocol = parser.parse(test_xml);
    EXPECT_TRUE(rocketsd::protocol::from_cute_name(protocol, "prot1.node1.msg2", &node, &message));

    EXPECT_EQ(node->name(), "node1");
    EXPECT_EQ(node->id(), 1);
    EXPECT_EQ(message->name(), "msg2");
    EXPECT_EQ(message->id(), 2);
    EXPECT_EQ(message->type(), "float");
}
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""XML to #defines compiler"""

import argparse
import sys
import xml.etree.ElementTree

from abc import abstractmethod


class Visitor:
    @abstractmethod
    def visitMessage(self, message):
        pass

    @abstractmethod
    def visitNode(self, node):
        pass

    @abstractmethod
    def visitProtocol(self, protocol):
        pass


class Message:
    def __init__(self, name: str, id: int):
        self.name = name
        self.id = id

    def accept(self, visitor: Visitor):
        visitor.visitMessage(self)

    def __repr__(self) -> str:
        return f'<Message id={self.id} name={self.name}>'

    @staticmethod
    def parse(xml: xml.etree.ElementTree.Element):
        if all([key in xml.attrib for key in ['id', 'name']]):
            if not xml.attrib['id'].isdigit():
                print('ID is not a digit', file=sys.stderr)
                return None

            return Message(xml.attrib['name'], int(xml.attrib['id']))
        else:
            print('Missing mandatory attributes for Message element', file=sys.stderr)
            return None


class Node:
    """An object that can emit messages"""
    def __init__(self, name: str, id: int):
        self.name = name
        self.id = id
        self.messages = []

    def add(self, message: Message):
        self.messages.append(message)

    def accept(self, visitor: Visitor):
        visitor.visitNode(self)

    def __repr__(self) -> str:
        return f'<Node id={self.id} name={self.name}>'

    @staticmethod
    def parse(xml: xml.etree.ElementTree.Element):
        if all([key in xml.attrib for key in ['id', 'name']]):
            if not xml.attrib['id'].isdigit():
                print('ID is not a digit', file=sys.stderr)
                return None

            node = Node(xml.attrib['name'], int(xml.attrib['id']))

            for messagexml in xml:
                if message := Message.parse(messagexml):
                    node.add(message)

            return node
        else:
            print('Missing mandatory attributes for Node element', file=sys.stderr)
            return None


class Protocol:
    """Root element for a Protocol definition"""
    def __init__(self, prefix=''):
        self.nodes = []
        self.prefix = prefix

    def add(self, node: Node):
        self.nodes.append(node)

    def accept(self, visitor: Visitor):
        visitor.visitProtocol(self)

    def __repr__(self) -> str:
        return f'<Protocol>'

    @staticmethod
    def parse(xml: xml.etree.ElementTree.Element):
        protocol = Protocol()

        if 'prefix' in xml.attrib:
            protocol.prefix = xml.attrib['prefix']

        for nodexml in xml:
            if node := Node.parse(nodexml):
                protocol.add(node)

        return protocol


class PrintVisitor(Visitor):
    """Debug visitor to print() a protocol and all subelements"""
    def visitMessage(self, message: Message):
        print('\t\t', message, sep='')

    def visitNode(self, node: Node):
        print('\t', node, sep='')
        for message in node.messages:
            message.accept(self)

    def visitProtocol(self, protocol: Protocol):
        print(protocol)
        for node in protocol.nodes:
            node.accept(self)


class MessageDefinePrinterVisitor(Visitor):
    """Output #define directives for Message IDs"""
    def __init__(self, outfile):
        super().__init__()
        self.outfile = outfile
        self.current_node = ''
        self.prefix = ''

    def visitMessage(self, message: Message):
        print('#define', f'{self.prefix}{self.current_node.upper()}_{message.name.upper()}_INDEX', message.id, file=self.outfile)

    def visitNode(self, node: Node):
        print('/*', node.name.capitalize(), '*/', file=self.outfile)
        self.current_node = node.name
        for message in node.messages:
            message.accept(self)
        print(file=self.outfile)

    def visitProtocol(self, protocol: Protocol):
        if prefix := protocol.prefix:
            self.prefix = prefix + '_'

        print('/** CAN Message IDs **/', file=self.outfile)
        for node in protocol.nodes:
            node.accept(self)


def compile_c(args):
    """Compile C-language output"""
    if outfile := open(args.c_out, 'w'):
        if args.prepend:
            outfile.write(open(args.prepend, 'r').read())

        visitor = MessageDefinePrinterVisitor(outfile)
        protocol.accept(visitor)

        if args.append:
            outfile.write(open(args.append, 'r').read())


def main(args):
    file = open('protocol.xml', 'r')
    if not file:
        return -1

    root = xml.etree.ElementTree.fromstring(file.read())
    protocol = Protocol.parse(root)

    if args.c_out:
        compile_c(args)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', action='store', required=True, type=str, help='Input protocol XML file')
    parser.add_argument('--c_out', action='store', required=True, type=str, help='C-language Output file')
    parser.add_argument('-B', '--prepend', action='store', required=False, type=str, help='File to preprend to output')
    parser.add_argument('-A', '--append', action='store', required=False, type=str, help='File to apprend to output')
    exit(main(parser.parse_args()))

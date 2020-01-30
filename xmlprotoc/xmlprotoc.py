#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""XML to #defines compiler"""

import argparse
import sys
import xml.etree.ElementTree


class Message:
    def __init__(self, name, id):
        self.name = name
        self.id = id

    def accept(self, visitor):
        visitor.visitMessage(self)

    def __repr__(self):
        return f'<Message id={self.id} name={self.name}>'

    @staticmethod
    def parse(xml):
        if all([key in xml.attrib for key in ['id', 'name']]):
            if not xml.attrib['id'].isdigit():
                print('ID is not a digit', file=sys.stderr)
                return None

            return Message(xml.attrib['name'], int(xml.attrib['id']))
        else:
            print('Missing mandatory attributes for Message element', file=sys.stderr)
            return None


class MessageGroup:
    """A group of messages that share a name token"""
    def __init__(self, name):
        self.name = name
        self.childs = []

    def add(self, child):
        self.childs.append(child)

    def accept(self, visitor):
        visitor.visitMessageGroup(self)

    def __repr__(self):
        return f'<MessageGroup name={self.name}>'

    @staticmethod
    def parse(xml):
        if 'name' in xml.attrib:
            group = MessageGroup(xml.attrib['name'])

            for subxml in xml:
                child = None
                if subxml.tag == 'message':
                    child = Message.parse(subxml)

                if subxml.tag == 'message_group':
                    child = MessageGroup.parse(subxml)

                if child:
                    group.add(child)

            return group
        else:
            print('Missing mandatory attributes for MessageGroup element', file=sys.stderr)
            return None


class Node:
    """An object that can emit messages"""
    def __init__(self, name, id):
        self.name = name
        self.id = id
        self.childs = []

    def add(self, child):
        self.childs.append(child)

    def accept(self, visitor):
        visitor.visitNode(self)

    def __repr__(self):
        return f'<Node id={self.id} name={self.name}>'

    @staticmethod
    def parse(xml):
        if all([key in xml.attrib for key in ['id', 'name']]):
            if not xml.attrib['id'].isdigit():
                print('ID is not a digit', file=sys.stderr)
                return None

            node = Node(xml.attrib['name'], int(xml.attrib['id']))

            for subxml in xml:
                child = None
                if subxml.tag == 'message':
                    child = Message.parse(subxml)

                if subxml.tag == 'message_group':
                    child = MessageGroup.parse(subxml)

                if child:
                    node.add(child)

            return node
        else:
            print('Missing mandatory attributes for Node element', file=sys.stderr)
            return None


class Protocol:
    """Root element for a Protocol definition"""
    def __init__(self, prefix=''):
        self.nodes = []
        self.prefix = prefix

    def add(self, node):
        self.nodes.append(node)

    def accept(self, visitor):
        visitor.visitProtocol(self)

    def __repr__(self):
        return f'<Protocol>'

    @staticmethod
    def parse(xml):
        protocol = Protocol()

        if 'prefix' in xml.attrib:
            protocol.prefix = xml.attrib['prefix']

        for subxml in xml:
            node = Node.parse(subxml)
            if node:
                protocol.add(node)

        return protocol


class PrintVisitor:
    """Debug visitor to print() a protocol and all subelements"""
    def visitMessage(self, message):
        print('\t' * (self.depth + 1), message, sep='')

    def visitMessageGroup(self, group):
        self.depth += 1
        print('\t' * self.depth, group, sep='')
        for child in group.childs:
            child.accept(self)
        self.depth -= 1

    def visitNode(self, node):
        self.depth += 1
        print('\t' * self.depth, node, sep='')
        for child in node.childs:
            child.accept(self)
        self.depth -= 1

    def visitProtocol(self, protocol):
        self.depth = 0
        print(protocol)
        for node in protocol.nodes:
            node.accept(self)


class MessageDefinePrinterVisitor:
    """Output #define directives for Message IDs"""
    def __init__(self, outfile):
        super().__init__()
        self.outfile = outfile
        self.prefix = ''
        self.groups = []

    def visitMessage(self, message):
        print('#define', f'{self.prefix}{"_".join(self.groups).upper()}_{message.name.upper()}_INDEX', message.id, file=self.outfile)

    def visitMessageGroup(self, group):
        self.groups.append(group.name)
        for child in group.childs:
            child.accept(self)
        self.groups.pop()

    def visitNode(self, node):
        print('/*', node.name.capitalize(), '*/', file=self.outfile)
        self.groups = [node.name]
        for child in node.childs:
            child.accept(self)
        print(file=self.outfile)

    def visitProtocol(self, protocol):
        prefix = protocol.prefix
        if prefix:
            self.prefix = prefix + '_'

        print(f'/** {prefix} Message IDs **/', file=self.outfile)
        for node in protocol.nodes:
            node.accept(self)


def compile_c(protocol, args):
    """Compile C-language output"""
    outfile = open(args.c_out, 'w')
    if outfile:
        if args.prepend:
            outfile.write(open(args.prepend, 'r').read())

        visitor = MessageDefinePrinterVisitor(outfile)
        protocol.accept(visitor)

        if args.append:
            outfile.write(open(args.append, 'r').read())


def main(args):
    file = open(args.i, 'r')
    if not file:
        return -1

    root = xml.etree.ElementTree.fromstring(file.read())
    protocol = Protocol.parse(root)

    if args.c_out:
        compile_c(protocol, args)

    if args.print:
        protocol.accept(PrintVisitor())


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', action='store', required=True, type=str, help='Input protocol XML file')
    parser.add_argument('--c_out', action='store', required=False, type=str, help='C-language Output file')
    parser.add_argument('-B', '--prepend', action='store', required=False, type=str, help='File to preprend to output')
    parser.add_argument('-A', '--append', action='store', required=False, type=str, help='File to apprend to output')
    parser.add_argument('--print', action='store_true', required=False, help='Debug print')
    exit(main(parser.parse_args()))

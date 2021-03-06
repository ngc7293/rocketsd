import sys

from log import fatal
from util import isonow, joinstr
from version import __version__


class MessageDefinePrinterVisitor:
    """Output #define directives for Message IDs"""
    def __init__(self, outfile):
        super().__init__()
        self.outfile = outfile
        self.prefix = ''

    def visitMessage(self, message):
        name = message.fqn().replace(self.name, self.prefix).replace('.', '_')
        print('#define', f'{name.upper()}_INDEX', message.id, file=self.outfile)  # noqa

    def visitMessageGroup(self, group):
        for child in group.childs:
            child.accept(self)

    def visitNode(self, node):
        print('/*', node.name.capitalize().replace('_', ' '), '*/', file=self.outfile)  # noqa
        for child in node.childs:
            child.accept(self)
        print(file=self.outfile)

    def visitProtocol(self, protocol):
        self.name = protocol.name
        self.prefix = protocol.c_prefix

        print(f'/** {self.prefix} Message IDs **/', file=self.outfile)
        for node in protocol.childs:
            node.accept(self)


def compile(protocol, args):
    """Compile C-language output"""
    if args.out == '-':
        outfile = sys.stdout
    else:
        outfile = open(args.out, 'w')

    if not outfile:
        fatal('compile', f'Could not open {args.out}')

    outfile.write(f'/** Generated by XMLProtoc {joinstr(".", *__version__)} on {isonow()} */\n')  # noqa

    if args.prepend:
        outfile.write(open(args.prepend, 'r').read())

    protocol.accept(MessageDefinePrinterVisitor(outfile))

    if args.append:
        outfile.write(open(args.append, 'r').read())

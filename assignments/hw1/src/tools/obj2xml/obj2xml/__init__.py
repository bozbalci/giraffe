import sys

from obj2xml.core import Obj2Xml

if __name__ == '__main__':
    obj2xml = Obj2Xml(sys.argv[1])
    obj2xml.run()

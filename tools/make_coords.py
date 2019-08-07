#!/usr/bin/env python3

from os.path import dirname, realpath, join, basename
from glob import glob
from lxml import etree
import re

ROOT_DIR = realpath(join(dirname(__file__), '..'))

XPATH_NAMESPACES = {
    'svg': "http://www.w3.org/2000/svg",
    'sodipodi': "http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd",
    'inkscape': "http://www.inkscape.org/namespaces/inkscape",
    'xlink': "http://www.w3.org/1999/xlink",
    're': "http://exslt.org/regular-expressions",
}


find = etree.XPath(".//svg:g[@id][@transform]", namespaces=XPATH_NAMESPACES)

outfile = open(join(ROOT_DIR, 'src', 'coords.hpp'), 'w')
outfile.write("#pragma once\n\n")
outfile.write("using namespace rack;\n\n\n")


class Coordinate:
    def __init__(self, id, x, y):
        self.name = None
        self.type = "POS"

        res = re.match("^([A-Z_]+)_(START|STEP)$", id)
        if res:
            self.name = res.group(1)
            self.type = res.group(2)

        if not self.name:
            res = re.match("^([A-Z_]+)__([0-9]+)$", id)
            if res:
                self.name = res.group(1)
                self.type = "LIST"
                self.idx = int(res.group(2))

        if not self.name and re.match("^[A-Z_]+$", id):
            self.name = id

        if not self.name:
            return

        self.x = x
        self.y = y
        self.w = None
        self.h = None


files = glob(join(ROOT_DIR, 'res', '*.svg'))
for fileName in files:
    moduleName = basename(fileName).split('.')[0]
    prefix = moduleName.upper()

    parser = etree.XMLParser()
    svgTree = etree.parse(fileName)
    groups = find(svgTree)

    coords = {}
    listCoords = {}

    outfile.write("// %s\n" % moduleName)

    for group in groups:
        id = group.attrib['id']
        if re.match("g[0-9]+", id):
            continue

        res = re.match("translate\(([0-9.-]+)\s*,\s*([-0-9.]+)\)", group.attrib['transform'])
        if not res:
            continue

        x = float(res.group(1))
        y = float(res.group(2))

        c = Coordinate(id, x, y)
        if c.name:
            if c.type == "LIST":
                if not c.name in listCoords:
                    listCoords[c.name] = []
                listCoords[c.name].append(c)
            else:
                if c.type == "POS":
                    for el in list(group):
                        if el.attrib['id'] == "%s_SIZE" % c.name and "width" in el.attrib and "height" in el.attrib:
                            c.w = float(el.attrib['width'])
                            c.h = float(el.attrib['height'])
                coords[id] = c

    for id, coord in coords.items():
        if coord.type == "STEP":
            start = coords["%s_START" % coord.name]
            x, y = coord.x - start.x, coord.y - start.y
        else:
            x, y = coord.x, coord.y
        outfile.write("#define %s_%s_%s mm2px(Vec(%.3f, %.3f))\n" % (prefix, coord.name, coord.type, x, y))
        if coord.w and coord.h:
            outfile.write("#define %s_%s_SIZE mm2px(Vec(%.3f, %.3f))\n" % (prefix, coord.name, coord.w, coord.h))

    outfile.write("\n")

    for id, cl in listCoords.items():
        cl.sort(key=lambda c: c.idx)
        outfile.write("static const Vec %s_%s_POS[%d] = {\n" % (prefix, id, len(cl)))
        for c in cl:
            outfile.write("    mm2px(Vec(%.3f, %.3f)),\n" % (c.x, c.y))
        outfile.write("};\n\n")

    outfile.write("\n")




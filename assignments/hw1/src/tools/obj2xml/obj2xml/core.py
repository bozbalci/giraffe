from obj2xml.readers import WavefrontObjReader
from obj2xml.writers import Ceng477XmlWriter


class Obj2Xml:
    def __init__(self, input_filename, output_filename):
        self.input_filename = input_filename
        self.output_filename = output_filename

    def run(self):
        reader = WavefrontObjReader(self.input_filename)
        reader.read()
        vertex_data, faces = reader.get_vertex_data(), reader.get_faces()
        writer = Ceng477XmlWriter(self.output_filename, vertex_data, faces)
        writer.write()

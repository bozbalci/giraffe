from obj2xml.readers import WavefrontObjReader
from obj2xml.writers import Ceng477XmlWriter, Ceng477CameraXmlWriter


class Obj2Xml:
    def __init__(self, input_filename):
        self.input_filename = input_filename
        self.image_prefix = self.input_filename.split('.')[0]
        self.output_filename = f'{self.image_prefix}.xml'

    def run(self):
        # reader = WavefrontObjReader(self.input_filename)
        # reader.read()
        # vertex_data, objects = reader.get_vertex_data(), reader.get_objects()
        # writer = Ceng477XmlWriter(self.output_filename, vertex_data, objects, image_prefix=self.image_prefix)
        # writer.write()
        Ceng477CameraXmlWriter('out.json').write()

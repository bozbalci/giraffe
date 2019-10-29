import xml.etree.cElementTree as ET

from obj2xml.utils import generate_cameras


class Ceng477XmlWriter:
    def __init__(self, filename, vertex_data, faces):
        self.filename = filename

        self.max_recursion_depth = "6"
        self.intersection_test_epsilon = "1e-6"
        self.background_color = "0 0 0"

        # self.cameras = [
        #     {
        #         "id": '1',
        #         "position": "0 5 25",
        #         "gaze": "0 0 -1",
        #         "up": "0 1 0",
        #         "near_plane": "-1 1 -1 1",
        #         "near_distance": "1",
        #         "image_resolution": "800 800",
        #         "image_name": "giraffe_1.ppm",
        #     },
        # ]
        self.cameras = generate_cameras(
            count=2,
            position_start='0 0.10 2',
            position_end='0 0.10 1.5',
            gaze_start='0 0 -1',
            up_start='0 1 0',
            near_plane='-0.12 0.12 -0.12 0.12',
            near_distance='1.8',
            near_distance_end='0.9',
            image_resolution='400 400',
            image_prefix='dragon_lowres'
        )

        self.lights = [
            {
                'type': 'ambient',
                'value': '25 25 25'
            },
            {
                'type': 'point',
                'id': '1',
                'position': '0 30 50',
                'intensity': '1000000 1000000 1000000',
            },
        ]

        self.materials = [
            {
                'id': '1',
                'ambient_reflectance': '1 1 1',
                'diffuse_reflectance': '1 1 1',
                'specular_reflectance': '1 1 1',
                'phong_exponent': '1',
            },
        ]

        self.vertex_data = vertex_data

        self.objects = [
            {
                'type': 'mesh',
                'material': '1',
                'id': '1',
                'faces': faces,
            },
        ]

    def write(self):
        scene = ET.Element("Scene")

        max_recursion_depth = ET.SubElement(scene, "MaxRecursionDepth")
        max_recursion_depth.text = self.max_recursion_depth

        intersection_test_epsilon = ET.SubElement(scene, "IntersectionTestEpsilon")
        intersection_test_epsilon.text = self.intersection_test_epsilon

        background_color = ET.SubElement(scene, "BackgroundColor")
        background_color.text = self.background_color

        cameras = ET.SubElement(scene, "Cameras")
        for camera in self.cameras:
            el = ET.SubElement(cameras, "Camera", id=camera['id'])
            position = ET.SubElement(el, "Position")
            position.text = camera["position"]
            gaze = ET.SubElement(el, "Gaze")
            gaze.text = camera["gaze"]
            up = ET.SubElement(el, "Up")
            up.text = camera["up"]
            near_plane = ET.SubElement(el, "NearPlane")
            near_plane.text = camera["near_plane"]
            near_distance = ET.SubElement(el, "NearDistance")
            near_distance.text = camera["near_distance"]
            image_resolution = ET.SubElement(el, "ImageResolution")
            image_resolution.text = camera["image_resolution"]
            image_name = ET.SubElement(el, "ImageName")
            image_name.text = camera["image_name"]

        lights = ET.SubElement(scene, "Lights")
        for light in self.lights:
            if light['type'] == 'ambient':
                el = ET.SubElement(lights, 'AmbientLight')
                el.text = light['value']
            elif light['type'] == 'point':
                el = ET.SubElement(lights, "PointLight", id=light['id'])
                position = ET.SubElement(el, "Position")
                position.text = light['position']
                intensity = ET.SubElement(el, "Intensity")
                intensity.text = light['intensity']

        materials = ET.SubElement(scene, "Materials")
        for material in self.materials:
            el = ET.SubElement(materials, "Material", id=material['id'])
            ambient_reflectance = ET.SubElement(el, "AmbientReflectance")
            ambient_reflectance.text = material['ambient_reflectance']
            diffuse_reflectance = ET.SubElement(el, "DiffuseReflectance")
            diffuse_reflectance.text = material['diffuse_reflectance']
            specular_reflectance = ET.SubElement(el, "SpecularReflectance")
            specular_reflectance.text = material['specular_reflectance']
            phong_exponent = ET.SubElement(el, "PhongExponent")
            phong_exponent.text = material['phong_exponent']

        vertex_data = ET.SubElement(scene, "VertexData")
        vertex_data.text = '\n' + '\n'.join(self.vertex_data) + '\n'

        objects = ET.SubElement(scene, "Objects")
        for obj in self.objects:
            if obj['type'] == 'mesh':
                el = ET.SubElement(objects, "Mesh", id=obj['id'])
                material = ET.SubElement(el, "Material")
                material.text = obj['material']
                faces = ET.SubElement(el, "Faces")
                faces.text = '\n' + '\n'.join(obj['faces']) + '\n'

        tree = ET.ElementTree(scene)
        tree.write(self.filename)

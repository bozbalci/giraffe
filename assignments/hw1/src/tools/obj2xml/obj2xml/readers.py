class WavefrontObjReader:
    def __init__(self, filename):
        self.filename = filename

        self.vertex_data = []
        self.faces = []

    def read(self):
        with open(self.filename, 'r') as infile:
            for line in infile:
                tokens = line.strip().split(' ')
                attr_type = tokens[0]

                if attr_type == 'v':
                    x = tokens[1]
                    y = tokens[2]
                    z = tokens[3]
                    self.vertex_data.append(f'{x} {y} {z}')
                elif attr_type == 'f':
                    v1 = tokens[1].split('/')[0]
                    v2 = tokens[2].split('/')[0]
                    v3 = tokens[3].split('/')[0]
                    self.faces.append(f'{v1} {v2} {v3}')

    def get_vertex_data(self):
        return self.vertex_data

    def get_faces(self):
        return self.faces

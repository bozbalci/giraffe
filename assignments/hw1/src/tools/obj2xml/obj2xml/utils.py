import json
# import numpy as np
from math import pi, sin, cos, sqrt


def generate_cameras(count=300,
                     position_start='0.0 15.5 20.0',
                     gaze_start='0.0 -0.6 -0.8',
                     up_start='0 0.8 -0.6',
                     near_plane='-0.8 0.8 -0.45 0.45',
                     near_distance='1.8',
                     image_resolution='3840 2160',
                     image_prefix='chess'):
    cameras = []

    pos_start = tuple(map(float, position_start.split(' ')))
    gz_start = tuple(map(float, gaze_start.split(' ')))
    U_start = tuple(map(float, up_start.split(' ')))

    radius = pos_start[2]

    pos_norm = sqrt(pos_start[0]**2+pos_start[1]**2+pos_start[2]**2)

    angle = 0
    angle_diff = 2*pi/count

    for i in range(count):
        id_ = i + 1
        pos = [radius*sin(angle), pos_start[1], radius*cos(angle)]
        gaze = list(map(lambda x: -x/pos_norm, pos))

        up = (U_start[0] * cos(angle) + U_start[2] * sin(angle),
              U_start[1],
              U_start[0] * -sin(angle) + U_start[2] * cos(angle))

        # NP_gaze = np.array([gaze[0], gaze[1], gaze[2]])
        # NP_up = np.array([up[0], up[1], up[2]])
        # NP_gaze_DOT_up = np.dot(NP_gaze, NP_up)
        # NP_gaze_CROSS_up = np.linalg.norm(np.cross(NP_gaze, NP_up))
        # print(f'GAZE dot UP = {NP_gaze_DOT_up}')
        # print(f'GAZE cross UP = {NP_gaze_CROSS_up}')
        # print('-------------------------------------')


        cameras.append({
            'id': str(id_),
            'position': ' '.join((str(pos[0]),
                                  str(pos[1]),
                                  str(pos[2]))),
            'gaze': ' '.join((str(gaze[0]),
                              str(gaze[1]),
                              str(gaze[2]))),
            'up': ' '.join((str(up[0]),
                            str(up[1]),
                            str(up[2]))),
            'near_plane': near_plane,
            'near_distance': str(float(near_distance)),
            'image_resolution': image_resolution,
            'image_name': f'{image_prefix}_{id_:04}.ppm',
        })
        angle += angle_diff

    return cameras


if __name__ == '__main__':
    # generate_cameras()
    print(json.dumps(generate_cameras()))

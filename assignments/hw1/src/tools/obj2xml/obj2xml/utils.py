def generate_cameras(count=1,
                     position_start='0 5 25',
                     position_end=None,
                     gaze_start='0 0 -1',
                     gaze_end=None,
                     up_start='0 1 0',
                     up_end=None,
                     near_plane='-1 1 -1 1',
                     near_distance='1',
                     near_distance_end=None,
                     image_resolution='800 800',
                     image_prefix='giraffe'):
    cameras = []

    pos_start = tuple(map(float, position_start.split(' ')))
    if position_end is None:
        position_end = position_start
    pos_end = tuple(map(float, position_end.split(' ')))
    pos_difference = ((pos_end[0] - pos_start[0])/count,
                      (pos_end[1] - pos_start[1])/count,
                      (pos_end[2] - pos_start[2])/count)

    gz_start = tuple(map(float, gaze_start.split(' ')))
    if gaze_end is None:
        gaze_end = gaze_start
    gz_end = tuple(map(float, gaze_end.split(' ')))
    gz_difference = ((gz_end[0] - gz_start[0])/count,
                      (gz_end[1] - gz_start[1])/count,
                      (gz_end[2] - gz_start[2])/count)

    U_start = tuple(map(float, up_start.split(' ')))
    if up_end is None:
        up_end = up_start
    U_end = tuple(map(float, up_end.split(' ')))
    U_difference = ((U_end[0] - U_start[0])/count,
                     (U_end[1] - U_start[1])/count,
                     (U_end[2] - U_start[2])/count)

    if near_distance_end is None:
        near_distance_end = near_distance
    near_difference = (float(near_distance_end) - float(near_distance))/count

    for i in range(count):
        id_ = i + 1

        cameras.append({
            'id': str(id_),
            'position': ' '.join((str(pos_start[0] + pos_difference[0]*i),
                                  str(pos_start[1] + pos_difference[1]*i),
                                  str(pos_start[2] + pos_difference[2]*i))),
            'gaze': ' '.join((str(gz_start[0] + gz_difference[0]*i),
                              str(gz_start[1] + gz_difference[1]*i),
                              str(gz_start[2] + gz_difference[2]*i))),
            'up': ' '.join((str(U_start[0] + U_difference[0]*i),
                            str(U_start[1] + U_difference[1]*i),
                            str(U_start[2] + U_difference[2]*i))),
            'near_plane': near_plane,
            'near_distance': str(float(near_distance) + near_difference*i),
            'image_resolution': image_resolution,
            'image_name': f'{image_prefix}_{id_:04}.ppm',
        })

    return cameras

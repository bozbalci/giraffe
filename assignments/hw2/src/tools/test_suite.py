import json
import subprocess
import sys
import threading
import time

class ComparisonMetric:
    AE = 'AE'           # absolute error count
    DSSIM = 'DSSIM'     # structural dissimilarity index
    FUZZ = 'FUZZ'       # mean color distance
    MAE = 'MAE'         # mean absolute error
    MEPP = 'MEPP'       # mean error per pixel
    MSE = 'MSE'         # mean error squared
    NCC = 'NCC'         # normalized cross correlation
    PAE = 'PAE'         # peak absolute
    PHASH = 'PHASH'     # perceptual hash for sRGB and HCLp
    PSNR = 'PSNR'       # peak signal to noise ratio
    RMSE = 'RMSE'       # root mean squared
    SSIM = 'SSIM'       # structural similarity index


RASTERIZER = './rasterizer'
COMPARE = 'compare'
OUTPUT_FILENAME = 'tests.json'


TS_CLIPPING = [
    {
        'input': 'examples/clipping_example/empty_box_clipped.xml',
        'outputs': {
            'empty_box_clipped_{}.ppm.png'.format(i):
                'examples/clipping_example/empty_box_clipped_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
    },
]

TS_NO_CULLING = [
    {
        'input': 'examples/culling_disabled_inputs/empty_box.xml',
        'outputs': {
            'empty_box_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/empty_box/empty_box_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4, 5, 6, 7, 8, 'example']
        },
    },
    {
        'input': 'examples/culling_disabled_inputs/filled_box.xml',
        'outputs': {
            'filled_box_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/filled_box/filled_box_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4, 5, 6, 7, 8, 'example']
        },
    },
    {
        'input': 'examples/culling_disabled_inputs/flag_brazil.xml',
        'outputs': {
            'flag_brazil_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/flag_brazil/flag_brazil_{}.ppm.png'.format(i)
            for i in [1, 2, 'final']
        },
    },
    {
        'input': 'examples/culling_disabled_inputs/flag_turkey.xml',
        'outputs': {
            'flag_turkey_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/flag_turkey/flag_turkey_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
    },
    {
        'input': 'examples/culling_disabled_inputs/flag_turkey_alternative.xml',
        'outputs': {
            'flag_turkey_alt_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/flag_turkey_alternative/flag_turkey_alt_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
    },
    {
        'input': 'examples/culling_disabled_inputs/horse_and_mug.xml',
        'outputs': {
            'horse_and_mug_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/horse_and_mug/horse_and_mug_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4]
        },
    },
    {
        'input': 'examples/culling_disabled_inputs/sample.xml',
        'outputs': {
            'sample.ppm.png': 'examples/culling_disabled_outputs/sample/sample.ppm.png',
        },
    },
]

TS_CULLING = [
    {
        'input': 'examples/culling_enabled_inputs/empty_box.xml',
        'outputs': {
            'empty_box_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/empty_box/empty_box_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4, 5, 6, 7, 8, 'example']
        },
    },
    {
        'input': 'examples/culling_enabled_inputs/filled_box.xml',
        'outputs': {
            'filled_box_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/filled_box/filled_box_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4, 5, 6, 7, 8, 'example']
        },
    },
    {
        'input': 'examples/culling_enabled_inputs/flag_brazil.xml',
        'outputs': {
            'flag_brazil_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/flag_brazil/flag_brazil_{}.ppm.png'.format(i)
            for i in [1, 2, 'final']
        },
    },
    {
        'input': 'examples/culling_enabled_inputs/flag_iceland.xml',
        'outputs': {
            'flag_iceland_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/flag_iceland/flag_iceland_{}.ppm.png'.format(i)
            for i in [1, 2, 'final']
        },
    },
    {
        'input': 'examples/culling_enabled_inputs/flag_turkey.xml',
        'outputs': {
            'flag_turkey_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/flag_turkey/flag_turkey_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
    },
    {
        'input': 'examples/culling_enabled_inputs/flag_turkey_alternative.xml',
        'outputs': {
            'flag_turkey_alt_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/flag_turkey_alternative/flag_turkey_alt_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
    },
    {
        'input': 'examples/culling_enabled_inputs/horse_and_mug.xml',
        'outputs': {
            'horse_and_mug_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/horse_and_mug/horse_and_mug_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4]
        },
    },
    {
        'input': 'examples/culling_enabled_inputs/sample.xml',
        'outputs': {
            'sample.ppm.png': 'examples/culling_enabled_outputs/sample/sample.ppm.png',
        },
    },
]

TS_ORTHOGRAPHIC = [
    {
        'input': 'examples/different_projection_type_example/flag_turkey/flag_turkey_orthographic.xml',
        'outputs': {
            'flag_turkey_orthographic.ppm.png':
                'examples/different_projection_type_example/flag_turkey/flag_turkey_orthographic.ppm.png',
        },
    },
    {
        'input': 'examples/different_projection_type_example/flag_turkey/flag_turkey_perspective.xml',
        'outputs': {
            'flag_turkey_perspective.ppm.png':
                'examples/different_projection_type_example/flag_turkey/flag_turkey_perspective.ppm.png',
        },
    },
    {
        'input': 'examples/different_projection_type_example/horse_and_mug'
                 '/horse_and_mug_orthographic.xml',
        'outputs': {
            'horse_and_mug_orthographic_{}.ppm.png'.format(i):
                'examples/different_projection_type_example/horse_and_mug'
                '/horse_and_mug_orthographic_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4]
        },
    },
    {
        'input': 'examples/different_projection_type_example/horse_and_mug'
                 '/horse_and_mug_perspective.xml',
        'outputs': {
            'horse_and_mug_perspective_{}.ppm.png'.format(i):
                'examples/different_projection_type_example/horse_and_mug'
                '/horse_and_mug_perspective_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4]
        },
    },
]

# TEST_SUITE = TS_CLIPPING + TS_NO_CULLING + TS_CULLING + TS_ORTHOGRAPHIC
TEST_SUITE = TS_CULLING

TEST_FAILURES = []
TEST_PASSES = []
PROGRESS = 0
MAX_PROGRESS = 95  # this value obtained experimentally


def test_single(obj, metric=ComparisonMetric.SSIM):
    global PROGRESS

    input_path, outputs = obj['input'], obj['outputs']
    subprocess.run([RASTERIZER, input_path])
    PROGRESS += 1
    for actual_file, expected_file in outputs.items():
        convert = subprocess.run([COMPARE,
                                  '-metric',
                                  metric,
                                  expected_file,
                                  actual_file,
                                  'NULL'],
                                 stderr=subprocess.PIPE)
        PROGRESS += 1
        if convert.returncode == 0:  # dissimilar
            lst = TEST_PASSES
        else:
            lst = TEST_FAILURES
        lst.append({
            'input': input_path,
            'output': actual_file,
            'score': float(convert.stderr.decode()),
            'metric': metric
        })


if __name__ == '__main__':
    threads = []
    finished = False

    def print_progress():
        global PROGRESS, MAX_PROGRESS

        while not finished:
            sys.stdout.write('\rtests: {:.2f}%'.format(100*PROGRESS/MAX_PROGRESS))
            time.sleep(0.5)

    printer_thread = threading.Thread(target=print_progress, args=())

    for test_item in TEST_SUITE:
        threads.append(threading.Thread(target=test_single, args=(test_item,)))

    printer_thread.start()
    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()
    finished = True
    printer_thread.join()

    TEST_PASSES.sort(key=lambda o: o['score'], reverse=True)
    TEST_FAILURES.sort(key=lambda o: o['score'], reverse=True)

    with open(OUTPUT_FILENAME, 'w') as outfile:
        json.dump({'passes': TEST_PASSES, 'failures': TEST_FAILURES}, outfile, indent=4, sort_keys=True)

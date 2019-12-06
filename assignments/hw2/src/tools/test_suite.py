import json
import os
import subprocess


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
OUTPUTS_ROOT = 'test_outputs'
COMPARISON_METRIC = ComparisonMetric.SSIM


TS_CLIPPING = [
    {
        'input': 'examples/clipping_example/empty_box_clipped.xml',
        'outputs': {
            'empty_box_clipped_{}.ppm.png'.format(i):
                'examples/clipping_example/empty_box_clipped_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
        'dir': 'clipping',
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
        'dir': 'culling_disabled',
    },
    {
        'input': 'examples/culling_disabled_inputs/filled_box.xml',
        'outputs': {
            'filled_box_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/filled_box/filled_box_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4, 5, 6, 7, 8, 'example']
        },
        'dir': 'culling_disabled',
    },
    {
        'input': 'examples/culling_disabled_inputs/flag_brazil.xml',
        'outputs': {
            'flag_brazil_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/flag_brazil/flag_brazil_{}.ppm.png'.format(i)
            for i in [1, 2, 'final']
        },
        'dir': 'culling_disabled',
    },
    {
        'input': 'examples/culling_disabled_inputs/flag_turkey.xml',
        'outputs': {
            'flag_turkey_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/flag_turkey/flag_turkey_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
        'dir': 'culling_disabled',
    },
    {
        'input': 'examples/culling_disabled_inputs/flag_turkey_alternative.xml',
        'outputs': {
            'flag_turkey_alt_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/flag_turkey_alternative/flag_turkey_alt_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
        'dir': 'culling_disabled',
    },
    {
        'input': 'examples/culling_disabled_inputs/horse_and_mug.xml',
        'outputs': {
            'horse_and_mug_{}.ppm.png'.format(i):
                'examples/culling_disabled_outputs/horse_and_mug/horse_and_mug_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4]
        },
        'dir': 'culling_disabled',
    },
    {
        'input': 'examples/culling_disabled_inputs/sample.xml',
        'outputs': {
            'sample.ppm.png': 'examples/culling_disabled_outputs/sample/sample.ppm.png',
        },
        'dir': 'culling_disabled',
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
        'dir': 'culling_enabled',
    },
    {
        'input': 'examples/culling_enabled_inputs/filled_box.xml',
        'outputs': {
            'filled_box_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/filled_box/filled_box_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4, 5, 6, 7, 8, 'example']
        },
        'dir': 'culling_enabled',
    },
    {
        'input': 'examples/culling_enabled_inputs/flag_brazil.xml',
        'outputs': {
            'flag_brazil_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/flag_brazil/flag_brazil_{}.ppm.png'.format(i)
            for i in [1, 2, 'final']
        },
        'dir': 'culling_enabled',
    },
    {
        'input': 'examples/culling_enabled_inputs/flag_iceland.xml',
        'outputs': {
            'flag_iceland_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/flag_iceland/flag_iceland_{}.ppm.png'.format(i)
            for i in [1, 2, 'final']
        },
        'dir': 'culling_enabled',
    },
    {
        'input': 'examples/culling_enabled_inputs/flag_turkey.xml',
        'outputs': {
            'flag_turkey_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/flag_turkey/flag_turkey_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
        'dir': 'culling_enabled',
    },
    {
        'input': 'examples/culling_enabled_inputs/flag_turkey_alternative.xml',
        'outputs': {
            'flag_turkey_alt_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/flag_turkey_alternative/flag_turkey_alt_{}.ppm.png'.format(i)
            for i in [1, 2]
        },
        'dir': 'culling_enabled',
    },
    {
        'input': 'examples/culling_enabled_inputs/horse_and_mug.xml',
        'outputs': {
            'horse_and_mug_{}.ppm.png'.format(i):
                'examples/culling_enabled_outputs/horse_and_mug/horse_and_mug_{}.ppm.png'.format(i)
            for i in [1, 2, 3, 4]
        },
        'dir': 'culling_enabled',
    },
    {
        'input': 'examples/culling_enabled_inputs/sample.xml',
        'outputs': {
            'sample.ppm.png': 'examples/culling_enabled_outputs/sample/sample.ppm.png',
        },
        'dir': 'culling_enabled',
    },
]
TS_ORTHOGRAPHIC = [
    {
        'input': 'examples/different_projection_type_example/flag_turkey/flag_turkey_orthographic.xml',
        'outputs': {
            'flag_turkey_orthographic.ppm.png':
                'examples/different_projection_type_example/flag_turkey/flag_turkey_orthographic.ppm.png',
        },
        'dir': 'different_projection_types',
    },
    {
        'input': 'examples/different_projection_type_example/flag_turkey/flag_turkey_perspective.xml',
        'outputs': {
            'flag_turkey_perspective.ppm.png':
                'examples/different_projection_type_example/flag_turkey/flag_turkey_perspective.ppm.png',
        },
        'dir': 'different_projection_types',
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
        'dir': 'different_projection_types',
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
        'dir': 'different_projection_types',
    },
]

TEST_SUITE = TS_CLIPPING + TS_CULLING + TS_NO_CULLING + TS_ORTHOGRAPHIC

TEST_RESULTS = []
PROGRESS = 0


def increment_and_report_progress():
    global PROGRESS

    PROGRESS += 1
    print('Progress: {}/{}'.format(PROGRESS, MAX_PROGRESS))


def test_single(obj, metric=COMPARISON_METRIC):
    global PROGRESS

    input_path, outputs, directory = obj['input'], obj['outputs'], obj['dir']
    target_dir = '{}/{}'.format(OUTPUTS_ROOT, directory)
    os.makedirs(target_dir, exist_ok=True)
    subprocess.run([RASTERIZER, input_path])
    increment_and_report_progress()

    for actual_file, expected_file in outputs.items():
        target_file = '{}/{}'.format(target_dir, actual_file)
        os.remove(actual_file.replace('.ppm.png', '.ppm'))
        os.rename(actual_file, target_file)
        convert = subprocess.run([COMPARE, '-metric', metric, expected_file, target_file, 'NULL'],
                                 stderr=subprocess.PIPE)
        increment_and_report_progress()
        TEST_RESULTS.append({
            'input': input_path,
            'output': target_file,
            'score': float(convert.stderr.decode()),
            'metric': metric
        })


if __name__ == '__main__':
    MAX_PROGRESS = sum(len(obj['outputs']) for obj in TEST_SUITE) + len(TEST_SUITE)

    for test_item in TEST_SUITE:
        test_single(test_item)

    TEST_RESULTS.sort(key=lambda o: o['score'], reverse=True)

    with open(OUTPUT_FILENAME, 'w') as outfile:
        json.dump({'results': TEST_RESULTS}, outfile, indent=4, sort_keys=True)

    # This script has the side effect of creating a file named "NULL". Remove it.
    os.remove('NULL')

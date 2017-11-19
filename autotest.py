#!/usr/bin/env python

import argparse
import glob
import os
import shutil
import subprocess
import sys
import tempfile


def parse_arguments():
    parser = argparse.ArgumentParser(description='Auto test for ts_splitter util. FFmpeg is required.')
    parser.add_argument('-u', '--util', help='Path to ts_splitter util', type=str, required=True, dest='util')
    parser.add_argument('-f', '--files', help='Path to directory containing TS files', type=str, required=True, dest='files')
    parser.add_argument('--ffmpeg', help='Path to FFmpeg util', type=str, required=False, dest='ffmpeg')
    return parser.parse_args()


def check_settings(settings):
    # check ts_splitter path
    if not os.path.isfile(settings.util):
        raise Exception('File {0} does not exist or is not a file'.format(settings.util))

    # check files dir path
    if not os.path.isdir(settings.files):
        raise Exception('Directory {0} does not exist or is not a directory'.format(settings.files))

    # check ffmpeg presence
    if settings.ffmpeg:
        if not os.path.isfile(settings.util):
            raise Exception('File {0} does not exist or is not a file'.format(settings.ffmpeg))
    else:
        try:
            dev_null = open(os.devnull, 'w')
            subprocess.call(['ffmpeg', '-h'], stdout=dev_null, stderr=dev_null)
            dev_null.close()
            settings.ffmpeg = 'ffmpeg'
        except OSError as e:
            if e.errno == os.errno.ENOENT:
                raise Exception('FFmpeg is not found')
            raise

    # prepare temp dir
    settings.temp = tempfile.mkdtemp(prefix='ts_splitter_')


def clean(settings, remove_top_dir):
    if not hasattr(settings, 'temp'):
        return
    if remove_top_dir:
        shutil.rmtree(settings.temp)
    else:
        for f in os.listdir(settings.temp):
            os.remove(os.path.join(settings.temp, f))


def run_tests(settings):
    ts_files = glob.glob(os.path.join(settings.files, '*.ts'))
    ts_files.sort()
    if not ts_files:
        raise Exception('No TS files found')

    failures = 0
    for ts_file in ts_files:
        failures += run_test(ts_file, settings)

    if failures == 0:
        sys.stdout.write('ALL TESTS PASSED\n')
    else:
        sys.stdout.write('{0} TEST(S) FAILED\n'.format(failures))


def run_test(ts_file, settings):
    sys.stdout.write('Checking file {0} ... '.format(ts_file))
    sys.stdout.flush()

    result = 0
    description = ''

    dev_null = open(os.devnull, 'w')
    audio_output = os.path.join(settings.temp, 'audio_1.out')
    video_output = os.path.join(settings.temp, 'video_1.out')
    util_args = [settings.util, '-i', ts_file, '-oa', audio_output, '-ov', video_output]

    try:
        # run util
        process = subprocess.Popen(util_args, stdout=dev_null, stderr=subprocess.PIPE)
        _, errors = process.communicate()
        if process.returncode != 0:
            raise Exception('{0}\n{1} returned code {2}\n'.format(errors, settings.util, process.returncode))

        # run ffmpeg
        files_to_check = glob.glob(os.path.join(settings.temp, '*.out'))
        for f in files_to_check:
            ffmpeg_args = [settings.ffmpeg, '-i', f, '-v', 'fatal', '-f', 'null', '-']
            process = subprocess.Popen(ffmpeg_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            output, errors = process.communicate()
            if process.returncode !=0 or output or errors:
                raise Exception('{0} found some errors in output file {1}'.format(settings.ffmpeg, f))

    except Exception as e:
        result = 1
        description = str(e) + '\n'

    clean(settings.temp, False)
    if result == 0:
        sys.stdout.write('OK\n')
    else:
        sys.stdout.write('FAIL\n')
        sys.stderr.write(description)

    return result


def main():
    exit_code = 0

    try:
        settings = parse_arguments()
        check_settings(settings)
        run_tests(settings)
    except Exception as e:
        sys.stderr.write(str(e) + '\n')
        exit_code = 1

    clean(settings, True)
    sys.exit(exit_code)


if __name__ == '__main__':
    main()

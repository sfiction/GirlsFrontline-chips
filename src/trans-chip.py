#!/bin/env python3

import io
import os
import re
import sys
import yaml

if __name__ == '__main__':
    real_path = os.path.dirname(os.path.realpath(__file__))
    os.chdir(os.path.join(real_path, '..'));

    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', newline='\n')
    
    raw_file = './user/chips-raw'
    with open(raw_file, 'r', encoding='utf-8', newline='\n') as fd:
        content = fd.read().strip()

    trans_file = './data/trans-chip.yml'
    with open(trans_file, 'r', encoding='utf-8', newline='\n') as fd:
        trans = yaml.load(fd.read(), Loader=yaml.Loader)

    mat_chips = re.search(r'(?<=\[\w!).*?(?=\?\w\])', content)
    if not mat_chips:
        print('parse failed', file=sys.stderr)
        exit(1)
    text_chips = mat_chips.group(0)
    lines = list(filter(None, text_chips.split('&')))
    chips = [list(int(x) for x in line.split(',')) for line in lines]

    for chip in chips:
        color = chip[1]
        clazz = chip[2]
        shape = chip[3]
        val = chip[5: 9]

        trans_class = trans['class'][clazz]

        color = int(color) - 1
        clazz = trans_class['class']
        shape = trans_class['shape'][int(shape)]
        if clazz == 0x550 and shape >= 19:
            clazz = 0x551
        val = val[2: 4] + val[0: 2]

        chip[: ] = [color, clazz, shape] + val
    fd = open('./user/chips-inner', 'w', encoding='utf-8', newline='\n')
    print('\n'.join(' '.join(str(x) for x in chip) for chip in chips), file=fd)
    fd.close()

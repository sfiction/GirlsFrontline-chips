#!/bin/env python3

import os
import sys
    
def show_shape(info_file, skip):
    with open(info_file, 'r', encoding='utf-8', newline='\n') as fd:
        info = list(filter(None, fd.read().split('\n')))

    for chip in info:
        chip = list(map(int, chip.split(' ')[skip: ]))
        
        h, w = chip[0: 2]
        n = chip[2]

        print(h, w)
        a = [[' '] * w for i in range(h)]
        for i in chip[3: ]:
            x, y = i // w, i % w
            a[x][y] = '*'
        print('\n'.join(' '.join(line) for line in a))

if __name__ == '__main__':
    real_path = os.path.dirname(os.path.realpath(__file__))
    show_shape(os.path.join(real_path, '../data/chips.txt'), 4)
    show_shape(os.path.join(real_path, '../data/weapons.txt'), 2)

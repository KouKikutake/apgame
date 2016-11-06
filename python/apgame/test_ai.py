#!/bin/env python
# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

import sys
import time
import random

x = 0
y = 0
blackWin = 0
whiteWin = 0
while True:
    time.sleep(0.1)
    sys.stderr.write("\033[2J")
    sys.stderr.write("\033[1;1H")
    print('GET_COLOR')
    sys.stdout.flush()
    color = sys.stdin.readline()[:-1]
    sys.stderr.write(color + '\n')
    print('GET_STATUS')
    sys.stdout.flush()
    status = sys.stdin.readline()[:-1]
    sys.stderr.write(status + '\n')
    print('GET_BOARD')
    sys.stdout.flush()
    board = sys.stdin.readline()
    for i in range(8):
        sys.stderr.write(board[8*i:8*i+8] + '\n')

    black = board.count('B')
    white = board.count('W')
    sys.stderr.write('BLACK: %d, WHITE: %d\n' % (black, white))
    sys.stderr.write('BLACKWIN: %d, WHITEWIN: %d\n' % (blackWin, whiteWin))
    if status == 'AFTER_GAME':
        if black > white:
            blackWin += 1
        if white > black:
            whiteWin += 1
        continue
    if status == 'BEFORE_GAME':
        continue

    if status.find(color) == -1:
        continue

    place = []
    for i in range(8):
        for j in range(8):
            if board[i + 8 * j] == '.':
                place.append((i, j))
    random.shuffle(place)

    for x, y in place:
        print('PUT_STONE %d %d' % (x, y))
        sys.stdout.flush()
        response = sys.stdin.readline()
        if response == 'TRUE':
            sys.stderr.write('PUT_STONE %d %d\n' % (x, y))
            break

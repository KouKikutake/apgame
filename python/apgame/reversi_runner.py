# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function

from .socket import Socket

from .user_client import UserClient
from .room_client import RoomClient
from .reversi_client import ReversiClient

import os.path
import subprocess
import sys
import time

def spin (reversi_client, commands, prog):
#     print(commands)
    if commands[0] == 'GET_STATUS':
        status = reversi_client.getStatus()
        if status == 0:
            prog.stdin.write('BEFORE_GAME\n')
            return
        elif status == 1:
            prog.stdin.write('BLACK_TURN\n')
            return
        elif status == 2:
            prog.stdin.write('WHITE_TURN\n')
            return
        elif status == 3:
            prog.stdin.write('AFTER_GAME\n')
            return
    elif commands[0] == 'GET_COLOR':
        color = reversi_client.getColor()
        if color == 1:
            prog.stdin.write('BLACK\n')
            return
        elif color == -1:
            prog.stdin.write('WHITE\n')
            return
        else:
            prog.stdin.write('EMPTY\n')
            return
    elif commands[0] == 'GET_BOARD':
        board = reversi_client.getBoard()
        for stone in board:
            if stone == 0:
                ch = '.'
            elif stone == 1:
                ch = 'B'
            elif stone == -1:
                ch = 'W'
            prog.stdin.write(ch)
        prog.stdin.write('\n')
        return
    elif commands[0] == 'PUT_STONE':
        x = int(commands[1])
        y = int(commands[2])
        status = reversi_client.putStone(x, y)
        if status == 0:
            prog.stdin.write('TRUE\n')
            return
        else:
            prog.stdin.write('FALSE\n')
            return
    print('unknown command {}'.format(commands))

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='user client')
    parser.add_argument('--host', dest='host', required=True, type=str)
    parser.add_argument('--port', dest='port', required=True, type=int)
    parser.add_argument('--room', dest='room', required=True, type=str)
    parser.add_argument('--name', dest='name', required=True, type=str)
    parser.add_argument('args', nargs='+', type=str)
    
    args = parser.parse_args()
    proc = subprocess.Popen(args.args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=sys.stderr)
    socket = Socket(host=args.host, port=args.port)
    user_client = UserClient(socket)
    user_client.joinUser(args.name)
    user_client.exit()
    room_client = RoomClient(socket)
    room_client.joinRoom(args.room, "Reversi")
    room_client.exit()
    reversi_client = ReversiClient(socket)

    buf = ''
    while True:
        request = proc.stdout.readline()
        if len(request) == 0:
            time.sleep(0.1)
            continue
        commands = request[:-1].split()
        spin(reversi_client, commands, proc)

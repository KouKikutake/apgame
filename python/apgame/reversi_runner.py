# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

from .socket import Socket

from .user_client import UserClient
from .room_client import RoomClient
from .reversi_client import ReversiClient

import subprocess

def spin (reversi_client, commands, prog):
    print(commands)
    if commands[0] == 'GET_STATUS':
        status = reversi_client.getStatus()
        if status == 0:
            prog.stdin.write(b'BEFORE_GAME\n')
            return
        elif status == 1:
            prog.stdin.write(b'BLACK_TURN\n')
            return
        elif status == 2:
            prog.stdin.write(b'WHITE_TURN\n')
            return
        elif status == 3:
            prog.stdin.write(b'AFTER_GAME\n')
            return
    elif commands[0] == 'GET_COLOR':
        color = reversi_client.getColor()
        if color == 1:
            prog.stdin.write(b'BLACK\n')
            return
        elif color == -1:
            prog.stdin.write(b'WHITE\n')
            return
        else:
            prog.stdin.write(b'EMPTY\n')
            return
    elif commands[0] == 'GET_BOARD':
        board = reversi_client.getBoard()
        for stone in board:
            if stone == '0':
                ch = b'.'
            elif stone == '1':
                ch = b'B'
            elif stone == '-1':
                ch = b'W'
            prog.stdin.write(ch)
        prog.stdin.write(b'\n')
        return
    elif commands[0] == 'PUT_STONE':
        status = reversi_client.putStone(x, y)
        if status == 0:
            prog.stdin.write(b'TRUE\n')
            return
        else:
            prog.stdin.write(b'FALSE\n')
            return
    print('unknown command {}'.format(commands))

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='user client')
    parser.add_argument('--host', dest='host', required=True, type=str)
    parser.add_argument('--port', dest='port', required=True, type=int)
    parser.add_argument('--room', dest='room', required=True, type=str)
    parser.add_argument('--name', dest='name', required=True, type=str)
    parser.add_argument('path', type=str)
    
    args = parser.parse_args()
    socket = Socket(host=args.host, port=args.port)
    user_client = UserClient(socket)
    user_client.joinUser(args.name)
    user_client.exit()
    room_client = RoomClient(socket)
    room_client.joinRoom(args.room, "Reversi")
    room_client.exit()
    reversi_client = ReversiClient(socket)

    proc = subprocess.Popen([args.path], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    while True:
        line = str(proc.stdout.readline())
        commands = line.split(' ')
        spin(reversi_client, commands, proc)

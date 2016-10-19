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
    if commands[0] == 'GET_STATUS':
        status = reversi_client.getStatus()
        if status == 0:
            prog.stdin.write('BEFORE_GAME')
        elif status == 1:
            prog.stdin.write('BEFORE_GAME')
    elif commands[0] == 'GET_COLOR':

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='user client')
    parser.add_argument('--host', dest='host', required=True, type=str)
    parser.add_argument('--port', dest='port', required=True, type=int)
    parser.add_argument('--room', dest='room', required=True, type=str)
    parser.add_argument('--name', dest='name', required=True, type=str)
    parser.add_argument('path', dest='path', required=True, type=str)
    
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
        line = proc.stdout.readline()
        commands = line.split(' ')
        spin(reversi_client, commands, proc)

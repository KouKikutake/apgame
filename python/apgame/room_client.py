# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

from .socket import Socket

ROOM_COMMAND_JOIN_ROOM = 0
ROOM_COMMAND_EXIT = ROOM_COMMAND_JOIN_ROOM + 1

class RoomClient(object):
    
    def __init__(self, socket):
        self._socket = socket
    
    def joinRoom(self, room_name, game_name):
        self._socket.sendInt32(ROOM_COMMAND_JOIN_ROOM)
        self._socket.sendString(room_name)
        self._socket.sendString(game_name)
        error = self._socket.receiveInt32()
        return error
    
    def exit(self):
        self._socket.sendInt32(ROOM_COMMAND_EXIT)
        error = self._socket.receiveInt32()
        return error

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='user client')
    parser.add_argument('--host', dest='host', required=True, type=str)
    parser.add_argument('--port', dest='port', required=True, type=int)
    
    args = parser.parse_args()
    socket = Socket(host=args.host, port=args.port)
    room_client = RoomClient(socket)
    
    from IPython import embed
    embed()

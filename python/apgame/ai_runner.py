# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

from .socket import Socket

from .user_client import UserClient
from .room_client import RoomClient

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='user client')
    parser.add_argument('--host', dest='host', required=True, type=str)
    parser.add_argument('--port', dest='port', required=True, type=int)
    
    args = parser.parse_args()
    socket = Socket(host=args.host, port=args.port)
    user_client = UserClient(socket)
    room_client = RoomClient(socket)
    
    from IPython import embed
    embed()

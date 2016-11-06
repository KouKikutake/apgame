# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function

from .socket import Socket

REVERSI_COMMAND_GET_COLOR = 0
REVERSI_COMMAND_GET_STATUS = 1
REVERSI_COMMAND_GET_BOARD = 2
REVERSI_COMMAND_PUT_STONE = 3
REVERSI_COMMAND_GET_LAST_STONE = 4
REVERSI_COMMAND_FINISH_TURN = 5

REVERSI_STONE_BLACK = 1
REVERSI_STONE_EMPTY = 0
REVERSI_STONE_WHITE = -1

class ReversiClient(object):
    
    def __init__(self, socket):
        self._socket = socket

    def getColor(self):
        """
            @return
                1: BLACK
                -1: WHITE
                0: EMPTY, game is not started
        """
        self._socket.sendInt32(REVERSI_COMMAND_GET_COLOR)
        color = self._socket.receiveInt8()
        return color
    
    def getStatus(self):
        self._socket.sendInt32(REVERSI_COMMAND_GET_STATUS)
        status = self._socket.receiveInt32()
        return status
    
    def getBoard(self):
        self._socket.sendInt32(REVERSI_COMMAND_GET_BOARD)
        board = [self._socket.receiveInt8() for i in range(64)]
        return board
    
    def putStone(self, x, y):
        self._socket.sendInt32(REVERSI_COMMAND_PUT_STONE)
        self._socket.sendInt32(x)
        self._socket.sendInt32(y)
        error = self._socket.receiveInt32()
        return error
    
    def exit(self):
        self._socket.sendInt32(USER_COMMAND_EXIT)
        error = self._socket.receiveInt32()
        return error


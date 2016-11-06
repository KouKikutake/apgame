# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals
from builtins import bytes, str

import socket
import struct

class Socket(object):
    _socket = None
    _buffer = None
    def __init__(self, host, port):
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._socket.connect((host, port))
        self._buffer = bytes()
    
    def __del__(self):
        self._socket.close()
    
    def sendInt32(self, val):
        self._send(struct.pack('<i', val))
    
    def sendUInt64(self, val):
        self._send(struct.pack('<Q', val))
    
    def sendString(self, val):
        self.sendUInt64(len(val))
        msg = val.encode('utf8')
        self._send(struct.pack('{}s'.format(len(msg)), msg))
   
    def receiveBool(self):
        self._receive(1)
        return struct.unpack('?', self._consume(1))[0]
    
    def receiveInt8(self):
        self._receive(1)
        return struct.unpack('<b', self._consume(1))[0]
    
    def receiveInt32(self):
        self._receive(4)
        return struct.unpack('<i', self._consume(4))[0]
    
    def receiveUInt64(self):
        self._receive(8)
        return struct.unpack('<Q', self._consume(8))[0]
    
    def receiveString(self):
        size = self.receiveUInt64()
        return self.unpack('{}s'.format(size), self._consume(size))[0]
    
    def _consume(self, length):
        data = self._buffer[:length]
        self._buffer = self._buffer[length:]
        return data
    
    def _send(self, data):
#         print('send {} bytes'.format(len(data)))
        total = 0
        while total < len(data):
            size = self._socket.send(data[total:])
            if size == 0:
                raise RuntimeError('connection closed')
            total += size
    
    def _receive(self, recv_size):
#         print('receive{} bytes'.format(recv_size))
        while len(self._buffer) < recv_size:
            segment = self._socket.recv(4096)
            if len(segment) == 0:
                raise RuntimeError('connection closed')
            self._buffer += segment

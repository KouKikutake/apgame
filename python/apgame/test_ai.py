#!/bin/env python
# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import generators
from __future__ import print_function
from __future__ import unicode_literals

import sys

while True:
    print('GET_STATUS')
    sys.stdout.flush()
    response = sys.stdin.readline()
    sys.stderr.write(response)

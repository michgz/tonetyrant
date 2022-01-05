#!/usr/bin/env python
import wx
import sys
import logging
import os

sys.path.append("src")



def _(X):
    return X


        
def main():
  
    # Set up the root logger
    _logger = logging.getLogger()
    _logger.setLevel(logging.DEBUG)
    _fh = logging.FileHandler('log.txt')
    _fh.setLevel(logging.DEBUG)
    _logger.addHandler(_fh)

    # Run the application
    _app = wx.App(False)
    _app.MainLoop() 


if __name__=="__main__":
    main()



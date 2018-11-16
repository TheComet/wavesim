__author__ = 'thecomet'

import pygame
from Window import Window

if __name__ == '__main__':
    pygame.init()
    window = Window(1280, 720)
    window.enter_main_loop()
    pygame.quit()

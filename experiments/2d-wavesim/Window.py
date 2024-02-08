__author__ = 'thecomet'

import time
import pygame
from Simulation import Simulation


class Window(object):
    def __init__(self, width, height):
        super(Window, self).__init__()
        self.dimensions = width, height
        self.screen = pygame.display.set_mode(self.dimensions)

        self.updateable_items = list()
        self.updateable_items.append(Simulation())

        self.__fixed_step = 1.0 / 1200
        self.__last_time_updated = None
        self.__running = True

    def enter_main_loop(self):
        self.__last_time_updated = time.process_time()
        while self.__running:
            self.__process_events()
            self.__update_items()
            self.__draw_items()

    def __process_events(self):
        for event in pygame.event.get():
            for item in self.updateable_items:
                item.process_event(event)
            if event.type == pygame.QUIT:
                self.__running = False

    def __update_items(self):
        if self.__update_time_step():
            for item in self.updateable_items:
                item.update(self.__fixed_step)

    def __draw_items(self):
        self.screen.fill((0, 0, 0))
        for item in self.updateable_items:
            item.draw(self.screen)
        pygame.display.flip()

    def __update_time_step(self):
        new_time = time.process_time()
        if new_time - self.__last_time_updated < self.__fixed_step:
            return False
        self.__last_time_updated = new_time
        return True

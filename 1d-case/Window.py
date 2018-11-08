__author__ = 'thecomet'

import time
import pygame
from Updateable import Updateable
from Simulation import Simulation


class Window(Updateable):
    def __init__(self, width, height):
        self.dimensions = width, height
        self.screen = pygame.display.set_mode(self.dimensions)

        self.updateable_items = list()
        self.updateable_items.append(self)
        self.updateable_items.append(Simulation())

        self.__last_time_updated = None
        self.__running = None

    def enter_main_loop(self):
        self.__last_time_updated = time.clock()
        self.__running = True
        while self.__running:
            self.__process_events()
            self.__update_items()
            self.__draw_items()

    def __process_events(self):
        for event in pygame.event.get():
            for item in self.updateable_items:
                item.process_event(event)

    def __update_items(self):
        time_step = self.__update_time_step()
        for item in self.updateable_items:
            item.update(time_step)

    def __update_time_step(self):
        new_time = time.clock()
        time_step = new_time - self.__last_time_updated
        self.__last_time_updated = new_time
        return time_step

    def __draw_items(self):
        self.screen.fill((0, 0, 0))

        for item in self.updateable_items:
            item.draw(self.screen)

        pygame.display.flip()

    def process_event(self, event):
        if event.type == pygame.QUIT:
            self.__running = False

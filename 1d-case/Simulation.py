import math
import pygame
import numpy as np
from Updateable import Updateable


class Domain(Updateable):
    def __init__(self, begin, end, sound_velocity, screen_coords_transform):
        self.begin = begin
        self.end = end
        self.sound_velocity = sound_velocity
        self.transform = screen_coords_transform

        # Calculate minimum grid size h and number of cells for given boundary and frequency
        fmax = 20e3  # Hz
        hmax = self.sound_velocity / (2*fmax)
        distance = abs(end - begin)
        num_cells = math.ceil(distance / hmax)
        self.h = distance / num_cells
        self.pressures = np.zeros(num_cells)
        self.pressures[0] = 1

        print(f"Cells: {num_cells}, Cell size: {self.h}")

    def to_screen_coords(self, x, p):
        x, p, w = (np.array([x, p, 1]) * self.transform.T).tolist()[0]
        return int(x/w), int(p/w)

    def draw(self, surface):
        for x, p in zip(np.linspace(self.begin, self.end, len(self.pressures)), self.pressures):
            base_pos = self.to_screen_coords(x, 0)
            tip_pos = self.to_screen_coords(x, p)
            pygame.draw.line(surface, (255, 255, 255), base_pos, tip_pos)
            pygame.draw.circle(surface, (255, 255, 255), tip_pos, 2, 1)


class Simulation(Updateable):
    def __init__(self):
        transform = np.matrix([
            [1000, 0, 200],
            [0, -30, 500],
            [0, 0, 1]
        ])
        self.domains = [
            Domain(0, 1, 340, transform)
        ]

    def process_event(self, event):
        pass

    def update(self, time_step):
        pass

    def draw(self, surface):
        for d in self.domains:
            d.draw(surface)

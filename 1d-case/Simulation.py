import math
import pygame
import numpy as np
import math
from scipy.fftpack import dct
from Updateable import Updateable


def idct(arr):
    return dct(arr, type=3) / (2 * arr.size)


class Domain(Updateable):
    def __init__(self, name, begin, end, sound_velocity, screen_coords_transform):
        self.name = name
        self.begin = begin
        self.end = end
        self.sound_velocity = sound_velocity
        self.transform = screen_coords_transform
        self.time_passed = 0

        # Calculate minimum grid size h and number of cells for given boundary and frequency
        fmax = 20e3  # Hz
        hmax = self.sound_velocity / (2*fmax)
        distance = abs(end - begin)
        num_cells = int(math.ceil(distance / hmax))
        self.h = distance / num_cells

        # calculate maximum time step
        self.dt = self.h / (self.sound_velocity * math.sqrt(3))

        # Spatial axis
        self.spatial = np.linspace(0, self.end - self.begin, num_cells)

        # Calculate characteristic frequencies omega
        k = np.pi * self.spatial / (self.end - self.begin)
        self.w = self.sound_velocity * k

        # Allocate modes for two timesteps
        self.M_past = np.zeros(num_cells)
        self.M_past[0] = 0.01
        self.M_past[1] = -0.01
        self.M_past = dct(self.M_past)
        self.M_current = np.zeros(num_cells)

        # font stuff
        self.font = pygame.font.SysFont('monospace', 18)

        print(f"Cells: {num_cells}, Cell size: {self.h}, Time step: {self.dt}")

    def to_screen_coords(self, x, p):
        x, p, w = (np.array([x, p, 1]) * self.transform.T).tolist()[0]
        return int(x/w), int(p/w)

    def step_time(self):
        M_new = 2 * self.M_current * np.cos(self.w * self.dt) - self.M_past
        self.M_current, self.M_past = M_new, self.M_current
        self.time_passed += self.dt

    def draw(self, surface):
        # draw boundaries
        pygame.draw.line(surface, (255, 255, 255), self.to_screen_coords(self.begin, -1), self.to_screen_coords(self.begin, 1))
        pygame.draw.line(surface, (255, 255, 255), self.to_screen_coords(self.end, -1), self.to_screen_coords(self.end, 1))

        # draw pressure values
        pressures = idct(self.M_current)
        for x, p in zip(self.spatial + self.begin, pressures):
            base_pos = self.to_screen_coords(x, 0)
            tip_pos = self.to_screen_coords(x, p)
            pygame.draw.line(surface, (255, 255, 255), base_pos, tip_pos)
            pygame.draw.circle(surface, (255, 255, 255), tip_pos, 2, 1)

        # draw text
        text = self.font.render(f"Domain {self.name}: c={self.sound_velocity}m/s, Time: {self.time_passed:.2f}s", 1, (255, 255, 255))
        surface.blit(text, self.to_screen_coords(self.begin, 1.8))


class Simulation(Updateable):
    def __init__(self):
        transform = np.matrix([
            [500, 0, 200],
            [0, -120, 500],
            [0, 0, 1]
        ])
        self.domains = [
            Domain("1", 0, 1, 340, transform),
            Domain("2", 1, 2, 450, transform)
        ]

    def process_event(self, event):
        pass

    def update(self, time_step):
        for domain in self.domains:
            target_time = domain.time_passed + time_step
            while domain.time_passed < target_time:
                domain.step_time()

    def draw(self, surface):
        for d in self.domains:
            d.draw(surface)

import numpy as np
import math
import colorsys
import pygame
from scipy.fftpack import dct
from Updateable import Updateable


def idct(arr):
    return dct(arr, type=3) / (2 * len(arr))

def dct2(arr):
    return dct(dct(arr, axis=0), axis=1)

def idct2(arr):
    along_x = dct(arr, type=3, axis=0) / (2 * arr.shape[0])
    along_y = dct(along_x, type=3, axis=1) / (2 * arr.shape[1])
    return along_y


class Domain1D(Updateable):
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


class Domain2D(Updateable):
    def __init__(self, name, begin, end, sound_velocity, screen_coords_transform):
        self.name = name
        self.begin = np.array(begin)
        self.end = np.array(end)
        self.sound_velocity = sound_velocity
        self.transform = screen_coords_transform
        self.time_passed = 0
        self.steps_passed = 0

        # Calculate minimum grid size h and number of cells for given boundary and frequency
        fmax = 20e3  # Hz
        hmax = self.sound_velocity / (2*fmax)
        dimension = np.abs(self.end - self.begin)
        num_cells = np.cast[int](np.ceil(dimension / hmax))
        self.h = np.min(dimension / num_cells)

        # calculate maximum time step
        self.dt = self.h / (self.sound_velocity * math.sqrt(3))

        # Local spatial coordinates
        self.spatial_coords = np.zeros((num_cells[0], num_cells[1], 2))
        for x, ix in zip(np.linspace(self.h, dimension[0], num_cells[0]), range(len(self.spatial_coords))):
            for y, iy in zip(np.linspace(self.h, dimension[1], num_cells[1]), range(len(self.spatial_coords[ix]))):
                self.spatial_coords[ix][iy][0], self.spatial_coords[ix][iy][1] = x, y

        # Calculate characteristic frequencies omega
        k2 = np.pi**2 * np.sum(np.power(self.spatial_coords[:,:,dim] / dimension[dim], 2) for dim in range(2))
        self.w = self.sound_velocity * np.sqrt(k2)

        self.cos_w_dt = np.cos(self.w * self.dt)

        # Allocate mode arrays for two timesteps
        self.M_current = np.zeros((num_cells[0], num_cells[1]))
        self.M_past = np.zeros((num_cells[0], num_cells[1]))
        self.initial_force = np.zeros((num_cells[0], num_cells[1]))
        self.initial_force[30][30] = 1e8
        self.initial_force = 2 * dct2(self.initial_force) / np.power(self.w, 2) * (1 - np.cos(self.w * self.dt))

        # Font stuff
        self.font = pygame.font.SysFont('monospace', 18)

        self.step_time = self.step_time_initial
        print(f"Cells: {num_cells}, Cell size: {self.h}, Time step: {self.dt}")

    def to_screen_coords(self, x, p):
        x, p, w = (np.array([x, p, 1]) * self.transform.T).tolist()[0]
        return int(x), int(p)

    def to_screen_scale(self, x, y):
        return (x * self.transform.A[0][0], y * np.abs(self.transform.A[0][0]))

    def step_time_initial(self):
        M_new = 2 * self.M_current * self.cos_w_dt - self.M_past + self.initial_force
        self.M_current, self.M_past = M_new, self.M_current
        self.time_passed += self.dt
        self.step_time = self.step_time_consecutive

    def step_time_consecutive(self):
        M_new = 2 * self.M_current * self.cos_w_dt - self.M_past
        self.M_current, self.M_past = M_new, self.M_current
        self.time_passed += self.dt
        self.steps_passed += 1

    def calc_color(self, pressure):
        hue = pressure - np.floor(pressure) + 0.7
        return [x*255 for x in colorsys.hsv_to_rgb(hue, 1, 1)]

    def draw(self, surface):
        # draw pressure values
        rect_dims = self.to_screen_scale(self.h*0.8, self.h*0.8)
        pressures = idct2(self.M_current)
        for x, px in zip(self.spatial_coords + self.begin, pressures):
            for y, py in zip(x, px):
                rect_pos = self.to_screen_coords(y[0], y[1])
                rect_pos = (rect_pos[0] - rect_dims[0]/2, rect_pos[1] - rect_dims[1]/2)
                pygame.draw.rect(surface, self.calc_color(py), pygame.Rect(rect_pos, rect_dims))

        # draw boundaries
        left, top = self.to_screen_coords(self.begin[0], self.begin[1])
        right, bottom = self.to_screen_coords(self.end[0], self.end[1])
        pygame.draw.rect(surface, (255, 255, 255), pygame.Rect(left, top, right-left, bottom-top), 1)

        # draw text
        text = self.font.render(
            f"Domain ({self.begin[0]},{self.begin[1]})-({self.end[0]},{self.end[1]}): "
            f"c={self.sound_velocity}m/s, t={self.time_passed:.2f}s, p={np.mean(pressures):.4f}"
            f", step: {self.steps_passed}", 1, (255, 255, 255))
        surface.blit(text, self.to_screen_coords(self.begin[0], self.begin[1]))


class Simulation(Updateable):
    def __init__(self):
        transform = np.matrix([
            [500, 0, 200],
            [0, -500, 600],
            [0, 0, 1]
        ])
        self.domains = [
            Domain2D("1", (0, 0), (2, 1), 700, transform)
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

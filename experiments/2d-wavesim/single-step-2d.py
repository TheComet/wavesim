import numpy as np
from scipy.fftpack import dct


def dct2(arr):
    along_x = dct(arr, axis=0)
    along_y = dct(along_x, axis=1)
    return along_y

def idct2(arr):
    along_x = dct(arr, type=3, axis=0) / (2 * arr.shape[0])
    along_y = dct(along_x, type=3, axis=1) / (2 * arr.shape[1])
    return along_y

c = 340                    # Sound velocity in m/s
fmax = 20e3                # Max frequency in Hz
l = np.array((1, 1))       # 1x1 meter box extending from (0,0) to (lx, ly)
hmax = c / (2*fmax)        # Max cell size
cells = np.cast[int](np.ceil(l / hmax))
h = np.min(l / cells)      # Cell size
dt = h / (c * np.sqrt(3))  # Maximum timestep

# Calculate characteristic frequencies omega_i (equation 6)
k = np.zeros(cells)
for ix in range(cells[0]):
    for iy in range(cells[1]):
        k[ix][iy] = np.pi * np.sqrt(ix**2 / l[0]**2 + iy**2 / l[1]**2)
w = c * k

# Can pre-compute this
cos_w_dt = np.cos(w * dt)

# Create 2D array of pressures for initial condition
pressures = np.zeros(cells)
pressures[30][30] = 1  # impulse response

# Initialize mode arrays
M_past = np.zeros(cells)
M_current = dct2(pressures)

pressures = idct2(M_current)
print(f"initial: pmax={np.max(pressures)}, p={np.sum(pressures)}")

for i in range(50):
    # Step
    M_new = 2 * M_current * cos_w_dt - M_past
    M_current, M_past = M_new, M_current

    # Get back pressures
    pressures = idct2(M_current)
    print(f"{i}: pmax={np.max(pressures)}, p={np.sum(pressures)}")

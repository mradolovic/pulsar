import numpy as np
from astropy.time import Time
import astropy.units as u
from astropy.coordinates import SkyCoord
from pint.observatory import get_observatory
from astropy.constants import c

# --- INPUTS ---
P_bary = 0.714519700 * u.s  # barycentric period (ATNF)
RAJ = "03:32:59.368"
DECJ = "+54:34:43.57"

# Observation setup
t_start = Time("2025-10-12T00:00:00", scale="utc")
duration = 4 * u.hour
times = t_start + np.linspace(0, duration.to(u.s).value, 25) * u.s

# Observatory (example: Effelsberg)
obs = get_observatory("ef")

# Pulsar direction vector
src = SkyCoord(RAJ, DECJ, unit=(u.hourangle, u.deg))
s_hat = src.cartesian.xyz.value  # unit vector

# --- Compute observatory barycentric velocities and project along line of sight ---
v_rad = []
for t in times:
    # Some PINT versions use .posvel_gcrs(), older ones only .posvel()
    try:
        posvel = obs.posvel_gcrs(t)
    except AttributeError:
        posvel = obs.posvel(t)

    v_obs = posvel[1]  # velocity 3-vector (Quantity, m/s)
    vdot = np.dot(v_obs.to(u.m/u.s).value, s_hat) * (u.m/u.s)
    v_rad.append(vdot)

v_rad = u.Quantity(v_rad)

# Doppler factor and topocentric period
doppler = 1 + v_rad / c
P_topo = P_bary * doppler
P_mean = np.mean(P_topo.value) * u.s

# --- PRINT RESULTS ---
for t, p in zip(times, P_topo):
    print(f"{t.utc.isot} | P_topo = {p:.9f}")

print(f"\nMean topocentric period over 4h: {P_mean:.9f}")

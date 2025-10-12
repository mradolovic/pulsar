import numpy as np
from astropy.coordinates import SkyCoord, get_body_barycentric_posvel
from astropy.time import Time
import astropy.units as u
from astropy.constants import c

# Pulsar coordinates
RAJ = "03:32:59.368"
DECJ = "+54:34:43.57"
pulsar = SkyCoord(RAJ, DECJ, unit=(u.hourangle, u.deg))

# Observation times
t_start = Time("2025-10-12T00:00:00", scale="utc")
duration = 4 * u.hour
n_samples = 25
times = t_start + np.linspace(0, duration.to(u.s).value, n_samples) * u.s

# Barycentric period
P_bary = 0.714519700 * u.s

# Pulsar unit vector
p_vec = pulsar.icrs.represent_as("cartesian").xyz.value

# Compute topocentric period over time
P_topo = []

for t in times:
    # Earth barycentric velocity (AU/day -> m/s)
    pos, vel = get_body_barycentric_posvel("earth", t)
    v_earth = vel.to(u.m/u.s)

    # Radial velocity along pulsar line-of-sight
    v_rad = np.dot(v_earth.xyz.value, p_vec) * u.m/u.s

    # Doppler-corrected period
    P_topo.append(P_bary * (1 + v_rad/c))

P_topo = u.Quantity(P_topo)

# --- OUTPUT ---
for t, p in zip(times, P_topo):
    print(f"{t.utc.isot} | P_topo = {p:.9f}")

P_mean = np.mean(P_topo.value) * u.s
print(f"\nMean topocentric period over 4h: {P_mean:.9f}")

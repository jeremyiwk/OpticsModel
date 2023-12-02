# OpticsModel
##### Author: Jeremy Welsh-Kavan
##### Last Updated: Dec. 1st, 2023

This program is an attempt to create a simple simulation engine for a charged particle optical system. It will be based vaguely on my understanding of the code used in Pulsar Physics' General Particle Tracer: https://www.pulsar.nl/gpt/

### Basic Function

The general idea will be that the user specifies the optical elements in the system (apertures, lenses, multipoles, etc.) and the simulator calculates the trajectory of particles launched through the system. The output will be the coordinates of each particle when they've reached the physical end of the system. The coordinates will be in the form $q = (x,y,z,v_x,v_y,v_z)$.

### Things to work out

1. How are optical elements encoded in the system? Individual classes?

2. How are potentials calculated?

i) Should the potential be a function which is fixed at the beginning of the simulation?

ii) Should the potential only consider the optical element currently surround each particle? This neglects effects at the boundary of each element.

iii) How can we transform the potential calculation, including coloumbic interactions, into a matrix multiplication. If the phase space coordinates of the entire system are given by $\mathbf{q}$, it would be great if we could propagate the physics via some matrix $\mathbf{A}$, such that

$$
\mathbf{q}(t + \Delta t) = \mathbf{A} \mathbf{q}(t)
$$

Can we do this with coloumbic interactions too such that $\mathbf{A} = \mathbf{V} + \mathbf{C}$, where $\mathbf{V}$ and $\mathbf{C}$ are the optical system potential and the Coulombic potentials respectively?



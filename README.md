# OpticsModel
##### Author: Jeremy Welsh-Kavan
##### Last Updated: Dec. 4th, 2023

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

This preference allows us to delegate expensive matrix algebra to the OpenBLAS backend of a library like Eigen.

But, can we do this with coloumbic interactions too such that $\mathbf{A} = \mathbf{V} + \mathbf{C}$, where $\mathbf{V}$ and $\mathbf{C}$ are the optical system potential and the Coulombic potentials respectively?

### Column Element Geometry and BEM

Preliminary exploration of column elements gives some clues as to how column elements will be implemented. In practice, we have control over the voltages applied to surfaces in the column. The BEM method tells us the configuration of charges/densities required to produce a given boundary potential. In theory, this is given by solving the following integral equation for $\rho(\mathbf{r})$

$$
\varphi(\mathbf{r}) = \int \frac{ \rho(\mathbf{r'} }{ | \mathbf{r} - \mathbf{r'} | } d\mathbf{r'}
$$

In practice, this is a matrix multiplication. The positions of the given potential are divided into a grid of points

# OpticsModel
##### Author: Jeremy Welsh-Kavan
##### Last Updated: Dec. 7th, 2023

This program is an attempt to create a simple simulation engine for a charged particle optical system. For the time being, this will be entirely exploratory and unusable. This file will be a running notebook of my design strategy as I build the thing, but I will replace it with an actual README file when I'm done. 

I'm hoping the program will be based vaguely on my understanding of the BEM code used in Pulsar Physics' General Particle Tracer: https://www.pulsar.nl/gpt/, only much much much simpler and far less optimized. 

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
\varphi(\mathbf{r}) = \int \frac{ \rho(\mathbf{r'}) }{ | \mathbf{r} - \mathbf{r'} | } d\mathbf{r'}
$$

In practice, this is a matrix multiplication. To start, the given potential distribution is divided into a grid of $N$ points, whose cartesian coordinate positions are $\mathbf{x}_p, \mathbf{y}_p, \mathbf{z}_p \in \mathbb{R}^N$. Next, a list of $M$ points is chosen to represent the positions of the charges $\mathbf{x}_c, \mathbf{y}_c, \mathbf{z}_c \in \mathbb{R}^M$. Now we can form the matrix multiplication. Note, vectors in bold face are now used to indicate an array of numbers rather than cartesian vectors.

Define

$$
\mathbf{R} = \frac{1}{\sqrt{ (\mathbf{x}_p - \mathbf{x}_c^T)^2 + (\mathbf{y}_p - \mathbf{y}_c^T)^2 + (\mathbf{z}_p - \mathbf{z}_c^T)^2}}
$$ 

So $\mathbf{R} \in \mathbb{R}^{N \times M}$, and represents the inverse of the absolute value of all pairs of distances between density charges and vertices of the potential grid. The potential is then given by

$$
\mathbf{\varphi} = \mathbf{R} \mathbf{\rho}
$$

in natural units, of course. This system is either underdetermined or overdetermined depending on the choice for $N$ and $M$. However, our theoretical resolution for the potential is effectively infinite. Whereas, our resolution for the charge density is limited by our computing resources. Therefore, in practice we have $N > M$. We can solve the (overdetermined) matrix equation with a least squares solution. This yields

$$
\mathbf{\rho} = (\mathbf{R}^T\mathbf{R})^{+}\mathbf{R}^T\mathbf{\varphi}
$$

where $A^{+}$ denotes the Moore-Penrose pseudo-inverse of a matrix $A$.

Great! This gives us a list of charges, $\mathbf{\rho}$, that approximates the given potential field. Now all we have to do is compute the Coulomb force between each of the particles in our simulation and each of the density charges ... at every timestep ... and possibly also the Coulomb force between each particle in the simulation as well. 

This raises another question: once all the density charges are determined, how do we know which charges to use to propagate motion? Do we use all density charges at once, even though some of the are very far away and have negligible effects on the motion of the given particle? Or do we place a cutoff that eliminates most of the density charges from the calculation at every timestep? If we do this, how do we determine the cutoff?

### Einzel Lens Design

After some preliminary research, the Einzel lenses look like they will be fairly straightforward. The design of an Einzel lens is a series of 3 electrodes separated by insulators. The first and last electrodes are held at ground, $V=0$, while the second electrode is held at some $V>0$. The potential at the insulator surface is whatever it needs to be to satisfy Laplace's equation. For the simulated implementation of the Einzel lens, there are several things to figure out. But the initialization of the column element will be, roughly

1) The user specifies the voltage of the lens (voltage of the central electrode). If desired, user can also specify lens dimensions, electrode and insulator lengths and radius. Though, I think these are fairly consistent so these will have default values.

2) The electrode is discretized into a set of $N$ points in $\mathbb{R}^3$, $\mathbf{r}_e = (\mathbf{x}_e, \mathbf{y}_e, \mathbf{z}_e) \in \mathbb{R}^{3 \times N}$. The potential, $\mathbf{\varphi} \in \mathbb{R}^N$ is set or calculated at each of these points.

	i) This raises the question: What is the best way to discretize the element? How much resolution can we afford?

3) The set of, $M$ image charges is chosen by discretizing the entire column in the region occupied the element: $\mathbf{r}_c = (\mathbf{x}_c, \mathbf{y}_c, \mathbf{z}_c) \in \mathbb{R}^{3 \times M}$.

	i) This is perhaps the step with the most room for optimization. The number of image charges will drastically impact the time to calculate forces on moving charges and the pseudo-inverse calculation time.
	ii) The accuracy of the actual potential distribution is not equally dependent on the discretization of the cylindrical coordinates $r$, $\theta$, and $z$. In particular, it depends most significantly on $z$. The questions here will be: how few image charges can we get away with and still approximate the potential? and what is a reasonable, if not optimal, discretization of the space?
	iii) I suspect I will end up specifying only decent discretization parameters (ranges and resolutions for $r$, $\theta$, and $z$).

4) The $\mathbf{R} \in \mathbb{R}^{N\times M}$ matrix is formed as defined above. Once the image charge distribution is calculated, this matrix can also be used to determine the actual potential defined by the image charger at the element surface.

5) The charge distribution $\mathbf{\rho} \in \mathbb{R}^M$ is calculated via $\mathbf{\rho} = (\mathbf{R}^T\mathbf{R})^{+}\mathbf{R}^T\mathbf{\varphi}$.

6) The image charge distribution is then given by $(\mathbf{x}_c, \mathbf{y}_c, \mathbf{z}_c, \mathbf{\rho}) \in \mathbb{R}^{4 \times M}$.

I think the real questions for elements will be: how few charges can we get away with? For a potential with ~2000 points, roughly ~1500 charges yields an errors $\sim 10^{-4}$ and ~750 points yields errors $\sim 10^{-3}$. If I were being really rigorous, I would try to determine how the error scales when the number of point in each coordinate is increased. But I think we'll end up with a number of points roughly half to a quarter of the number in the potential. 


### Multipole Design

The construction of the multipole elements will most likely follow the Einzel lens design very closely but require a different discretization. We probably don't have to worry about multipole elements until the rest of the simulator is finished.

### Numerical Integration

Equally important to the optical element implementation is the method of numerical integration. This choice may also help determine how optical elements are applied to the numerical integration. The two main options I'm considering for numerical integration are: Runge-Kutta methods and sympletic methods, specifically Verlet. Verlet algorithms have the advantage of being explicitly time-reversible. Whatever the integration method, we would like the coordinates to be propagated as follows: 

Let $\mathbf{x}_n$  and $\mathbf{p}_n$ be the position and momentum of a particle at the $n^{\text{th}}$ timestep. Our integration method will compute $\mathbf{x}_{n+1}$  and $\mathbf{p}_{n+1}$ via













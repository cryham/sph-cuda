SPH CUDA
--------

![](https://raw.githubusercontent.com/cryham/sph-cuda/master/demo-screen.jpg)


About
-----

My old 2009 Windows program, from master's thesis. Implements

Smoothed Particle Hydrodynamics using CUDA and OpenGL, based from Particles demo.

Proving 5 times faster simulation of small scale fluids on GPU vs CPU.


Install
-------

Download zip archive from [releases](https://github.com/cryham/sph-cuda/releases), unpack, start SPH.exe, and hit F1 for help.


[Project website](https://github.com/cryham/sph-cuda/)

License: [GNU GPL v3](http://www.gnu.org/licenses/gpl-3.0.en.html).


Features
--------

The program is very fun to play with and has a lot of scenes (119).

For 56k particles it allows about 2 to 3 times slower than realtime simulation, due to small time step requirement of SPH.


It features:

- Camera with visual parameters for rendering particles
- Collider, moveable by mouse
- Emitters and accelerators for flow or fountains
- Rotors with various shape allowing propellers
- Two types of Rotational Pumps, just like those for PC water cooling
- Dye which allows flow tracing
- Moving border for shore waves generation
- Heightmap, possibly with holes
- Lots of simulation parameters with sliders on several tabs for tweaking

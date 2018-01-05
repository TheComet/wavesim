Acoustic Wave Simulator
=======================

This library can be used to automatically  generate a lattice of audio filters
from  a  3D scene. The filters can then be used in a real-time application  to
reproduce accurate propagation of sounds within the scene.

Why?  This  is  best  explained  by  quoting  a  paragraph from *Efficient and
Accurate Sound Propagation  Using  Adaptive  Rectangular  Decomposition*  (see
doc/adaptive-rectangular-decomposition.pdf)

"*In contrast to the  incredible advancement of graphics in todays interactive
applications (such as computer games), accurate sound simulation seems to have
been forgotten entirely. The  state  of  the  art is the use of reverb filters
that are not physically-based and roughly correspond to the acoustical  spaces
with  different  sizes.  In  reality,  the   acoustics  of  a  space  exhibits
perceptibly large variations depending on the  wall  material,  room  size and
geometry, along with many other factors. A handful of reverb filters common to
all scenes cannot possibly capture all the  different acoustical effects which
we  routinely observe in real life and thus, such a method at best provides  a
crude approximation of the actual acoustics  of the scene. Moreover, an artist
has to assign  these  reverb  filters  to  different  parts of the environment
manually,  which  requires   a  considerable  amount  of  time  and  effort.*"

The library is being developed with the hope of one day being used in blender.


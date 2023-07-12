#  Computer Graphics Project
## Students Team Data

Name: Inbar Lev Tov

Student Number: 316327246


Name: Avital Berendorf

Student Number: 205815111

## Project Description:
- In this project, our assignment was to load 3D models from .obj files and render not only them – but also their silhouette.
- Based on our research, we concluded that a silhouette line is in fact "the separator" between a front facing face and a back facing face. Therefore, for every face we encountered while rendering the original model, all we had to do was understand whether or not this face is front facing or back facing – and draw a silhouette line where necessary.
- That was our initial approach. In reality (after some more research), the process became a little more complicated. We discovered that it's not enough to understand the status of one face at a time – we had to understand its relationship with its neighboring faces and only than cast judgement regarding what to do.
- In order to do that, we first calculated the adjacent triangles for each face in the model, and stored them in a vector to be used later. As you can see in the picture, those adjacent triangles serve as the neighboring faces to the current face in the middle (vertices 0, 2 and 4)
![adjacent](adjacent.png)
- After we did that, we worked with two shader: the first one was used for rendering both the model itself and the ground beneath it. Notice that this shader is responsive to changes happening to the model itself (transformations), as does camera and light transformations. 
- The second shader was used solely for the silhouette. This shader is also responsive to the same parameters as the first one, but here we also have to consider the adjacent triangles. Neither the vertex shader nor the fragment shader knows how to deal with the relationship between triangles – and so we had to introduce a new shader – a geometry shader.
- Based on the vector of adjacency triangles prepared for each face, the geometry shader calculate the status of 4 triangles:
  - Current triangle (vertices 0, 2 and 4)
  - First neighbor (vertices 0, 1 and 2)
  - Second neighbor (vertices 2, 3 and 4)
  - Third neighbor (vertices 4, 5 and 0)
- If the current triangle is back facing, we don't do anything at this point. But if it's front facing, we check each neighbor and decide which lines to call the fragment shader with (depends on the status of the neighbor – front or back facing)
- That approach has produced those results
![cube](cube.gif)
![star](star.gif)
- But when we tried to move on to more complex models, that approach has produced horrible results. And so, we fixed the part where we sent lines between two front facing faces – and it worked
![homer](homer.gif)
- Note: if you wish to see our weekly updates regarding our progress, as does the number of hours we worked on, press here
[a relative link](progress.md) 

## Reading Materials:
https://prideout.net/blog/old/blog/index.html@tag=opengl-silhouette.html

https://gamedev.stackexchange.com/questions/68401/how-can-i-draw-outlines-around-3d-models

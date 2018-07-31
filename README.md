# GLSL_Subdivision
A simple practice of Subdivision and Phong Shading with GLSL

### UI control:

Keyboard : Move the camera and adjust parameters.
'w' : zoom in
'a' : move left (circle the center)
's' : zoom out
'd' : move right (circle the center)
'f' : move light position(Decrease y)
'r' : move light position(Increase y

'1' ~ '9' : change the level of subdivision(Suggest DO NOT exceed '3' , or it might crash because of the massive computation )

### Implement Steps:
Subdivision & Phong shading program:

#### Vertex shader:

Send the vertex's eye space's position, normal and texture coordinate to geometry shader.

#### Geometry shader:

Depand on level,Generate particular number of new vertex data by linear combination of three input vertex data(Triangle).

Data including of position,normal,tex_coordinate,vertexEyeSpace.

Send Data to fragment shader.

#### Fragment shader:

Compute the ambient, diffuse and specular color and accumulate them as lighting result color.
Use uniform variable to access the texture layer and compute texture color.
Final color = texture color * lighting result color.
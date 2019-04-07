----Application Controls----
Q: Edge collapse
E: Split Vertex
F: Refresh Mesh
W,A,S,D: Translate Camera
Up,Down,Left,Right: Camera Orientation
Mouse: Also orients Camera

Progressive Mesh is buggy. Edge Collapse with Quadrics works perfectly.
To esnure optimal behavior, never use split vertex. Split vertex works, but incorrectly restores
fins which results in holes. If all you do is just quadric based edge collapses then everything works
as it should. I do remove fins properly, but due to time contraints I never bothered restoring them
during vertex splits properly.

Example: If I collapse all the way to the point where the mesh dissappears, I can still restore the mesh, but
there will be a few holes here and there. These are the fins that are encountered and removed during the later 
iterations of edge collapse. More info at my website.

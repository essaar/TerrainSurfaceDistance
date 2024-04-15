I worked on this project using Qt Creator and recommend running the project using it.
The input points can be specified in the main class by changing the values of startCoord, endCoord variables (lines 11 & 12). The project does not handle data validation, so kindly enter valid values only (0 - 512).

My approach was as follows:
- Find bounding box of the path from start to end points
- If the path is parallel to X, Y or X=-Y axis, then calculate surface distance (for pre & post eruption) by traversing pixel by pixel along the path and applying the Pythagorean theorem
- Else (case 4), do the calculation piece-wise everytime the path intersects with any horizontal, vertical or diagonal lines within the bounding box
    - To achieve this a map is maintained of all intersection points with their distance from start point as the key. This helps maintain an ascending order of points for distance calculation. Each point of intersection is calculated and checked for validity against the bounds of the path.
    - To calculate the piece wise distance, first the points on the grid that connect to the intersection point are found and using interpolation, we can get the intersection point's height. Again, using the Pythagorean theorem, the surface distance is calculated.
- Finally, the difference between the accumulated distances of pre and post eruption data are printed out

This work took me more than 3 hours. Within the 3 hour limit, I was able to code everything but the height interpolation and distance calculation for the 4th case. Commiting, pushing code and creating the README was also done after the 3 hour mark.

Thank you.

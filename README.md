#### Synchronise submodules
>git submodule update --init --recursive

>git submodule update --recursive --remote
#### Build (on Linux)
>cmake -S . -B build && cmake --build build
#### Run
>./build/2_5D_GAME
#### Run ut's
>./build/2_5D_GAME_ut
##### (Make sure to be in ./2_5D_GAME directory)


### Build (on Windows)
# TODO
- [ ] Colisons
    - [x] Circle - Circle
    - [ ] Line - Circle
        - Project circle on line, to check length of line after circle center, then Project circle and line on perpendicular line, your line of interest should be there a point, and with length from center of circle we will not only determine if our line is in good direction, but also with magic of sin and cos the exact length that line up to center, summing them up you get exact length to cut off from line
    - [ ] Convex - Circle
        - Firstly check if center of circle is inside of convex with SAT for every edge, then from all previous computations you should find closest point on that edge to circle, test if it is smaller then radius of circle, and you are good to go. In Both cases result should be the direction to closest point, with it distance plus radius
    - [ ] Line - Convex
        - [ ] Line - Line (returning point of intersection)
    - [ ] Change functions so that don't return tuple of bool and optional value, but just optional value

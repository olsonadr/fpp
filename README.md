# fpp - basic ncurses ascii raycasting demo

## Some Notes
- This demo uses an std::string as its map, where a '#' represents a wall, a '.' represents open space (the floor if you look at the map as a bird's-eye view), and a 'S' represents the starting position of the player.

- Controls:
  - W, A, S, and D move you forward, left, back, and right, respectively.
  - Pushing Q or left-arrow will turn you some small amount left and pushing E or right-arrow will turn you some small amount right. Holding shift and hitting Q or E will turn you 90 degrees in that direction.
  - Pushing K toggles whether the 'tweakables' menu is displayed. While this bottom bar is visible, you can press the keys it shows to modify some values that control how the renderer and ray-tracing algorithm function. The default values are fairly solid but I used it to find these, and it can be fun to mess around with.
  - Pushing M toggles the mini-map in the top-right corner.
  - Backspace closes the program.

- The demo uses some ANSI escape sequences in order to resize the terminal window and make the cursor visible or invisible. These may not be handled correctly by default in your terminal and need to be enabled.

- I coded the original version of this in a few hours while trying to learn c++. It is inspired by my final pseudo-ray-traced project in Java for AP Computer Science near the end of the 2016-17 school year, which can be found [here](https://github.com/RadBuilder/Maze). When my partner and I made this original project, I did not know that ray-tracing existed, so I tried to create how I thought a first-person renderer could work, but it is extremely limited in application. This c++ program may be rushed and rudimentary, but was a fun proof of concept and exposure to ncurses.
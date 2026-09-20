# Game Engine Architecture

A custom game engine...

## Progress Demo One Stats

Currently my performance is not great there is a big spike in memory when the model is loaded and even when
there is no model the memory usage is still around 80mb, I'm currently working on finding the issue that
is causing this.

<p float="left">
  <img src="/GitHubImages/NoModel.png" alt="Game Image One" width="400" height="300">
  <img src="/GitHubImages/Model.png" alt="Game Image Two" width="400" height="300">
</p>

There are no memory leaks as there usage is not increasing overtime, I think I may have just misplaced some code which 
is hogging memory throughout the programs lifetime.

## Progress Demo Two Stats

There are a few issues with loading models and heightmaps in the linux build of the game engine, to combat this I'm planning on decreasing the size
of textures and number of trees spawned. I'm also going to use a way of differenciating the linux and windows version so I can create a more detailed scene on windows.

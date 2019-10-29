# Chess, the bonus scene

The featured bonus scene consists of a standard 8x8 chess board and 32 chess pieces in initial configuration.

The scene file `chess.xml` contains 14 meshes, ~19.4K vertices and over ~38.4K faces. That's about 7x more complexity
than the most complex input provided this year, `input03.xml` (the bunny). Moreover, the scene file contains 300
cameras, each in 3840x2160 resolution. Each camera generates a single frame of a 4K@60fps video, 5 seconds in length.

You may find `chess.xml` here: https://gist.github.com/bozbalci/99cb9a83331c2b1897c516912d11e89e

Video (original, 22M): http://helios.whatbox.ca:59931/share/ceng/chess.mp4

Video (low-res): https://giphy.com/gifs/h1nIA04wAQC2tmoDS8/html5

Gif (low-res, low-framerate): https://media.giphy.com/media/h1nIA04wAQC2tmoDS8/giphy.gif

## How the scene was made

We developed a Python script that parses a Wavefront OBJ file and dumps an XML compatible with the assignment
specification. We downloaded a chessboard model (a `.blend` file) from the web, and edited the model slightly.
Most notably, the mesh required to be converted to a triangle mesh, which is quite easy with the `Triangulate`
modifier included in Blender. Also, the white king and queen were improperly placed (a common mistake for
chess newbies) and we fixed it as well.

We then wrote a script to add extra camera data to an existing XML file. The camera rotation along the Y-axis is
the product of this script.

## How the scene was rendered

Rendering a single frame of this scene takes approx. 50-60 seconds on ineks. Figuring that it would take too long,
we created a droplet in DigitalOcean that was optimized for CPU-intensive tasks. 

    32-core Intel(R) Xeon(R) Platinum 8168 CPU @ 2.70GHz
    64 GB RAM
    400 GB SSD
    
The above configuration was rented at $0.952 per hour for about 2.5 hours. About half an hour into rendering, we
figured that our CPU utilization was suboptimal. After a core finishes rendering a strip of the image, it idles
until the entire image is saved to the disk. We figured that it would be better to split the XML into two chunks,
having two processes spawn threads individually for better utilization. This increased the load average by threefold,
reducing the computation time by ~50 minutes.

    # First 74 images, single process
    real    32m17.983s
    user    351m40.247s
    sys     0m6.896s
    
    # Images 75 through 187, two processes
    real    49m38.336s                    \
    user    510m53.993s                    \
    sys     0m10.591s                       \___ran parallel
                                            /
    # Images 188 through 300, two processes
    real    50m51.521s
    user    545m23.508s
    sys     0m10.607s
   
Rendering took 82 minutes. This produced 18GB of PPM files, which were converted to 257MB of PNG files in 5 minutes.

We then used FFmpeg to produce the final video.
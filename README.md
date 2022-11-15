# Termresume

My terminal based resume/homepage, available at [robinadi.li](robinadi.li), and from your terminal at `ssh -p 4422 en@robinadi.li`

- Written in C, using the excellent [notcurses library](https://github.com/dankamongmen/notcurses)
- Dynamic sizing and positioning of widgets, custom unicode line-breaking, text scrolling with scrollbars
- Shaded, point-based 3d renderer (own header-only library [term3d_c](https://github.com/dankamongmen/term3d_c)), rendering a continuously scrolling perlin noise generated terrain
- Available on the web thanks to [webssh2](https://github.com/billchurch/webssh2), modified to support sixel graphics, gpu rendering and hyperlinks (fork available [here](https://github.com/w-v/webssh2))

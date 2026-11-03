# Todo list

## Platform setup
- [x] GLFW setup
- [x] Create window
- [x] Event loop
- [x] Handle close
- [x] Error callback
- [x] Handle resize (use framebuffer size, not window size; glViewport)
- [x] Load OpenGL function pointers
- [ ] GL debug output
- [x] Vsync (glfwSwapInterval(1))

## Rendering
- [x] Shader compile/link helper
- [x] Draw a triangle
- [ ] Draw a rectangle
- [ ] Batch quads, one draw call
- [ ] Draw multiple rectangles from a dynamic buffer (glDrawArraysInstanced)
- [ ] Rasterize one glyph (FreeType? stb_truetype?)
- [ ] Glyph atlas + cache
- [ ] Render a line of hardcoded text

## Data and memory
- [ ] Arena api
- [ ] Gap buffer

## Editor
- [ ] Key and char callbacks
- [ ] Cursor movement
- [ ] Text layout: line index, cursor to pixel
- [ ] Scrolling
- [ ] File load/save
- [ ] Selection
- [ ] Undo

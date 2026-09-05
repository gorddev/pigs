# PIGS

i dunno what pigs stands for yet. 

It's a game engine / toolkit. It uses SDL3, OpenGL, and ImGui. It's written in C++23.

### hey, what's this?

It's meant to be an easy way to get a cross-platform OpenGL application off the ground and 
running really quickly. For example, a basic code example for a working application would be:

```c++
AppResult loop(Engine& eng, Application*) {
    auto vb = pg::VBuffer::make({{0, 0}, {1, 0}, {1, 1}}, 3);
    glBindVertexArray(vb.vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return APP_CONTINUE;
}

Application* init(pg::Engine& e) {
    e.window.setResizable(true);
    e.window.setResolution({400, 500});
    e.window.setName("My Application");

    shader = pg::gl::makeShaderProgram({"vert.glsl"}, {"frag.glsl"});

    pg::setLoopCallback(loop);

    return nullptr;
}
```



- SDL3 integration
- OpenGL 4.1 or ES 3.0
- ImGui
- Custom filesystem and string types
- Works with Emscripten!

### Building
cmake

## Status
Extremely experimental. Don't use this for anything important. I'm just trying to get pixels on the screen.

- [x] Windowing
- [x] Basic rendering
- [x] ImGui
- [ ] Documentation (lol)

## License
mit i think

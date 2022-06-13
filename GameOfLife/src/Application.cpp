
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <string>

#include <vector>
#include <iostream>

const int sim_width = 1024;
const int sim_height = 512;
const int window_width = 1080;
const int window_height = 720;
GLuint texture;
GLuint program;
GLuint gol_program;

std::string read(const char* filename)
{
    std::stringbuf source;
    std::ifstream in(filename);
    // verifie que le fichier existe
    if (in.good() == false)
        printf("[error] loading program '%s'...\n", filename);
    else
        printf("loading program '%s'...\n", filename);

    // lire le fichier, le caractere '\0' ne peut pas se trouver dans le source de shader
    in.get(source, 0);
    // renvoyer la chaine de caracteres
    return source.str();
}

void print_errors(GLuint program) {

    GLint isCompiled = 0;
    glGetShaderiv(program, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        // recupere les erreurs de compilation des shaders
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &isCompiled);
        std::vector<char>log(isCompiled + 1, 0);
        glGetShaderInfoLog(program, (GLsizei)log.size(), NULL, &log.front());

        printf("[error log]\n%s\n", &log.front());

        return;
    }
}

void init() {

    // cree la texture, 1 canal, entiers 32bits non signes
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0,
        GL_R32UI, sim_width, sim_height, 0,
        GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);  // GL_RED_INTEGER, sinon normalisation implicite...

    // pas la peine de construire les mipmaps / pas possible pour une texture int / uint
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    std::vector<unsigned int> col;
    for (int i = 0; i < 10; i++) {
        col.push_back(1);
    }
    //Generation d'un "gun"
    //https://en.wikipedia.org/wiki/Gun_(cellular_automaton)#/media/File:Game_of_life_glider_gun.svg
    for (int i = 0; i < 20; i++) {

        int x_offset = 45*i;
        int y_offset = 490;

        //glTexSubImage2D(GL_TEXTURE_2D, 0, 1, 4 + y_offset, 1, 2, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 1 + x_offset, 4 + y_offset, 2, 2, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
    
        glTexSubImage2D(GL_TEXTURE_2D, 0, 11 + x_offset, 3 + y_offset, 1, 3, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 12 + x_offset, 2 + y_offset, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 12 + x_offset, 6 + y_offset, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 13 + x_offset, 1 + y_offset, 2, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 13 + x_offset, 7 + y_offset, 2, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());

        glTexSubImage2D(GL_TEXTURE_2D, 0, 15 + x_offset, 4 + y_offset, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());

        glTexSubImage2D(GL_TEXTURE_2D, 0, 16 + x_offset, 6 + y_offset, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 16 + x_offset, 2 + y_offset, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 17 + x_offset, 3 + y_offset, 1, 3, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 18 + x_offset, 4 + y_offset, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());

        glTexSubImage2D(GL_TEXTURE_2D, 0, 21 + x_offset, 5 + y_offset, 2, 3, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 23 + x_offset, 4 + y_offset, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 23 + x_offset, 8 + y_offset, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());

        glTexSubImage2D(GL_TEXTURE_2D, 0, 25 + x_offset, 3 + y_offset, 1, 2, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 25 + x_offset, 8 + y_offset, 1, 2, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());

        glTexSubImage2D(GL_TEXTURE_2D, 0, 35 + x_offset, 6 + y_offset, 2, 2, GL_RED_INTEGER, GL_UNSIGNED_INT, col.data());

    }  
    

    // charger le source du vertex shader
    std::string vertex_source = read("src/vertex.glsl");
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertex_strings[] = { vertex_source.c_str() };
    glShaderSource(vertex_shader, 1, vertex_strings, NULL);
    glCompileShader(vertex_shader);
    print_errors(vertex_shader);

    // pareil pour le fragment shader
    std::string fragment_source = read("src/fragment.glsl");
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_strings[] = { fragment_source.c_str() };
    glShaderSource(fragment_shader, 1, fragment_strings, NULL);
    glCompileShader(fragment_shader);
    print_errors(fragment_shader);


    // creer le program et linker les 2 shaders
    program = glCreateProgram();
    // inclure les 2 shaders dans le program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    // linker les shaders
    glLinkProgram(program);

    // verifier que tout c'est bien passe, si les shaders ne se sont pas compiles correctement, le link du program va echouer.
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
        printf("[error] linking pogram...\n");

    // pareil pour le compute shader
    std::string compute_source = read("src/compute.glsl");
    GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    const char* compute_strings[] = { compute_source.c_str() };
    glShaderSource(compute_shader, 1, compute_strings, NULL);
    glCompileShader(compute_shader);
    print_errors(compute_shader);
    
    gol_program = glCreateProgram();
    glAttachShader(gol_program, compute_shader);
    glLinkProgram(gol_program);


    // verifier que tout c'est bien passe, si les shaders ne se sont pas compiles correctement, le link du program va echouer.
    //GLint status;
    glGetProgramiv(gol_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
        printf("[error] linking pogram...\n");

    glClearColor(0.2, 0.2, 0.2, 1);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Game Of Life", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");


    init();

    const double fpsLimit = 1.0 / 460.0;
    double lastUpdateTime = 0;  // number of seconds since the last loop
    double lastFrameTime = 0;   // number of seconds since the last frame

    double now = glfwGetTime();
    double deltaTime = now - lastUpdateTime;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        double deltaTime = now - lastUpdateTime;
        if ((now - lastFrameTime) >= fpsLimit)
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);
            //unsigned int zero = 0;
            //glClearTexImage(texture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);

            // #### Affichage ####

            // texture
            glBindImageTexture(0, texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);
            // sampler2D declare par le fragment shader
            GLint location = glGetUniformLocation(program, "image");
            glUniform1i(location, 0);

            glUseProgram(program);

            glDrawArrays(GL_QUADS, 0, 4);

            // attendre le resultat
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


            // nettoyage
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindSampler(0, 0);
            glUseProgram(0);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            // #### compute step ####

            glUseProgram(gol_program);

            // texture
            glBindImageTexture(0, texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);
            // sampler2D declare par le fragment shader
            location = glGetUniformLocation(gol_program, "image");
            glUniform1i(location, 0);

            int nx = sim_width / 8;
            int ny = sim_height / 8;
            // lancer le calcul
            glDispatchCompute(nx, ny, 1);
            // attendre le resultat
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            


            lastFrameTime = now;
            lastUpdateTime = now;
        }


        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

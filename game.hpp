#include "jengine.hpp"

#ifndef GAME_HPP
#define GAME_HPP

class Game: public JEngine {
    private:        
    protected:
    public:
        Game(void);
        ~Game(void);
        void init_phases(void);        
        void close_phases(void);
};

class Demo: public JPhase {
    private:
        // Textura fondo
        struct sized_texture texture_background;
        struct sized_texture texture_sprites;
        int vx, vy, x, y;
        int sheet_y;
        SDL_RendererFlip flip_h;
    protected:
    public:
        Demo(void);
        ~Demo(void);
        void render_phase(SDL_Renderer* sdl_renderer);
        void update_phase(void);
        void load_media(SDL_Renderer* sdl_renderer);
        void close_media(void);
        void process_input(SDL_Event *e);
        int run_phase(void);
        
};



#endif

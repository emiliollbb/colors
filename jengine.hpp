#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <vector>
#include "jphase.hpp"

#ifndef JENGINE_HPP
#define JENGINE_HPP

using namespace std;


struct sized_texture
{
    SDL_Texture* texture;
    int width;
    int height;
};

class JEngineException {
	private:
	protected:
		string message;
	public:
		JEngineException(string &what);
		string* what();
};

class Resource {
    private:
		string file_name;
		char *data;
		long size;
    protected:
    public:
		Resource(string &file_name);
		~Resource();
		string getFileName(void);
		void load_plain(void);
		void load(void);
		char* getData(void);
		long getSize(void);
};

class JEngine {
    private:
        void sync_render(void);
        void process_input_internal(SDL_Event *e);
        bool quit;
        int current_phase;
        int previous_phase;
    protected:
        //Screen dimension constants
        int SCREEN_WIDTH;
        int SCREEN_HEIGHT;
        // Frames count
        unsigned int frames;
        //The window we'll be rendering to
        SDL_Window *sdl_window;
        //The window renderer
        SDL_Renderer* sdl_renderer;
        // Display mode
        SDL_DisplayMode sdl_display_mode;
        //Game Controllers 
        SDL_Joystick *sdl_gamepads[2];
        std::vector<JPhase*> phases;        
        void render(void);        
    public:
        JEngine(void);
        virtual ~JEngine(void);
        void init(void);
        void close(void);
        void run(void);
        virtual void init_phases(void);
        virtual void close_phases(void);        
};

#endif

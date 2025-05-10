#include "chip8.h"
#include <iostream>
#include <SDL.h>

const int VIDEO_WIDTH = 64;
const int VIDEO_HEIGHT = 32;
const int SCALE = 10;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL 초기화 실패: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("CHIP-8",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              VIDEO_WIDTH * SCALE, VIDEO_HEIGHT * SCALE,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "윈도우 생성 실패: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "렌더러 생성 실패: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void drawGraphics(uint8_t gfx[32][64]) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // 배경: 검정
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // 픽셀: 흰색
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (gfx[y][x]) {
                SDL_Rect pixel = { x * SCALE, y * SCALE, SCALE, SCALE };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void handleInput(SDL_Event& e, Chip8& chip8) {
    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
        
        bool isPressed = (e.type == SDL_KEYDOWN);
        switch (e.key.keysym.sym) {
            case SDLK_x: chip8.keyState[0x0] = isPressed; break;
            case SDLK_1: chip8.keyState[0x1] = isPressed; break;
            case SDLK_2: chip8.keyState[0x2] = isPressed; break;
            case SDLK_3: chip8.keyState[0x3] = isPressed; break;
            case SDLK_q: chip8.keyState[0x4] = isPressed; break;
            case SDLK_w: chip8.keyState[0x5] = isPressed; break;
            case SDLK_e: chip8.keyState[0x6] = isPressed; break;
            case SDLK_a: chip8.keyState[0x7] = isPressed; break;
            case SDLK_s: chip8.keyState[0x8] = isPressed; break;
            case SDLK_d: chip8.keyState[0x9] = isPressed; break;
            case SDLK_z: chip8.keyState[0xA] = isPressed; break;
            case SDLK_c: chip8.keyState[0xB] = isPressed; break;
            case SDLK_4: chip8.keyState[0xC] = isPressed; break;
            case SDLK_r: chip8.keyState[0xD] = isPressed; break;
            case SDLK_f: chip8.keyState[0xE] = isPressed; break;
            case SDLK_v: chip8.keyState[0xF] = isPressed; break;
        }
    }
}

int main(int argc, char* argv[]){
    
    if (argc != 2) {
        std::cout << "Usage: chip8 <ROM file>" << std::endl;
        return -1;
    }
    Chip8 chip8 = Chip8();
    chip8.init();
    
    if(!initSDL())
        return -1;
    
    if(!chip8.loadGame(argv[1])){
       return 1;
    }
    
    SDL_Event e;
    bool running = true;
    
    // Emulator loop
    while(running){
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            handleInput(e, chip8);
        }
        
        for(int i = 0; i < 10; ++i)
        chip8.emulateCycle();
        
        if(chip8.drawFlag){
            drawGraphics(chip8.gfx);
            chip8.drawFlag = false;
        }

        SDL_Delay(15);
        
    }

    closeSDL();
    return 0;
}
#include "chip8.h"

int main(int argc, char* argv[]){
    
    Chip8 chip = Chip8();
    
    // 그래픽, 인풋 콜백 셋업
    setupGraphics();
    setupInput();
    
    chip8.init();
    if(!chip8.loadGame("pong")){
       return 0;
    }
    
    // Emulator loop
    while(true){
        chip8.emulateCycle();
        
        if(chip8.drawFlag){
            drawGraphics();
        }
        
        chip8.setKeys();
        
    }
    
    return 0;
}
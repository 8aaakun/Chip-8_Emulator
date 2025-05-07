#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdint.h>

class Chip8{
    private:
    
    uint16_t opcode;
    uint8_t memory[4096]; //4kb 메모리
    uint8_t char V[16]; // 8비트 범용 레지스터
    
    uint16_t I; // 인덱스 레지스터
    uint16_t pc; // 프로그램 카운터
    
    // 60hz로 카운트하는 타이머 레지스터
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    uint16_t stack[16];
    uint16_t sp; // 스택포인터
    
    public:
    
    uint8_t gfx[64*32]; // 그래픽 버퍼
    uint8_t key[16]; // 키패드
    
    bool drawFlag; // drawFlag가 true 면 화면 업데이트
    
    void init();
    void loadGame(char* filepath)
    void setKeys();
    void emulateCycle();
    
}

#endif
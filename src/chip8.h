#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdint.h>

class Chip8{
    private:
    
    uint16_t opcode;
    uint8_t memory[4096]; //4kb 메모리
    uint8_t V[16]; // 8비트 범용 레지스터
    
    uint16_t I; // 인덱스 레지스터
    uint16_t pc; // 프로그램 카운터
    
    // 60hz로 카운트하는 타이머 레지스터
    uint8_t delay_timer;
    uint8_t sound_timer;
    
    uint16_t stack[16];
    uint16_t sp; // 스택포인터
    
    public:
    
    Chip8();
    ~Chip8();
    
    uint8_t gfx[32][64]; // 64x32 그래픽 버퍼 
    uint8_t keyState[16]; // 키 상태
    
    bool drawFlag; // drawFlag가 true 면 화면 업데이트
    
    void init();
    void loadGame(char* filename)
    void setKeys();
    void emulateCycle();
    
    
    void DecodeOp0(uint16_t opcode);
    void Op0NNN(uint16_t opcode);
    void Op00E0(uint16_t opcode);
    void Op00EE(uint16_t opcode);
    
    void Op1NNN(uint16_t opcode);
    void Op2NNN(uint16_t opcode);
    void Op3XNN(uint16_t opcode);
    void Op4XNN(uint16_t opcode);
    void Op5XY0(uint16_t opcode);
    void Op6XNN(uint16_t opcode);
    void Op7XNN(uint16_t opcode);
    
    void DecodeOp8(uint16_t opcode);
    void Op8XY0(uint16_t opcode);
    void Op8XY1(uint16_t opcode);
    void Op8XY2(uint16_t opcode);
    void Op8XY3(uint16_t opcode);
    void Op8XY4(uint16_t opcode);
    void Op8XY5(uint16_t opcode);
    void Op8XY6(uint16_t opcode);
    void Op8XY7(uint16_t opcode);
    void Op8XYE(uint16_t opcode);
    
    void Op9XY0(uint16_t opcode);
    void OpANNN(uint16_t opcode);
    void OpBNNN(uint16_t opcode);
    void OpCXNN(uint16_t opcode);
    void OpDXYN(uint16_t opcode);
    
    void DecodeOpE(uint16_t opcode);
    void OpEX9E(uint16_t opcode);
    void OpEXA1(uint16_t opcode);
    
    void DecodeOpF(uint16_t opcode);
    void OpFX07(uint16_t opcode);
    void OpFX0A(uint16_t opcode);
    void OpFX15(uint16_t opcode);
    void OpFX18(uint16_t opcode);
    void OpFX29(uint16_t opcode);
    void OpFX33(uint16_t opcode);
    void OpFX55(uint16_t opcode);
    void OpFX65(uint16_t opcode);
    
    
}

#endif
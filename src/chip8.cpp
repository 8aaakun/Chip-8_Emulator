#include "chip8.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <stdlib.h>

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

Chip8::Chip8(){}
Chip8::~Chip8(){}

void Chip8::init(){
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    drawFlag = true;
    
    memset(gfx, 0, sizeof(gfx));
    memset(keyState, 0, sizeof(keyState));
    memset(memory, 0, sizeof(memory));
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    
    // 폰트 셋을 메모리에 로딩
    for (int i = 0; i < 80; ++i) { 
        memory[i] = chip8_fontset[i];
    }
    
}

bool Chip8::loadGame(char* filename){
    std::cout << "게임 로드 중..." << "\n";
    char path[100];
    sprintf(path, "roms/%s", filename);
    FILE* in;
    in = fopen(path, "rb");
    if(in == NULL){
        std::cerr << "게임 로드 실패" << "\n";
        return false;
    }
    fread(&memory[0x200], 0xfff, 1, in);
    fclose(in);
    
    return true;
}

void Chip8::setKeys(){
    
}

int Chip8::getKeyPressed(){ // 입력된 키 리턴
    for(uint8_t i=0; i<16; i++){
        if(keyState[i] > 0){
            return i;
        }
    }
    return -1;
}

void Chip8::emulateCycle(){
    opcode = memory[pc] << 8 | memory[pc+1]; // opcode는 2byte
    pc+=2;
    
    switch (opcode & 0xF000){
        case 0x0000: DecodeOp0(opcode); break;
        case 0x1000: Op1NNN(opcode); break;
        case 0x2000: Op2NNN(opcode); break;
        case 0x3000: Op3XNN(opcode); break;
        case 0x4000: Op4XNN(opcode); break;
        case 0x5000: Op5XY0(opcode); break;
        case 0x6000: Op6XNN(opcode); break;
        case 0x7000: Op7XNN(opcode); break;
        case 0x8000: DecodeOp8(opcode); break;
        case 0x9000: Op9XY0(opcode); break;
        case 0xA000: OpANNN(opcode); break;
        case 0xB000: OpBNNN(opcode); break;
        case 0xC000: OpCXNN(opcode); break;
        case 0xD000: OpDXYN(opcode); break;
        case 0xE000: DecodeOpE(opcode); break;
        case 0xF000: DecodeOpF(opcode); break;
        
    }
    
}

void Chip8::DecodeOp0(uint16_t opcode){
    switch(opcode & 0xF){
        case 0x0: Op00E0(opcode); break;
        case 0xE: Op00EE(opcode); break;
        default : break;
    }
}
void Chip8::Op0NNN(uint16_t opcode){
    
}
void Chip8::Op00E0(uint16_t opcode){ // 화면을 지운다.
    memset(gfx, 0, sizeof(gfx));
    drawFlag = true;
}
void Chip8::Op00EE(uint16_t opcode){ // 서브루틴에서 반환
    pc = stack[--sp];
}
void Chip8::Op1NNN(uint16_t opcode){ // NNN 주소로 이동
    pc = opcode & 0x0FFF;
}
void Chip8::Op2NNN(uint16_t opcode){ // NNN 주소 서브루틴을 호출한다.
    stack[sp++] = pc;
    pc = opcode & 0x0FFF;
}
void Chip8::Op3XNN(uint16_t opcode){ // V[X] == NN 이면 다음 명령으로 이동
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t NN = opcode & 0x00FF;
    if(V[x] == NN){
        pc += 2; // 명령어는 2byte라
    }
}
void Chip8::Op4XNN(uint16_t opcode){ // V[X] != NN 이면 다음 명령으로 이동
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t NN = opcode & 0x00FF;
    if(V[X] != NN){
        pc += 2; // 명령어는 2byte라
    }
}
void Chip8::Op5XY0(uint16_t opcode){ // V[X] == V[Y]면 다음 명령으로 이동
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    if(V[X] == V[Y]){
        pc += 2;
    }
}
void Chip8::Op6XNN(uint16_t opcode){ // V[X] = NN
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t NN = opcode & 0x00FF;
    V[X] = NN;
}
void Chip8::Op7XNN(uint16_t opcode){ // V[X] += NN
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t NN = opcode & 0x00FF;
    V[X] += NN;
}
void Chip8::DecodeOp8(uint16_t opcode){
    switch(opcode & 0xF){
        case 0x0: Op8XY0(opcode); break;
        case 0x1: Op8XY1(opcode); break;
        case 0x2: Op8XY2(opcode); break;
        case 0x3: Op8XY3(opcode); break;
        case 0x4: Op8XY4(opcode); break;
        case 0x5: Op8XY5(opcode); break;
        case 0x6: Op8XY6(opcode); break;
        case 0x7: Op8XY7(opcode); break;
        case 0xE: Op8XYE(opcode); break;
        default: break;
    }
}
void Chip8::Op8XY0(uint16_t opcode){ //V[X] = V[Y]
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    V[X] = V[Y];
}
void Chip8::Op8XY1(uint16_t opcode){ // V[X] or V[Y]
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    V[X] |= V[Y];
}
void Chip8::Op8XY2(uint16_t opcode){ // V[X] & V[Y]
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    V[X] &= V[Y];
}
void Chip8::Op8XY3(uint16_t opcode){ // V[X] xor V[Y]
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    V[X] ^= V[Y];
}
void Chip8::Op8XY4(uint16_t opcode){ // V[X] += V[Y]
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint16_t Y = (opcode & 0x00F0) >> 4;
    
    if(V[X] + V[Y] > 255) // 오버플로시 V[F] = 1 아니면 0
        V[0xF] = 1;
    else
        V[0xF] = 0;

    V[X] += V[Y];
}
void Chip8::Op8XY5(uint16_t opcode){ // V[X] -= V[Y]
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint16_t Y = (opcode & 0x00F0) >> 4;
    
    if(V[X] >= V[Y]) // 언더플로시 V[F] = 0 아니면 1
        V[0xF] = 1;
    else
        V[0xF] = 0;

    V[X] -= V[Y];
}
void Chip8::Op8XY6(uint16_t opcode){ // V[X] >>= 1
    uint16_t X = (opcode & 0x0F00) >> 8;
    
    // V[F] 에 V[X]의 LSB 저장
    V[0xF] = V[X] & 0x1;

    V[X] >>= 1;
}

void Chip8::Op8XY7(uint16_t opcode){ // V[X] = V[Y] - V[X]
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint16_t Y = (opcode & 0x00F0) >> 4;
    
    //언더플로가 나면 V[0xF]에 0 저장 아니면 1
    if(V[Y] >= V[X])
        V[0xF] = 1;
    else
        V[0xF] = 0;
    
    V[X] = V[Y] - V[X];
    
}
void Chip8::Op8XYE(uint16_t opcode){ // V[X] <<= 1
    uint16_t X = (opcode & 0x0F00) >> 8;
    
    // V[F] 에 V[X]의 MSB 저장
    V[0xF] = V[X] >> 7;

    V[X] <<= 1;
}
void Chip8::Op9XY0(uint16_t opcode){ // V[X] != V[Y] 면 다음 명령으로 이동
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint16_t Y = (opcode & 0x00F0) >> 4;
    
    if(V[X] != V[Y]){
        pc += 2;
    }
}
void Chip8::OpANNN(uint16_t opcode){ // I 를 NNN으로 설정
    I = opcode & 0x0FFF;
}
void Chip8::OpBNNN(uint16_t opcode){ // pc 를 V[0] + NNN으로 설정
    pc = V[0] + (opcode & 0x0FFF);
}
void Chip8::OpCXNN(uint16_t opcode){ // V[X] = 난수 & NN
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint16_t NN = opcode & 0x00FF;
    V[X] = rand() & NN;
}
void Chip8::OpDXYN(uint16_t opcode){ // 좌표 (V[X], V[Y]) 에 너비 8픽셀, 높이 N 픽셀의 스프라이트를 그림
    uint8_t x = V[(opcode & 0x0F00) >> 8];
    uint8_t y = V[(opcode & 0x00F0) >> 4];
    uint8_t height = opcode & 0x000F;
    uint8_t pixel;
    V[0xF] = 0;
    for (int i = 0; i < height; i++)
    {
        pixel = memory[I + i];
        for (int j = 0; j < 8; j++){
            if((pixel & (0x80 >> j)) != 0){ // 현재 위치 스프라이트에 그리는 거라면
                uint8_t ypos = (y + i) % 32;
                uint8_t xpos = (x + j) % 64;
                if (gfx[ypos][xpos] == 1) // 현재 위치에 이미 그려져 있으면
                {               
                    V[0xF] = 1; // 충돌 처리
                }
                gfx[ypos][xpos] ^= 1; // xor로 그림
            }
        }
    }

    drawFlag = true;
}
void Chip8::DecodeOpE(uint16_t opcode){
    switch(opcode & 0xF){
        case 0xE: OpEX9E(opcode); break;
        case 0x1: OpEXA1(opcode); break;
        default: break;
    }
}
void Chip8::OpEX9E(uint16_t opcode){ //V[X]에 저장된 키가 눌려있으면 다음 명령으로
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint8_t key = V[X];
    if(keyState[key] == 1){
        pc += 2;
    }
}
void Chip8::OpEXA1(uint16_t opcode){ //V[X]에 저장된 키가 눌려있지 않으면 다음 명령으로
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint8_t key = V[X];
    if(keyState[key] == 0){
        pc += 2;
    }
}
void Chip8::DecodeOpF(uint16_t opcode){
    switch(opcode & 0xFF){
        case 0x07:OpFX07(opcode); break;
        case 0x0A:OpFX0A(opcode); break;
        case 0x15:OpFX15(opcode); break;
        case 0x18:OpFX18(opcode); break;
        case 0x1E: OpFX1E(opcode); break;
        case 0x29:OpFX29(opcode); break;
        case 0x33:OpFX33(opcode); break;
        case 0x55:OpFX55(opcode); break;
        case 0x65:OpFX65(opcode); break;
        default: break;
    }
}
void Chip8::OpFX07(uint16_t opcode){ // V[X] = delay timer
    uint16_t X = (opcode & 0x0F00) >> 8;
    V[X] = delay_timer;
}
void Chip8::OpFX0A(uint16_t opcode){
    uint16_t X = (opcode & 0x0F00) >> 8;
    int8_t key = getKeyPressed();
    
    if(key != -1){
        std::cout << "Key Pressed: " << std::hex << (int)key << "\n";
        V[X] = key;
    }
    else{ // 아니면 pc를 되돌려 다시 실행
        pc -= 2;
    }
}
void Chip8::OpFX15(uint16_t opcode){ // delay timer = V[X]
    uint16_t X = (opcode & 0x0F00) >> 8;
    delay_timer = V[X];
}
void Chip8::OpFX18(uint16_t opcode){ // sound timer = V[X]
    uint16_t X = (opcode & 0x0F00) >> 8;
    sound_timer = V[X];
}
void Chip8::OpFX1E(uint16_t opcode){ // I += V[X]
    uint16_t X = (opcode & 0x0F00) >> 8;
    I += V[X];
}
void Chip8::OpFX29(uint16_t opcode){ // V[X]의 문자 스프라이트 주소를 I에 저장
    uint16_t X = (opcode & 0x0F00) >> 8;
    I = V[X] * 5; // 폰트는 5바이트를 차지함 예: 0 = 0*5, 1 = 1*5, 2 = 2*5
}
void Chip8::OpFX33(uint16_t opcode){ // V[X]를 10진수로 만든것의 100의자리를 메모리 I, 10의 자리를 I+1, 1의 자리를 I+2에 저장
    uint16_t X = (opcode & 0x0F00) >> 8;
    uint8_t dem = V[X];
    memory[I] = dem / 100;
    memory[I + 1] = (dem / 10) % 10;
    memory[I + 2] = dem % 10;
}
void Chip8::OpFX55(uint16_t opcode){ // V[0] 부터 V[X] 까지 값을 메모리 I에 순서대로 저장
    uint16_t X = (opcode & 0x0F00) >> 8;
    for (uint16_t i = 0; i <= X; i++){
        memory[I + i] = V[i];
    }
    //I = I + X + 1; // 이후 오프셋 바꾸기
}
void Chip8::OpFX65(uint16_t opcode){ // 메모리 I의 값을 V[0] 부터 V[X] 까지 순서대로 저장
    uint16_t X = (opcode & 0x0F00) >> 8;
    for (uint16_t i = 0; i <= X; i++){
        V[i] = memory[I + i];
    }
    //I = I + X + 1; // 이후 오프셋 바꾸기
}
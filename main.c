/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ 
    0x00, 0x24, 0xA0, 0xE0, 0xB2, 0xE0, 0x0D, 0x91, 0x00, 0x30, 0xE9, 0xF7, 0x11, 0x97, 0xC0, 0xE0, 0xC4, 
    0xD2, 0xE0, 0x09, 0x91, 0x1E, 0x91, 0x01, 0x17, 0x51, 0xF4, 0x0A, 0x2F, 0x0A, 0x95, 0x1C, 0x2F, 0x65, 
    0x01, 0x17, 0xB9, 0xF7, 0x0B, 0x2F, 0x1D, 0x2F, 0x01, 0x17, 0x99, 0xF7, 0x03, 0x94, 0x00, 0x00 };

    //Cada 2 bytes es una instrucción (hay que mencionar que esta en little-endian)
    //En base a esto se busca en el archivo de instrucciones para encontrar una correspondencia

    //1) 
    //0x00 0x24 -> 0x2400 -> 0010 0100 0000 0000
    //             CLR Rd -> 0010 01dd dddd dddd
    //                                  d = 0x00  
    //                                    CLR R0   

    //2) 
    //0xA0 0xE0 -> 0xE0A0 -> 1110 0000 1010 0000
    //          LDI Rd, K -> 1110 KKKK dddd KKKK
    //                                  d = 0x0A + 16 (ya que LDI inicia en 16)
    //                                  K = 0x00
    //                             LDI R26, 0x00
    
    //3) 
    //0xB2 0xE0 -> 0xE0B2 -> 1110 0000 1011 0010
    //          LDI Rd, K -> 1110 KKKK dddd KKKK
    //                                  d = 0x0B + 16 (ya que LDI inicia en 16)
    //                                  K = 0x02
    //                             LDI R27, 0x02
    
    //4) 
    //0x0D 0x91 -> 0x910D -> 1001 0001 0000 1101
    //          LD Rd, X+ -> 1110 000d dddd 1101 (el 1101 es para X+)
    //                                  d = 0x10
    //                                LD R16, X+

    //5) 
    //0x00 0x30 -> 0x3000 -> 0011 0000 0000 0000
    //          CPI Rd, K -> 0011 KKKK dddd KKKK
    //                                  d = 0x00 + 16 (ya que CPI inicia en 16)
    //                             CPI R16, 0x00

    //6) 
    //0xE9 0xF7 -> 0xF7E9 -> 1111 0111 1110 1001
    //             BRNE k -> 1111 01kk kkkk k001
    //             k = 1111101 (el signo lo dicta el bit más significativo)
    //             k = - 111101 -> le hacemos complemento a1 y queda -> 000010  y el - -> -2
    //                BRNE = PC - 0x02 -> PC - 2

    //7) 
    //0x11 0x97 -> 0x9711 -> 1001 0111 0001 0001
    //         SBIW Rd, K -> 1001 0111 KKdd KKKK
    //                                    d = 26 (d ∈ {24,26,28,30} el valor de "d" es la posicion del arreglo)
    //                                  K = 0x01
    //                            SBIW R26, 0x01

    //8) 
    //0xC0 0xE0 -> 0xE0C0 -> 1110 0000 1100 0000
    //          LDI Rd, K -> 1110 KKKK dddd KKKK
    //                                  d = 0x0C + 16 (ya que LDI inicia en 16)
    //                                  K = 0x00
    //                             LDI R28, 0x00

    //9) 
    //0xC4 0xD2 -> 0xD2C4 -> 1101 0010 1100 0100
    //            RCALL k -> 1101 kkkk kkkk kkkk
    //                                   k = 2C4 + 1 (PC ← PC + k + 1 )
    //                      RCALL  = PC + 0x02C5 

    //10) 
    //0xE0 0x09 -> 0x09E0 -> 0000 1001 1110 0000
    //         SBC Rd, Rr -> 0000 10rd dddd rrrr
    //                                  r = 0x00
    //                                  d = 0x30
    //                               SBC R30, R0

    //11) 
    //0x91 0x1E -> 0x1E91 -> 0001 1110 1001 0001
    //         ADC Rd, Rr -> 0001 11rd dddd rrrr
    //                                  r = 0x11
    //                                  d = 0x09
    //                               ADC R9, R17

    //12) 
    //0x91 0x01 -> 0x0191 -> 0000 0001 1001 0001
    //        MOVW Rd, Rr -> 0000 0001 dddd rrrr
    //                                  r = 0x01 (pareja 1)
    //                                  d = 0x09 (pareja 9)
    //                             MOVW R19: R18 <- R3: R2

    //13) 
    //0x17 0x51 -> 0x5117 -> 0101 0001 0001 0111
    //         SUBI Rd, K -> 0101 KKKK dddd KKKK
    //                                  d = 0x01 + 16 (ya que SUBI inicia en 16)
    //                                  K = 0x17
    //                            SUBI R17, 0x17

    //14) 
    //0xF4 0x0A -> 0x0AF4 -> 0000 1010 1111 0100
    //         SBC Rd, Rr -> 0000 10rd dddd rrrr
    //                                  r = 0x14
    //                                  d = 0x0F
    //                              SBC R15, R20

    //15) 
    //0x2F 0x0A -> 0x0A2F -> 0000 1010 0010 1111
    //         SBC Rd, Rr -> 0000 1010 0010 1111
    //                                  r = 0x1F
    //                                  d = 0x02
    //                               SBC R2, R31

    //16) 
    //0x95 0x1C -> 0x1C95 -> 0001 1100 1001 0101
    //         ADC Rd, Rr -> 0001 11rd dddd rrrr
    //                                  r = 0x05
    //                                  d = 0x09
    //                                ADC R9, R5

    //17) 
    //0x2F 0x65 -> 0x652F -> 0110 0101 0010 1111
    //         ORI Rd, K  -> 0001 KKKK dddd KKKK
    //                                  K = 0x5F
    //                                  d = 0x02 + 16 (ya que SBR inicia en 16)
    //                             ORI R18, 0x5F

    //18) 
    //0x01 0x17 -> 0x1701 -> 0001 0111 0000 0001
    //         CP Rd, Rr  -> 0001 01rd dddd rrrr
    //                                  r = 0x11
    //                                  d = 0x10 
    //                               CP R16, R17

    //19) 
    //0xB9 0xF7 -> 0xF7B9 -> 1111 0111 1011 1001
    //            BRNE k  -> 1111 01kk kkkk k001
    //             k = 1110111 (el signo lo dicta el bit más significativo)
    //             k = - 110111-> le hacemos complemento a1 y queda -> 001000 y el - -> -8
    //                          BRNE = PC - 0x08 -> PC - 8

    //20) 
    //0x0B 0x2F -> 0x2F0B -> 0010 1111 0000 1011
    //        MOV Rd, Rr  -> 0010 11rd dddd rrrr
    //                                  r = 0x1B
    //                                  d = 0x10 
    //                              MOV R16, R27

    //21) 
    //0x1D 0x2F -> 0x2F1D -> 0010 1111 0001 1101
    //        MOV Rd, Rr  -> 0010 11rd dddd rrrr
    //                                  r = 0x1D
    //                                 d = 0x101 
    //                              MOV R17, R29

    //22) 
    //0x01 0x17 -> 0x1701 -> 0001 0111 0000 0001
    //        CP Rd, Rr   -> 0001 01rd dddd rrrr
    //                                  r = 0x11
    //                                  d = 0x10
    //                               CP R16, R17

    //23) 
    //0x99 0xF7 -> 0xF799 -> 1111 0111 1001 1001
    //            BRNE k  -> 1111 01kk kkkk k001
    //             k = 1110011 (el signo lo dicta el bit más significativo)
    //             k = - 110011-> le hacemos complemento a1 y queda -> 001100 y el - -> -12
    //                          BRNE = PC - 0x0C -> PC - 12 

    //24) 
    //0x03 0x94 -> 0x9403 -> 1001 0100 0000 0011
    //            INC Rd  -> 1001 010d dddd 0011
    //                                  d = 0x00
    //                                    INC R0

    //25) 
    //0x00 0x00 -> 0x0000 -> 0000 0000 0000 0000
    //                NOP -> 0000 0000 0000 0000
    //                                       NOP


const uint16_t inst16_table[] = {
  {0x0}, //NOP
};

enum{
    e_NOP,
};


// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // e.g: LSR
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // e.g.: MOV,MUL,ADC,ADD,AND,
    // TO-DO: Add more types as needed
} Op_Code_t;


int main()
{
    Op_Code_t *instruction;
    printf("- Practica 2: AVR OpCode -\n");
    // Decode the instructions by cycling through the array
    for (uint8_t idx = 0; idx < sizeof(flash_mem); idx+=2)
    {
        instruction = (Op_Code_t*) &flash_mem[idx];
        if (instruction->op16 == inst16_table[e_NOP])
        {
            printf("NOP\n");
        }
        else
        {
            printf("unknown\n");
        }
    }
    return 0;
}
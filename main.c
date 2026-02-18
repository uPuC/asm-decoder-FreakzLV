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
    //                                    d = 10 + 16 (ya que LDI inicia en 16)
    //                                  K = 0x00
    //                             LDI R26, 0x00
    
    //3) 
    //0xB2 0xE0 -> 0xE0B2 -> 1110 0000 1011 0010
    //          LDI Rd, K -> 1110 KKKK dddd KKKK
    //                                    d = 11 + 16 (ya que LDI inicia en 16)
    //                                  K = 0x02
    //                             LDI R27, 0x02
    
    //4) 
    //0x0D 0x91 -> 0x910D -> 1001 0001 0000 1101
    //          LD Rd, X+ -> 1110 000d dddd 1101 (el 1101 es para X+)
    //                                    d = 16
    //                                LD R16, X+

    //5) 
    //0x00 0x30 -> 0x3000 -> 0011 0000 0000 0000
    //          CPI Rd, K -> 0011 KKKK dddd KKKK
    //                                     d = 0 + 16 (ya que CPI inicia en 16)
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
    //                                    d = 26 (d ∈ {24,26,28,30} el valor de "d" es la posicion del arreglo o también visto como "24 + d*2")
    //                                  K = 0x01
    //                            SBIW R26, 0x01

    //8) 
    //0xC0 0xE0 -> 0xE0C0 -> 1110 0000 1100 0000
    //          LDI Rd, K -> 1110 KKKK dddd KKKK
    //                                    d = 12 + 16 (ya que LDI inicia en 16)
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
    //                                    d = 30
    //                               SBC R30, R0

    //11) 
    //0x91 0x1E -> 0x1E91 -> 0001 1110 1001 0001
    //         ADC Rd, Rr -> 0001 11rd dddd rrrr
    //                                  r = 0x11
    //                                     d = 9
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
    //                                     d = 1 + 16 (ya que SUBI inicia en 16)
    //                                  K = 0x17
    //                            SUBI R17, 0x17

    //14) 
    //0xF4 0x0A -> 0x0AF4 -> 0000 1010 1111 0100
    //         SBC Rd, Rr -> 0000 10rd dddd rrrr
    //                                  r = 0x14
    //                                    d = 15
    //                              SBC R15, R20

    //15) 
    //0x2F 0x0A -> 0x0A2F -> 0000 1010 0010 1111
    //         SBC Rd, Rr -> 0000 1010 0010 1111
    //                                  r = 0x1F
    //                                     d = 2
    //                               SBC R2, R31

    //16) 
    //0x95 0x1C -> 0x1C95 -> 0001 1100 1001 0101
    //         ADC Rd, Rr -> 0001 11rd dddd rrrr
    //                                  r = 0x05
    //                                     d = 9
    //                                ADC R9, R5

    //17) 
    //0x2F 0x65 -> 0x652F -> 0110 0101 0010 1111
    //         ORI Rd, K  -> 0001 KKKK dddd KKKK
    //                                  K = 0x5F
    //                                     d = 2 + 16 (ya que SBR inicia en 16)
    //                             ORI R18, 0x5F

    //18) 
    //0x01 0x17 -> 0x1701 -> 0001 0111 0000 0001
    //         CP Rd, Rr  -> 0001 01rd dddd rrrr
    //                                  r = 0x11
    //                                    d = 16 
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
    //                                    d = 16 
    //                              MOV R16, R27

    //21) 
    //0x1D 0x2F -> 0x2F1D -> 0010 1111 0001 1101
    //        MOV Rd, Rr  -> 0010 11rd dddd rrrr
    //                                  r = 0x1D
    //                                    d = 17 
    //                              MOV R17, R29

    //22) 
    //0x01 0x17 -> 0x1701 -> 0001 0111 0000 0001
    //        CP Rd, Rr   -> 0001 01rd dddd rrrr
    //                                  r = 0x11
    //                                    d = 16
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
    //                                     d = 0
    //                                    INC R0

    //25) 
    //0x00 0x00 -> 0x0000 -> 0000 0000 0000 0000
    //                NOP -> 0000 0000 0000 0000
    //                                       NOP


const uint16_t inst16_table[] = {
    0x00,   // e_NOP   → op16  completo
    0x01,   // e_MOVW  → type7.op8
    0x97,   // e_SBIW  → type5.op8
    0x3D,   // e_BRNE  → type4.op6  (BRNE: cond==1)
    0x09,   // e_EOR   → type2.op6  (CLR si Rd==Rr)
    0x02,   // e_SBC   → type2.op6
    0x07,   // e_ADC   → type2.op6
    0x05,   // e_CP    → type2.op6
    0x0B,   // e_MOV   → type2.op6
    0x48,   // e_LDX   → type8.op7 
    0x4A,   // e_INC   → type1.op7
    0x0D,   // e_RCALL → type6.op4
    0x0E,   // e_LDI   → type3.op4
    0x03,   // e_CPI   → type3.op4
    0x05,   // e_SUBI  → type3.op4
    0x06,   // e_ORI   → type3.op4
};


enum {
    e_NOP,  
    e_MOVW,  
    e_SBIW,  
    e_BRNE,  
    e_EOR,    
    e_SBC,   
    e_ADC,    
    e_CP,    
    e_MOV,   
    e_LDX,   
    e_INC,    
    e_RCALL, 
    e_LDI,   
    e_CPI,    
    e_SUBI,  
    e_ORI,    
};


// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; // e.g: LSR, INC, LDI (el op4 para LDI determina el modo -> (X, X+, -X) =  X=0xC, X+=0xD, -X=0xE ) 
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; // e.g.: MOV,MUL,ADC,ADD,AND,SBC,CP, EOR/CLR (si empleaba el puro CLR, tendria que haber agregado otro tipo es mejor meter una comparacion en la parte del else-if más adelante)
    struct {
        uint16_t Klo : 4;
        uint16_t d4  : 4;   /* d real = d4 + 16  (porque inician en 16) */
        uint16_t Khi : 4;
        uint16_t op4 : 4;
    } type3; //Instrucciones inmediatas e.g.: LDI, CPI, SUBI, ORI
    struct {
        uint16_t cond : 3;  
        uint16_t k7   : 7;
        uint16_t op6  : 6;
    } type4; //Branches e.g.: BRNE, BREQ, ...
    struct {
        uint16_t Klo : 4;
        uint16_t d2  : 2;   /* d real = 24 + d2*2  (trabaja con valores d ∈ {24,26,28,30})*/
        uint16_t Khi : 2;
        uint16_t op8 : 8;
    } type5; // e.g.: SBIW, ADIW (con WORD)
    struct {
        uint16_t k12 : 12; 
        uint16_t op4 : 4;
    } type6; // e.g.: RCALL
    struct {
        uint16_t r4  : 4;
        uint16_t d4  : 4;
        uint16_t op8 : 8;
    } type7; // e.g.: MOVW
} Op_Code_t;

/* Funciones para impresión con valores de Rd y Rr segun el caso */
void printType2(const Op_Code_t *inst, const char *op) {
    uint8_t Rd = inst->type2.d5;
    uint8_t Rr = (inst->type2.r1 << 4) | inst->type2.r4;
    printf("%s R%d, R%d\n", op, Rd, Rr);
}

void printType3(const Op_Code_t *inst, const char *op) {
    uint8_t Rd = inst->type3.d4 + 16;
    uint8_t K  = (inst->type3.Khi << 4) | inst->type3.Klo;
    printf("%s R%d, 0x%02X\n", op, Rd, K);
}

int main()
{
    Op_Code_t *instruction;
    printf("- Practica 2: AVR OpCode -\n\n");
    // Decode the instructions by cycling through the array
    for (uint8_t idx = 0; idx < sizeof(flash_mem); idx+=2)
    {
        instruction = (Op_Code_t*) &flash_mem[idx];
        printf("[PC=0x%02X] ", idx / 2);
        if (instruction->op16 == inst16_table[e_NOP])
        {
            printf("NOP\n");
        }
        else if(instruction->type7.op8 == inst16_table[e_MOVW])
        {
            uint8_t Rd = instruction -> type7.d4 * 2;
            uint8_t Rr = instruction -> type7.r4 * 2;
            printf("MOVW R%d:R%d, R%d:R%d\n", Rd+1, Rd, Rr+1, Rr);
        }
        else if(instruction->type5.op8 == inst16_table[e_SBIW])
        {
            uint8_t K = (instruction->type5.Khi<<4) | instruction->type5.Klo;
            uint8_t Rd = 24 + instruction->type5.d2 * 2;
            printf("SBIW R%d, 0x%02X\n", Rd, K);
        }
        else if(instruction->type4.op6 == inst16_table[e_BRNE])
        {
            int8_t k = (instruction->type4.k7 & 0x40)
                     ? (int8_t)(instruction->type4.k7 | 0x80)
                     : (int8_t)(instruction->type4.k7);
            printf("BRNE PC%+d\n", k+1);
        }
        else if(instruction->type2.op6 == inst16_table[e_EOR]){
            uint8_t Rd = instruction->type2.d5;
            uint8_t Rr = (instruction->type2.r1 << 4) | instruction->type2.r4;
            (Rd == Rr) ? printf("CLR R%d\n", Rd) : printf("EOR R%d, R%d\n", Rd, Rr); 
        }
        else if(instruction->type2.op6 == inst16_table[e_SBC])
        {
            printType2(instruction, "SBC");
        }
        else if(instruction->type2.op6 == inst16_table[e_ADC])
        {
            printType2(instruction, "ADC");
        }
        else if(instruction->type2.op6 == inst16_table[e_CP])
        {
            printType2(instruction, "CP");
        }
        else if(instruction->type2.op6 == inst16_table[e_MOV])
        {
            printType2(instruction, "MOV");
        }
        else if(instruction->type1.op7 == inst16_table[e_LDX])
        {
            uint8_t Rd  = instruction->type1.d5;
            switch (instruction->type1.op4){
                case 0xC: printf("LD R%d, X\n", Rd); break;
                case 0xD: printf("LD R%d, X+\n", Rd); break;
                case 0xE: printf("LD R%d, -X\n", Rd); break;
            }
        }
        else if(instruction->type1.op7 == inst16_table[e_INC])
        {
            printf("INC R%d\n", instruction->type1.d5);
        }
        else if(instruction->type6.op4 == inst16_table[e_RCALL])
        {
            int16_t k = (instruction->type6.k12 & 0x0800)
                     ? (int16_t)(instruction->type6.k12 | 0xF000)
                     : (int16_t)(instruction->type6.k12);
            printf("RCALL PC%+d\n", k+1);
        }
        else if(instruction->type3.op4 == inst16_table[e_LDI])
        {
            printType3(instruction, "LDI");
        }
        else if(instruction->type3.op4 == inst16_table[e_CPI])
        {
            printType3(instruction, "CPI");
        }
        else if(instruction->type3.op4 == inst16_table[e_SUBI])
        {
            printType3(instruction, "SUBI");
        }
        else if(instruction->type3.op4 == inst16_table[e_ORI])
        {
            printType3(instruction, "ORI");
        }
        else {
            printf("unknown\n");
        }
    }
    return 0;
}
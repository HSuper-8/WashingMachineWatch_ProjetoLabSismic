#include "lcd.h"

int porta=0; //Úlitmo valor escrito na porta
int cursorPos = 0;

// Ligar Back Light
void LCD_BL_on(void){
    porta = porta | BIT_BL;
    PCF_write(porta);
    // PCF_write(0x08)
}
// Desligar Back Light
void LCD_BL_off(void){
    porta = porta & ~BIT_BL;
    PCF_write(porta);
    // PCF_write(0x00)
}
// Zerar toda a porta
void LCD_00(void){
    porta=0;
    PCF_write(porta);
}

// Limpa display
void ClearDisplay_LCD(){
    Pulse_LCD(1, 0, 0);
}

// Muda a posição do cursor. size (spaces)
void ChangeCursor_LCD(uint8_t size, uint8_t isCursor, uint8_t direction){
    if(direction)                                   // direita
        cursorPos += size;                      
    else                                            // esquerda
        cursorPos -= size;                       
    
    if(cursorPos > 16){                        // fim da linha
        size += 24;                                 
        cursorPos += 24;
    }
    while(size){                                     
        uint8_t byte = (0x01 << 4) | ((((isCursor << 3) & 0x08) | ((direction << 2) & 0x04)) & 0x0C);
        CursorDisplayShift_nibble_LCD(byte);        // D7 D6 D5 D4
        CursorDisplayShift_nibble_LCD(byte << 4);   // D3 D2 D1 D0
        size--;
    }
}

// escreve os nibbles
void CursorDisplayShift_nibble_LCD(uint8_t nibble){
    PCF_write (((nibble & 0xf0) | 0x08)); 
    // Manda pulso       
    PCF_write (((nibble & 0xf0) | 0x0C));       
    PCF_write (((nibble & 0xf0) | 0x08));
}

void Pulse_LCD(uint8_t byte, int RS, int RW){
    PCF_write((byte & 0xf0) | (RS & 0x01)|((RW & 0x01) << 1)| 0x08);
    PCF_write((byte & 0xf0) | (RS & 0x01)|((RW & 0x01) << 1)| 0x0C);
    PCF_write((byte & 0xf0) | (RS & 0x01)|((RW & 0x01) << 1)| 0x08);

    PCF_write(((byte << 4) & 0xf0) | (RS & 0x01)|((RW & 0x01) << 1)| 0x08);
    PCF_write(((byte << 4) & 0xf0) | (RS & 0x01)|((RW & 0x01) << 1)| 0x0C);
    PCF_write(((byte << 4) & 0xf0) | (RS & 0x01)|((RW & 0x01) << 1)| 0x08);
}


// Coloca cursor numa posição
void CursorPosition_LCD(uint8_t position){
    Pulse_LCD((position | 0x80), 0 , 0);
}


// Escreve letra no LCD
void Write_letter_LCD(char data){
    uint8_t ascii_letter = data;     // valor ASCII 
    PCF_write(((ascii_letter & 0xf0) | 0x09));       
    PCF_write(((ascii_letter & 0xf0) | 0x0D));   
    PCF_write(((ascii_letter & 0xf0) | 0x09));
    PCF_write((((ascii_letter & 0x0f) << 4)| 0x09));
    PCF_write((((ascii_letter & 0x0f) << 4)| 0x0D));  
    PCF_write((((ascii_letter & 0x0f) << 4)| 0x09));    
    // alternativa: Pulse_LCD(ascii_letter, 1, 0)   
}

// Escreve string no LCD 
void Write_string_LCD(char* string, int line){
    int count = 0;
    int size = strlen(string); 
    if(size < 17){
        if(!line){                                        
            while(count++ != size)
                Write_letter_LCD(string[count]);
        }    
        else if(line){
            for(int i = 0; i < 40; i++)
                Write_letter_LCD(' ');

            while(count++ != size)
                Write_letter_LCD(string[count]);
        }
    }
}

// Function to display float value
void Write_floatValue_LCD(float value){
    int aux = (value*1000);
    char thousand, hundred, ten, unity;
    thousand = (0x30+((aux/1000)%10));
    hundred = (0x30+((aux/100)%10));
    ten = (0x30+((aux/10)%10));
    unity = (0x30+(aux%10));
    Write_letter_LCD(thousand);
    Write_letter_LCD('.');
    Write_letter_LCD(hundred);
    Write_letter_LCD(ten);
    Write_letter_LCD(unity);
    //Write_letter_LCD('V');
}

// Function to display hexadecimal value (0 < x < 255)
void Write_8hexValue_LCD(int value){
    if(value < 0 || value > 255){
        B2_write_error_LCD();
        return;
    }
    char hex[2] = {'0','0'};
    int temp;
    int i = 0;
    if(value != 0){
        while(value != 0) {
            temp = value % 16;
            if( temp < 10){
                hex[i++] = temp + 48;
            }else{
                hex[i++] = temp + 55;
            }
            value /= 16;
        }
    }
    Write_letter_LCD('0');
    Write_letter_LCD('X');
    Write_letter_LCD(hex[1]);
    Write_letter_LCD(hex[0]);
}

// Function to display hexadecimal value ( 0 < x < 4095)
void Write_12hexValue_LCD(int value){
    if(value < 0 || value > 4095){
        B2_write_error_LCD();
        return;
    }
    char hex[3] = {'0','0','0'};
    int temp;
    int i = 0;
    if(value != 0){
        while(value != 0) {
            temp = value % 16;
            if( temp < 10){
                hex[i++] = temp + 48;
            }else{
                hex[i++] = temp + 55;
            }
            value /= 16;
        }
    }
    Write_letter_LCD('0');
    Write_letter_LCD('X');
    Write_letter_LCD(hex[2]);
    Write_letter_LCD(hex[1]);
    Write_letter_LCD(hex[0]);
}


// Function to display hexadecimal value (0 < x < 65535)
void Write_16hexValue_LCD(int value){
    if(value < 0 || value > 65535){
        B2_write_error_LCD();
        return;
    }
    char hex[4] = {'0','0','0','0'};
    int temp;
    int i = 0;
    if(value != 0){
        while(value != 0) {
            temp = value % 16;
            if( temp < 10){
                hex[i++] = temp + 48;
            }else{
                hex[i++] = temp + 55;
            }
            value /= 16;
        }
    }
    Write_letter_LCD('0');
    Write_letter_LCD('X');
    Write_letter_LCD(hex[3]);
    Write_letter_LCD(hex[2]);
    Write_letter_LCD(hex[1]);
    Write_letter_LCD(hex[0]);
}

// Função Geral para escrever hexadecimal no lcd
void Write_toHexValue_LCD(int value){
    char hex = '0';
    int temp;
    int i = 0;
    Write_letter_LCD('0');
    Write_letter_LCD('X');

    if(value != 0){
        while(value != 0) {
            temp = value % 16;
            if( temp < 10){
                hex = temp + 48;
            }else{
                hex = temp + 55;
            }
            Write_letter_LCD(hex);
            value /= 16;
        }
    }
    
    else{
        Write_letter_LCD(hex);
    }
}

// Function to display decimal value (0 < x < 255)
void Write_8DecValue_LCD(int value){
    if(value < 0 || value > 255){
        return;
    }
    char unity = (int)(value%10);
    char ten = (int)((value/10)%10);
    char hundred = (int)((value/100)%10);
    Write_letter_LCD(hundred+0x30);
    Write_letter_LCD(ten+0x30);
    Write_letter_LCD(unity+0x30);
}

// Function to display decimal value (0 < x < 65535)
void B2_16DecValue_LCD(int value){
    if(value < 0 || value > 65535){
        return;
    }
    char unity = (int)(value%10);
    char ten = (int)((value/10)%10);
    char hundred = (int)((value/100)%10);
    char thousand1 = (int)((value/1000)%10);
    char thousand2 = (int)((value/10000)%10);
    Write_letter_LCD(thousand1+0x30);
    Write_letter_LCD(thousand2+0x30);
    Write_letter_LCD(hundred+0x30);
    Write_letter_LCD(ten+0x30);
    Write_letter_LCD(unity+0x30);
}


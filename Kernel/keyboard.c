 #include <videoDriver.h>
 #include <keyboard.h>
 #include <lib.h>

 #define BUFFER_SIZE 256
 

static const char keyboard_matrix[256] = 
    {       
          0,    27,  '1',  '2',  '3',  '4',  '5',  '6',   '7',  '8',  
          '9',   '0',   '-',  '=',8,' ','Q',  'W',  'E',  'R', 
          'T',  'Y',  'U',  'I',   'O',  'P',  '[',   ']',  '\n',    0,    
         'A', 'S','D','F',  'G',  'H',  'J',  'K',  'L',  ';', 
          '\'', 0, 0,  '\\', 'Z', 'X', 'C','V','B','N',
          'M', ',', '.',  '/',  0,  '*',     0,  ' ',    0,     0,  
           0,  0, 0,0,0,0,0,0,0,0,
            0,0,4,0,0,2,0,3,0,0,
            5,0,0,0,0,0,0,0,0,0
    };
static char buff[BUFFER_SIZE] = {0}; 

static int first = 0; 
static int last = 0; 
static int cant_elems = 0; 

char next(){ 
    if(cant_elems <= 0 ) {return -1;} 
    char out = buff[first];
    first++;
    cant_elems--;
    return out;
}
 
void keyboard_handler(){ 
    uint8_t key = get_key();
    if(key <=0x79){ 
        if (cant_elems == BUFFER_SIZE){
            return;
        }
        if(last == BUFFER_SIZE) last = 0; 
        if(first == BUFFER_SIZE) first  = 0;
        buff[last++] = keyboard_matrix[key];
        cant_elems++;
    }
}

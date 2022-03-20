// define if windows:
#define WINDOWS
#define HTCW_LITTLE_ENDIAN
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io_stream.hpp>
using namespace io;
#ifdef WINDOWS
#define PATH_CHAR '\\'
#else
#define PATH_CHAR '/'
#endif
const char* get_fname(const char* filepath) {
    
    const char* sz=filepath;
    while(nullptr!=sz && *sz) {
        sz=strchr(filepath,PATH_CHAR);
        if(sz!=nullptr) {
            filepath=sz+1;
        } 
    }
    return filepath;
}
void make_ident(const char* name,char* buffer) {
    if((*name>='A' && *name<='Z')||(*name>='a' && *name<='z')||('_'==*name)) {
        *buffer=*name;
    } else
        *buffer = '_';
    ++name;
    ++buffer;
    while(*name) {
        if((*name>='A' && *name<='Z')||(*name>='a' && *name<='z')||(*name>='0'&&*name<='9')||('_'==*name)) {
            *buffer=*name;
        } else
            *buffer = '_';
        ++name;    
        ++buffer;
    }
    *buffer='\0';
}
void print_esc_char(char ch) {
    if(ch>=' ' && ch<='~' && ch!='\"' && ch!='\'') {
        printf("'%c'",ch);
        return ;
    }
    switch(ch) {
        case 0:
            printf("'\\0'");
            return;
        case '\'':
            printf("'\\\''");
            return;
        case '\"':
            printf("'\\\"'");
            return;
        case '\t':
            printf("'\\t'");
            return;
        case '\r':
            printf("'\\r'");
            return;
        case '\a':
            printf("'\\a'");
            return;
        case '\b':
            printf("'\\b'");
            return;
        case '\f':
            printf("'\\f'");
            return;
        case '\v':
            printf("'\\v'");
            return;
        default:
            printf("'\\x%02X'",(uint8_t)ch);
            return;
    }
}
void generate(file_stream& f,const char* filename) {
    char ident[256];
    make_ident(get_fname(filename),ident);
    printf("#ifndef %s_HPP\r\n",ident);
    printf("#define %s_HPP\r\n",ident);
    printf("#include <stdint.h>\r\n");
    printf("#include <io_stream.hpp>\r\n\r\n");
    printf("#ifndef PROGMEM\r\n\t#define PROGMEM\r\n#endif\r\n\r\n");
    printf("static const uint8_t %s_bin[] PROGMEM = {\r\n\t",ident);
    int cc=0;
    size_t len = f.seek(0,seek_origin::end)+1;
    f.seek(0,seek_origin::start);
    for(size_t i = 0;i<len; ++i) {
        if(i!=0)
            printf(", ");
        uint8_t b;
        f.read(&b,1);
        printf("0x%02X",(int)b);
        ++cc;
        if(0==cc%25) {
            printf("\r\n\t");
        }
    }
    printf("};\r\n\r\n");
    
    printf("static ::io::const_buffer_stream %s_stream(\r\n\t",ident);
    printf("%s_bin,%d);\r\n\r\n",ident,(int)len);
    printf("#endif // %s_HPP\r\n",ident);
}
int main(int argc, char** argv) {
    
    if(argc<2) {
        fprintf(stderr,"Input file not specified");
        return -1;
    }    
    fprintf(stderr,"// %s\r\n",argv[1]);
    if(argc>5) {
        fprintf(stderr,"Too many arguments specified");
        return -1;
    }
    size_t index = 0;
    if(argc>2) {
        index = (size_t)atoi(argv[2]);
    }
    char fc='\0',lc='\xFF';
    if(argc>3) {
        fc = sscanf(argv[3],"%c",&fc);
    }
    if(argc>4) {
        fc = sscanf(argv[4],"%c",&lc);
    }
    size_t slen = strlen(argv[1]);
    
    auto stm = io::file_stream(argv[1],io::file_mode::read);
        
    generate(stm,argv[1]);
    
    return 0;
}
#include "shim.hpp"
#ifndef COMPL12_H
#define COMPL12_H

#include "../include/abstract_scaner.h"
#include "../include/error_count.h"
#include "../include/location.h"
#include <string>"

enum Codes : unsigned short {
    None,     Unknown,Number,
    Character,String, If,    
    Then,     Else,   Assign,
    Plus,     Minus,  Mul,   
    Div,      LP,     RP
};

struct Lexem_info{
    Codes code;
    union{
        size_t    ident_index;
        size_t    string_index;
        char32_t  c;
unsigned __int128 int_val;
    };
};

class compl12 : public Abstract_scaner<Lexem_info> {
public:
    compl12() = default;
    compl12(Location* location, const Errors_and_tries& et) :
        Abstract_scaner<Lexem_info>(location, et) {};
    compl12(const compl12& orig) = default;
    virtual ~compl12() = default;
    virtual Lexem_info current_lexem();
private:
    enum Automaton_name{
        A_start,     A_unknown, A_idKeyword, 
        A_delimiter, A_number,  A_string
    };
    Automaton_name automaton; /* current automaton */

   typedef bool (compl12::*Automaton_proc)();
    /* This is the type of pointer to the member function that implements the
     * automaton that processes the lexeme. This function must return true if
     * the lexeme is not yet parsed, and false otherwise. */

    typedef void (compl12::*Final_proc)();
    /* And this is the type of the pointer to the member function that performs
     * the necessary actions in the event of an unexpected end of the lexeme. */

    static Automaton_proc procs[];
    static Final_proc     finals[];

    /* Lexeme processing functions: */
    bool start_proc();     bool unknown_proc();   
    bool idkeyword_proc(); bool delimiter_proc(); 
    bool number_proc();    bool string_proc();
    /* functions for performing actions in case of an
     * unexpected end of the token */
    void none_proc();            void unknown_final_proc();   
    void idkeyword_final_proc(); void delimiter_final_proc(); 
    void number_final_proc();    void string_final_proc();
};
#endif

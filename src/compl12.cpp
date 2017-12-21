#include "../include/compl12.h"
#include "../include/get_init_state.h"
#include "../include/search_char.h"
#include "../include/belongs.h"
#include <set>
#include <string>
#include <vector>
#include "../include/operations_with_sets.h"
compl12::Automaton_proc compl12::procs[] = {
    &compl12::start_proc(),     &compl12::unknown_proc(),   
    &compl12::idkeyword_proc(), &compl12::delimiter_proc(), 
    &compl12::number_proc(),    &compl12::string_proc()
};

compl12::Final_proc compl12::finals[] = {
    &compl12::none_proc(),            &compl12::unknown_final_proc(),   
    &compl12::idkeyword_final_proc(), &compl12::delimiter_final_proc(), 
    &compl12::number_final_proc(),    &compl12::string_final_proc()
};

enum Category {
    SPACES,          DELIMITER_BEGIN, 
    STRING_BEGIN,    NUMBER0,         
    NUMBER2,         NUMBER3,         
    NUMBER_BEGIN,    NUMBER1,         
    NUMBER4,         NUMBER5,         
    NUMBER6,         NUMBER7,         
    IDKEYWORD_BEGIN, IDKEYWORD0,      
    IDKEYWORD1,      IDKEYWORD2,      
    IDKEYWORD3,      Other
};

static const std::map<char32_t, uint32_t> categories_table = {
    {'\0', 1},     {'\X01', 1},   {'\X02', 1},   {'\X03', 1},   
    {'\X04', 1},   {'\X05', 1},   {'\X06', 1},   {'\a', 1},     
    {'\b', 1},     {'\t', 1},     {'\n', 1},     {'\v', 1},     
    {'\f', 1},     {'\r', 1},     {'\X0e', 1},   {'\X0f', 1},   
    {'\X10', 1},   {'\X11', 1},   {'\X12', 1},   {'\X13', 1},   
    {'\X14', 1},   {'\X15', 1},   {'\X16', 1},   {'\X17', 1},   
    {'\X18', 1},   {'\X19', 1},   {'\X1a', 1},   {'\X1b', 1},   
    {'\X1c', 1},   {'\X1d', 1},   {'\X1e', 1},   {'\X1f', 1},   
    {' ', 1},      {", 4},        {'*', 2},      {'+', 2},      
    {'-', 2},      {'/', 2},      {'0', 34936},  {'1', 35056},  
    {'2', 35040},  {'3', 35040},  {'4', 35040},  {'5', 35040},  
    {'6', 35040},  {'7', 35040},  {'8', 35008},  {'9', 35008},  
    {':', 2},      {'A', 47104},  {'B', 47360},  {'C', 47104},  
    {'D', 47104},  {'E', 47104},  {'F', 47104},  {'G', 45056},  
    {'H', 45056},  {'I', 45056},  {'J', 45056},  {'K', 45056},  
    {'L', 45056},  {'M', 45056},  {'N', 45056},  {'O', 45568},  
    {'P', 45056},  {'Q', 45056},  {'R', 45056},  {'S', 45056},  
    {'T', 45056},  {'U', 45056},  {'V', 45056},  {'W', 45056},  
    {'X', 46080},  {'Y', 45056},  {'Z', 45056},  {'_', 45056},  
    {'a', 47104},  {'b', 47360},  {'c', 47104},  {'d', 47104},  
    {'e', 88064},  {'f', 79872},  {'g', 45056},  {'h', 77824},  
    {'i', 53248},  {'j', 45056},  {'k', 45056},  {'l', 77824},  
    {'m', 45056},  {'n', 77824},  {'o', 45568},  {'p', 45056},  
    {'q', 45056},  {'r', 45056},  {'s', 77824},  {'t', 53248},  
    {'u', 45056},  {'v', 45056},  {'w', 45056},  {'x', 46080},  
    {'y', 45056},  {'z', 45056},  {'Ё', 45056}, {'А', 45056}, 
    {'Б', 45056}, {'В', 45056}, {'Г', 45056}, {'Д', 45056}, 
    {'Е', 45056}, {'Ж', 45056}, {'З', 45056}, {'И', 45056}, 
    {'Й', 45056}, {'К', 45056}, {'Л', 45056}, {'М', 45056}, 
    {'Н', 45056}, {'О', 45056}, {'П', 45056}, {'Р', 45056}, 
    {'С', 45056}, {'Т', 45056}, {'У', 45056}, {'Ф', 45056}, 
    {'Х', 45056}, {'Ц', 45056}, {'Ч', 45056}, {'Ш', 45056}, 
    {'Щ', 45056}, {'Ъ', 45056}, {'Ы', 45056}, {'Ь', 45056}, 
    {'Э', 45056}, {'Ю', 45056}, {'Я', 45056}, {'а', 45056}, 
    {'б', 45056}, {'в', 45056}, {'г', 45056}, {'д', 45056}, 
    {'е', 45056}, {'ж', 45056}, {'з', 45056}, {'и', 45056}, 
    {'й', 45056}, {'к', 45056}, {'л', 45056}, {'м', 45056}, 
    {'н', 45056}, {'о', 45056}, {'п', 45056}, {'р', 45056}, 
    {'с', 45056}, {'т', 45056}, {'у', 45056}, {'ф', 45056}, 
    {'х', 45056}, {'ц', 45056}, {'ч', 45056}, {'ш', 45056}, 
    {'щ', 45056}, {'ъ', 45056}, {'ы', 45056}, {'ь', 45056}, 
    {'э', 45056}, {'ю', 45056}, {'я', 45056}, {'ё', 45056}
};


uint64_t get_categories_set(char32_t c){
    auto it = categories_table.find(c);
    if(it != categories_table.end()){
        return it->second;
    }else{
        return 1ULL << Other;
    }
}
bool compl12::start_proc(){
    bool t = true;
    state = -1;
    /* For an automaton that processes a token, the state with the number (-1) is
     * the state in which this automaton is initialized. */
    if(belongs(SPACES, char_categories)){
        loc->current_line += U'\n' == ch;
        return t;
    }
    lexem_begin_line = loc->current_line;
    if(belongs(DELIMITER_BEGIN, char_categories)){
        (loc->pcurrent_char)--; automaton = A_delimiter;
        state = -1;
        return t;
    }

    if(belongs(STRING_BEGIN, char_categories)){
        (loc->pcurrent_char)--; automaton = A_string;
        state = 0;
        scanner13();
        return t;
    }

    if(belongs(NUMBER_BEGIN, char_categories)){
        (loc->pcurrent_char)--; automaton = A_number;
        state = 0;
        val_ = 0; token.code = Number;
        return t;
    }

    if(belongs(IDKEYWORD_BEGIN, char_categories)){
        (loc->pcurrent_char)--; automaton = A_idKeyword;
        state = 0;
        return t;
    }

    automaton = A_unknown;
    return t;
}

bool compl12::unknown_proc(){
    return belongs(Other, char_categories);
}

struct Keyword_list_elem{
    std::u32string keyword;
    Codes kw_code;
};

static const Keyword_list_elem kwlist[] = {
    {U"else", Else}, {U"if", If}, 
    {U"then", Then}
};

#define NUM_OF_KEYWORDS 3

#define THERE_IS_NO_KEYWORD (-1)

static int search_keyword(const std::u32string& finded_keyword){
    int result      = THERE_IS_NO_KEYWORD;
    int low_bound   = 0;
    int upper_bound = NUM_OF_KEYWORDS - 1;
    int middle;
    while(low_bound <= upper_bound){
        middle             = (low_bound + upper_bound) / 2;
        auto& curr_kw      = kwlist[middle].keyword;
        int compare_result = finded_keyword.compare(curr_kw);
        if(0 == compare_result){
            return middle;
        }
        if(compare_result < 0){
            upper_bound = middle - 1;
        }else{
            low_bound   = middle + 1;
        }
    }
    return result;
}

static const std::set<size_t> final_states_for_idkeywords = {
    1
};

bool compl12::idkeyword_proc(){
    bool t             = true;
    bool there_is_jump = false;
    switch(state){
        case 0:
            if(belongs(IDKEYWORD0, char_categories)){
                state = 1;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD1, char_categories)){
                buffer += ch;
                state = 1;
                there_is_jump = true;
            }

            break;
        case 1:
            if(belongs(IDKEYWORD2, char_categories)){
                state = 1;
                there_is_jump = true;
            }
             else if(belongs(IDKEYWORD3, char_categories)){
                buffer += ch;
                state = 1;
                there_is_jump = true;
            }

            break;
        default:
            ;
    }

    if(!there_is_jump){
        t = false;
        if(!is_elem(state, final_states_for_idkeywords)){
            printf("At line %zu unexpectedly ended identifier or keyword.", loc->current_line);
            en->increment_number_of_errors();
        }
        
        int search_result = search_keyword(buffer);
        if(search_result != THERE_IS_NO_KEYWORD) {
            token.code = kwlist[search_result].kw_code;
        }
    }

    return t;
}

static const State_for_char init_table_for_delimiters[] ={
    {4, U'*'}, {2, U'+'}, {3, U'-'}, {5, U'/'}, {0, U':'}
};

struct Elem {
    /** A pointer to a string of characters that can be crossed. */
    char32_t*       symbols;
    /** A lexeme code. */
    Codes code;
    /** If the current character matches symbols[0], then the transition to the state
     *  first_state;
     *  if the current character matches symbols[1], then the transition to the state
     *  first_state + 1;
     *  if the current character matches symbols[2], then the transition to the state
     *  first_state + 2, and so on. */
    uint16_t        first_state;
};

static const Elem delim_jump_table[] = {
    {const_cast<char32_t*>(U"="), Unknown, 1}, 
    {const_cast<char32_t*>(U""), Assign, 0},   
    {const_cast<char32_t*>(U""), Plus, 0},     
    {const_cast<char32_t*>(U""), Minus, 0},    
    {const_cast<char32_t*>(U""), Mul, 0},      
    {const_cast<char32_t*>(U""), Div, 0}
};

bool compl12::delimiter_proc(){
    bool t = false;
    if(-1 == state){
        state = get_init_state(ch, init_table_for_delimiters,
                               sizeof(init_table_for_delimiters)/sizeof(State_for_char));
        token.code = delim_jump_table[state].code;
        t = true;
        return t;
    }
    Elem elem = delim_jump_table[state];
    token.code = delim_jump_table[state].code;
    int y = search_char(ch, elem.symbols);
    if(y != THERE_IS_NO_CHAR){
        state = elem.first_state + y; t = true;
    }
    return t;
}

static const std::set<size_t> final_states_for_numbers = {
    1, 2, 3, 4, 5
};

bool compl12::number_proc(){
    bool t             = true;
    bool there_is_jump = false;
    switch(state){
        case 0:
            if(belongs(NUMBER0, char_categories)){
                val_ = val_ * 10 + digitToInt(ch);
                state = 4;
                there_is_jump = true;
            }
             else if(belongs(NUMBER1, char_categories)){
                val_ = val_ * 10 + digitToInt(ch);
                state = 3;
                there_is_jump = true;
            }

            break;
        case 1:
            if(belongs(NUMBER2, char_categories)){
                val_ = (val_ << 1) + digitToInt(ch);
                state = 1;
                there_is_jump = true;
            }

            break;
        case 2:
            if(belongs(NUMBER3, char_categories)){
                val_ = (val_ << 3) + digitToInt(ch);
                state = 2;
                there_is_jump = true;
            }

            break;
        case 3:
            if(belongs(NUMBER_BEGIN, char_categories)){
                val_ = val_ * 10 + digitToInt(ch);
                state = 3;
                there_is_jump = true;
            }

            break;
        case 4:
            if(belongs(NUMBER_BEGIN, char_categories)){
                val_ = val_ * 10 + digitToInt(ch);
                state = 3;
                there_is_jump = true;
            }
             else if(belongs(NUMBER4, char_categories)){
                state = 6;
                there_is_jump = true;
            }
             else if(belongs(NUMBER5, char_categories)){
                state = 7;
                there_is_jump = true;
            }
             else if(belongs(NUMBER6, char_categories)){
                state = 8;
                there_is_jump = true;
            }

            break;
        case 5:
            if(belongs(NUMBER7, char_categories)){
                val_ = (val_ << 4) + digitToInt(ch);
                state = 5;
                there_is_jump = true;
            }

            break;
        case 6:
            if(belongs(NUMBER2, char_categories)){
                val_ = (val_ << 1) + digitToInt(ch);
                state = 1;
                there_is_jump = true;
            }

            break;
        case 7:
            if(belongs(NUMBER3, char_categories)){
                val_ = (val_ << 3) + digitToInt(ch);
                state = 2;
                there_is_jump = true;
            }

            break;
        case 8:
            if(belongs(NUMBER7, char_categories)){
                val_ = (val_ << 4) + digitToInt(ch);
                state = 5;
                there_is_jump = true;
            }

            break;
        default:
            ;
    }

    if(!there_is_jump){
        t = false;
        if(!is_elem(state, final_states_for_numbers)){
            printf("At line %zu unexpectedly ended the number.", loc->current_line);
            en->increment_number_of_errors();
        }
        token.int_val = val_;
    }

    return t;
}

static const std::set<size_t> final_states_for_strings = {
    1
};

bool compl12::string_proc(){
    bool t             = true;
    bool there_is_jump = false;
    switch(state){
        case 0:
            if(belongs(STRING_BEGIN, char_categories)){
                state = 2;
                there_is_jump = true;
            }

            break;
        case 1:
            if(belongs(STRING_BEGIN, char_categories)){
                privet();
                state = 2;
                there_is_jump = true;
            }

            break;
        case 2:
            if(ch != U'\"'){
                buffer += ch;
                state = 2;
                there_is_jump = true;
            }
             else if(belongs(STRING_BEGIN, char_categories)){
                state = 1;
                there_is_jump = true;
            }

            break;
        default:
            ;
    }

    if(!there_is_jump){
        t = false;
        if(!is_elem(state, final_states_for_strings)){
            printf("At line %zu unexpectedly ended a string literal.", loc->current_line);
            en->increment_number_of_errors();
        }
        for(auto& c : buffer){
        c=to_upper(c);
    }
            token.string_index = strs -> insert(buffer);
    }

    return t;
}

void compl12::none_proc(){
    /* This subroutine will be called if, after reading the input text, it turned
     * out to be in the A_start automaton. Then you do not need to do anything. */
}

void compl12::unknown_final_proc(){
    /* This subroutine will be called if, after reading the input text, it turned
     * out to be in the A_unknown automaton. Then you do not need to do anything. */
}

void compl12::idkeyword_final_proc(){
    if(!is_elem(state, final_states_for_idkeywords)){
        printf("At line %zu unexpectedly ended identifier or keyword.", loc->current_line);
        en->increment_number_of_errors();
    }

    int search_result = search_keyword(buffer);
    if(search_result != THERE_IS_NO_KEYWORD) {
        token.code = kwlist[search_result].kw_code;
    }

}

void compl12::delimiter_final_proc(){
        
    token.code = delim_jump_table[state].code;
    
}

void compl12::number_final_proc(){
    if(!is_elem(state, final_states_for_numbers)){
        printf("At line %zu unexpectedly ended the number.", loc->current_line);
        en->increment_number_of_errors();
    }
    token.int_val = val_;
}

void compl12::string_final_proc(){
    if(!is_elem(state, final_states_for_strings)){
        printf("At line %zu unexpectedly ended a string literal.", loc->current_line);
        en->increment_number_of_errors();
    }
    for(auto& c : buffer){
        c=to_upper(c);
    }
    token.string_index = strs -> insert(buffer);
}

Lexem_info compl12::current_lexem(){
    automaton = A_start; token.code = None;
    lexem_begin = loc->pcurrent_char;
    bool t = true;
    while((ch = *(loc->pcurrent_char)++)){
        char_categories = get_categories_set(ch); //categories_table[ch];
        t = (this->*procs[automaton])();
        if(!t){
            /* We get here only if the lexeme has already been read. At the same time,
             * the current automaton reads the character immediately after the end of
             * the token read, based on this symbol, it is decided that the token has
             * been read and the transition to the next character has been made.
             * Therefore, in order to not miss the first character of the next lexeme,
             * we need to decrease the pcurrent_char pointer by one. */
            (loc->pcurrent_char)--;
            return token;
        }
    }
    /* Here we can be, only if we have already read all the processed text. In this
     * case, the pointer to the current symbol indicates a byte, which is immediately
     * after the zero character, which is a sign of the end of the text. To avoid
     * entering subsequent calls outside the text, we need to go back to the null
     * character. */
    (loc->pcurrent_char)--;
    /* Further, since we are here, the end of the current token (perhaps unexpected)
     * has not yet been processed. It is necessary to perform this processing, and,
     * probably, to display any diagnostics. */
    (this->*finals[automaton])();
    return token;
}



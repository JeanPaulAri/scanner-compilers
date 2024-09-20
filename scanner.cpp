#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <list>
#include <sstream>
#include <fstream>
#include <stack>

using namespace std;
set<string> tokens = {"ID", "#", "##", "###", "*", "**", "_", "[", "!(", ")", "]", "$"};
map<string,string> tokensValue = {
    {"ID", "TEXT"},
    {"#", "H1"},
    {"##", "H2"},
    {"###", "H3"},
    {"*", "ITALIC"},
    {"**", "BOLD"},
    {"_", "UNDERLINE"},
    {"[", "LINK"},
    {"!(", "IMG"},
    {")", "IMG"},
    {"]", "LINK"},
    {"$", "EOP"}
};
struct Token{
    string type = "";
    string value = "";

    int line, column;

    Token(string type="", string value="",int line=0, int column=0){
        //isToken(type, tokens) ? this->type = type : this->type = "ID";
        this->type = type;
        this->value = value;
        this->line = line;
        this->column = column;
    }
};

bool isToken(const string& str, const set<string>& querySet) {
    return querySet.find(str) != querySet.end();
}

vector<Token> scan( string& input){
   vector<Token> tokensVector;
    size_t pos = 0;
    int line = 1, column = 1;

    auto getchr = [&]() -> char {
        if (pos >= input.size()) {
            return '\0';
        }
        char ch = input[pos++];
        if (ch == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        return ch;
    };

    auto peekchar = [&]() -> char {
        if (pos >= input.size()) {
            return '\0';
        }
        return input[pos];
    };

    auto getToken = [&]() -> Token {
        char ch;
        while ((ch = getchr()) != '\0' && isspace(ch)) {
            // Ignora los espacios en blanco
        }

        if (ch == '\0') {
            return {"$", "", line, column};
        }

        Token token;
        token.line = line;
        token.column = column - 1;

        string str(1, ch);
        if (isToken(str, tokens)) {
            // Si el caracter es el inicio de un token de marcado, lee el resto del token
            string nextStr = str + peekchar();
            while (isToken(nextStr, tokens) ){
                str += getchr();
                nextStr = str + peekchar();
            }
            token.type = str;
            token.value = tokensValue[str];
        }else if (isdigit(ch)) {
            while (isdigit(ch = peekchar())) {
                str += getchr();
            }
            token.type = "ID";
            token.value = str;
        }  else if (isalpha(ch)) {
            while (isalpha( peekchar()) || peekchar() == ' ' || peekchar() == '.') { // Agrega un espacio a los caracteres permitidos para el texto sin formato
                str += getchr();
                
            }
            token.type = "ID";
            token.value = str;
        } else if (ispunct(ch)&& !isToken(string(1, ch), tokens)) { // Manejar caracteres de puntuacion como parte de texto plano
            while (ispunct(peekchar()) && !isToken(string(1, peekchar()), tokens)) {
                str += getchr();
            }
            if (isToken(str, tokens)) { // Si se formo un token especial como "!("
                token.type = str;
                token.value = tokensValue[str];
            } else { // Si es solo puntuacion normal
                token.type = "ID";
                token.value = str;
            }
            } else { // Si el caracter no es el final de la cadena
            cerr << " SCAN ERROR en (" << line << ":" << column << "): caracter inesperado '" << ch << "'\n";
            token.type = "ERROR";
            token.value = str;
        }

        return token;
    };

    Token token;
    while ((token = getToken()).type != "$") {
        if(token.type != "ERROR"){
            tokensVector.push_back(token);
        }
    }
    tokensVector.push_back({"$", "EOP", line, column}); // Add end-of-program marker

    return tokensVector;
}
void printTokens (const vector<Token>& tokens) {
    cout << "INFO SCAN - Start scanning...\n";
    for (const auto& token : tokens) {
        cout << "DEBUG SCAN - " << token.type << " [ " << token.value << " ] found at (" << token.line << ":" << token.column << ")\n";
    }
    cout << "INFO SCAN - Completed with 0 errors\n"<<endl;
}
int main(){
   
    ifstream file("input.txt");

    if (!file) {
        cerr << "No se pudo abrir el archivo input.txt";
        return 1; // Retorna un código de error
    }

    stringstream buffer;

    buffer << file.rdbuf();
    string input = buffer.str();
    vector<Token> tokens = scan(input);
    printTokens(tokens);

   

    return 0;
}
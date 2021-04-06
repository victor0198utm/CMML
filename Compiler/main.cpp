#include <stdio.h>
#include <string>
#include <iostream>
#include <list>
#include <fstream>

enum Token_Type {
    delimiter, statement, text, number, method, method_1, method_n, ident, btn, 
    qstn, gft, domain, param_int, param_1, param_2, answ_1, answ_2,  
    q_param_1, q_param_2, empty
};

struct Token
{  
    Token_Type type = empty; 
    std::string symbols;
};

struct Tree
{
    Token token;
    int counter = 0;
    Tree *children[5] = {NULL};
};

std::string error_msg = "";

bool str_is_int(std::string str){
    for(int digit_idx = 0; digit_idx < str.length(); digit_idx++){
        if(str[digit_idx] < 48 || str[digit_idx] > 57)
            return false;
    }

    return true;
}

std::list<Token> Lexer(std::string message){

    // TODO: split the code into tokens
    std::list<Token> tokens;
    std::cout<<"Splitting string "<<message<< " into tokens:\n";
    
    // handy variables for lexer
    std::string tmp = "";
    bool token_pushed = false;
    bool need_function = false;
    bool need_parameter = false;
    bool non_parameter_function = false;
    int opened_parant = 0;
    
    // lexer
    for (int i = 0; i < message.length(); i++){
        
        if(message[i] == '+'){
            // create token object and push to token list
            Token temp_token;
            
            if(tmp.compare("")!=0){
                temp_token.symbols = tmp;
                std::cout<<"==="<<tmp<<std::endl;
                temp_token.type = text;
                tokens.push_back(temp_token);    
            }
            
            temp_token.symbols = "+";
            temp_token.type = delimiter;
            tokens.push_back(temp_token);
            
            token_pushed = true;
        }
        if(message[i] == '.'){
            // create token object and push to token list
            Token new_token;
            new_token.symbols = tmp;
            new_token.type = statement;
            tokens.push_back(new_token);
            
            new_token.symbols = ".";
            new_token.type = ident;
            tokens.push_back(new_token);
            
            need_function = true;            
            token_pushed = true;
        }
        if(message[i] == '(' || message[i] == ')'){
            
            if(need_parameter == false && non_parameter_function == false){
                if(tmp.length() > 0){
                    // std::cout<<tmp<<std::endl;
                    Token tmp_token;
                    tmp_token.symbols = tmp + ")";
                    tmp_token.type = text;
                    tokens.push_back(tmp_token);
                }
                            
                token_pushed = true;
            }
            else{
                if (non_parameter_function == false){
                    if(tmp.length() == 0 && message[i] == ')'){
                        std::cout<<"parameter is missing"<<std::endl;
                        error_msg = "parameter is missing";
                        return tokens;
                    }

                    if (!str_is_int(tmp)){
                        std::cout<<"parameter is not a number"<<std::endl;
                        error_msg = "parameter is not a number";
                        return tokens;
                    }

                    Token param_token;
                    param_token.symbols = tmp;
                    param_token.type = param_int;
                    tokens.push_back(param_token);
                }

                Token open_p_token;
                open_p_token.symbols = message[i];
                open_p_token.type = ident;
                tokens.push_back(open_p_token);

                token_pushed = true;
                need_parameter = false;

                if (message[i] == ')')
                    non_parameter_function = false;
            }
        }

        
        // reset token or add curent char to token
        if(token_pushed){
            token_pushed = false;
            tmp = "";
        }else{
            // build token by adding next character
            tmp += message[i];
        }
        
        // search for the function
        if(need_function){
            if(tmp.compare("upper") == 0 || tmp.compare("lower") == 0){
                // create token object and push to token list
                Token temp_token;
                temp_token.symbols = tmp;
                temp_token.type = method;
                tokens.push_back(temp_token);
                
                need_function = false;
                non_parameter_function = true;
                tmp = "";
            }
            if(tmp.compare("repeat") == 0 || tmp.compare("rightcut") == 0 || tmp.compare("leftcut") == 0){
                // create token object and push to token list
                Token temp_token;
                temp_token.symbols = tmp;
                temp_token.type = method;
                tokens.push_back(temp_token);
            
                need_function = false;
                tmp = "";

                i++;
                if(message[i] != '('){
                    std::cout<<"no open paranthese after function"<<temp_token.symbols<<std::endl;
                    error_msg = "no open paranthese after function";
                    return tokens;
                }
                else if(message[i] == '('){
                    Token open_p_token;
                    open_p_token.symbols = "(";
                    open_p_token.type = ident;
                    tokens.push_back(open_p_token);
                }
				i++;
                need_parameter = true;
            }
        }

        if(need_parameter){
        	std::string param;
        	int q = 0;
        	while(message[i] != ')'){
        		param += message[i];
        		q++; i++;
			}
			
			if(param.length() == 0 && message[i] == ')'){
                        std::cout<<"parameter is missing"<<std::endl;
                        error_msg = "parameter is missing";
                        return tokens;
                    }

                    if (!str_is_int(param)){
                        std::cout<<"parameter is not a number"<<std::endl;
                        error_msg = "parameter is not a number";
                        return tokens;
                    }

                    Token param_token;
                    param_token.symbols = param;
                    param_token.type = param_int;
                    tokens.push_back(param_token);
        }
    } 
    std::cout<<"symbols left:"<<tmp<<std::endl;
    // add the left chars as text
    if(tmp.compare("")!=0){    
        Token temp_token;
        temp_token.symbols = tmp;
        temp_token.type = text;
        tokens.push_back(temp_token);
    }
    
    return tokens;
}

void print_tree(Tree *t){
    std::cout<<"<<token:"<<t->token.symbols<<std::endl;
    int i=0;
    while(t->children[i] != NULL){
        std::cout<<"i="<<i<<std::endl;
        print_tree(t->children[i]);
        i++;
    }
    std::cout<<"no more children\n";
}

int main ()
{
    // read the message from the file
    std::fstream newfile;
    std::string message;
    newfile.open("message.txt",std::ios::in); //open a file to perform read operation using file object
    if (newfile.is_open()){   //checking whether the file is open
        std::string tp;
        std::getline(newfile, tp); //read the first line from the file object and put it into string variable
        message = tp;
        newfile.close(); 
    }

    //----LEXER------------------------------------------------------
    // message to tokens
    std::list<Token> tokens = Lexer(message);

    // print token list
    std::list<Token>::iterator it;
    for (it = tokens.begin(); it != tokens.end(); it++) {
        std::cout<<"type:"<<it->type<<" | symbols:"<<it->symbols<<std::endl;
    }
    std::cout<<" Tokens printed\n\n";


    if (error_msg.length() > 0){
        std::cout<<"ERROR:"<<error_msg<<std::endl;
        return 1;
    }
        

    //----PARSER----------------------------------------------------------

    Tree free_nodes[100];
    int free_nodes_id = 0;
    Tree *root;
    Tree first_node;
    root = &first_node;
    std::list<Token>::iterator tk;
    for (tk = tokens.begin(); tk != tokens.end(); tk++) {
        std::cout<<"type:"<<tk->type<<" | symbols:"<<tk->symbols<<std::endl;

        if(tk->type == statement){
            // make main node
            Tree *node = &free_nodes[free_nodes_id];
            free_nodes_id++;
            node->token.type = tk->type;

            // make child node with text
            Tree *text_node = &free_nodes[free_nodes_id];
            free_nodes_id++;
            text_node->token.symbols = tk->symbols;
            text_node->token.type = text;

            // complete main node with function name
            tk++; 
            tk++;
            node->token.symbols = tk->symbols;

            // search the "("
            tk++;
            if(tk->symbols.compare("(") != 0) std::cout<<"ERROR: \"(\" missing\n";

            
            //parsing parameters
            tk++; 
            if (tk -> type == param_int){
            	Tree *param_node = &free_nodes[free_nodes_id];
            	free_nodes_id++;
            	param_node -> token.type = param_int;
        		param_node -> token.symbols = tk->symbols;		

				// make param_node the children of "method node"
				node->children[node->counter] = param_node;
				node->counter++;

				// show that the parameter was parsed
				std::cout<<"type:"<<param_node -> token.type<<" | symbols:"<<param_node -> token.symbols<<std::endl;	

				tk++; //skip the ")"        		
			}

            // build statement tree
            node->children[node->counter] = text_node;
            node->counter++;

            // add statement tree to main tree
            if(root->token.type == empty){
                root = node;
            }else{
                root->children[root->counter] = node;
                root->counter++;
            }
        }
        
        if(tk->type == delimiter){
            // add the delimiter as parent node
            Tree *new_parent = &free_nodes[free_nodes_id];
            free_nodes_id++;
            new_parent->token.symbols = tk->symbols;
            new_parent->token.type = tk->type;
            new_parent->children[0] = root;
            new_parent->counter++;
            root = new_parent; 
        }
        else if(tk->type == text){
            // root is empty -> make token as root
            if(root->token.type == empty){
                root->token.symbols = tk->symbols;
                root->token.type = tk->type;
            }
            // root is not empty -> make token as child
            else{
                // create child 
                Tree *new_child = &free_nodes[free_nodes_id];
                free_nodes_id++;
                new_child->token.symbols = tk->symbols;
                new_child->token.type = tk->type;
                // add child
                root->children[root->counter] = new_child;
                root->counter++;
            }
        }
        
    
    }
    std::cout<<" Tokens parsed\n";

    std::cout<<"\nTHE TREE\n";
    print_tree(root);

    
    // END
    std::cout<<"END";
    std::getchar();
    return 0;
}


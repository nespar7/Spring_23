#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 20000
#define max 100;

// char peek(char *s);
// char get(char *s);
// int expression();
// int number(char *s);
// int factor(char *s);
// int term(char *s);
// int expression(char *s);

struct operatorNode{
    char data;
    struct operatorNode *next;
};

struct operatorStack{
    int size;
    struct operatorNode *top;
};

struct operandNode{
    int data;
    struct operandNode *next;
};

// Defining operatorStack
struct operandStack{
    int size;
    struct operandNode *top;
};

struct operatorStack *createStack(){
    struct operatorStack *stack;
    stack = (struct operatorStack*)malloc(sizeof(struct operatorStack));
    stack->size = max;
    stack->top = NULL;

    return stack;
};

int isEmpty(struct operatorStack *stack){
    return (stack->top == NULL);
}

char peek(struct operatorStack *stack){
    char top;
    top = stack->top->data;
    return top;
}

char pop(struct operatorStack *stack){
    char pop;

    pop = stack->top->data;
    stack->top = stack->top->next;
   
   return pop;
}

void push(struct operatorStack *stack, char op){
    struct operatorNode *tmp;
    tmp = (struct operatorNode*)malloc(sizeof(struct operatorNode));
   
    tmp->data = op;
    tmp->next = stack->top;
    stack->top = tmp;
}

int isOperand(char ch){
    return (((int)ch >= 48) && ((int)ch <= 57));
}

int isOperator(char ch){
    return ((ch == '+') || (ch == '-') || (ch == '*') || (ch == '/') || (ch == '%'));
}

int prec_num(char ch){
    if((ch == '(') || (ch == ')')) return 3;
    else if((ch == '*') || (ch == '/') || (ch == '%')) return 2;
    else return 1;
}

int precedence(char op1, char op2){
    return (prec_num(op2) >= prec_num(op1));
}

char *convert(const char *exp){

    char *out; // For storing output
    struct operatorStack *stk;

    // Initialising the stack and out
    out = (char*)malloc((strlen(exp) * sizeof(char)));
    stk = createStack();

    int i, j = 0;

    // This loop checks every character 
    // and calls appropriate functions
    int len = strlen(exp);
    for(i = 0;i < len-1;i++){

        char t;

        t = *(exp+i);

        // If t is operand append to output
        if(isOperand(t)){

            *(out+j) = t;
            j++;

            continue;
        }

        // if t is space, append if the last element
        // of output isn't space
        else if(t == ' '){

            if(*(out+j-1) != ' '){
                *(out+j) = t;
                j++;
            }

            continue;
        }

        // If t is operator, pop tokens and add to output till 
        // we encounter ( or stack is empty and then push t into stack
        else if(isOperator(t)){

            if(isEmpty(stk) == 1){

                push(stk, t);
                continue;
            }

            char ch;
            ch = peek(stk);

            while(precedence(t, ch) == 1){

                if(ch == '(' || (isEmpty(stk) == 1)){
                    break;
                }


                else{
                    *(out+j) = pop(stk);
                    j++;
                }

                if(ch == '(' || (isEmpty(stk) == 1)){
                    break;
                }

                else ch = peek(stk);
            }
            
            push(stk, t);
            continue;
        }

        // If t is (, push into stack
        else if(t == '('){
            
            push(stk, t);
            continue;
        } 

        // If t is ), pop tokens and add to output
        // till ( is encountered
        else if(t == ')'){
            
            char ch;
            ch = pop(stk);
            
            while(1){
            
                *(out+j) = ch;
                j++;
                ch = peek(stk);
                
                if(ch == '('){
            
                    ch = pop(stk);
                    break;
                }

            }

            continue;
        }


    }

    // add space to output
    *(out+j) = ' ';
    j++;
    
    // while stack isn't empty, pop and add tokens to output
    while(!isEmpty(stk)){
    
        *(out+j) = pop(stk);
        j++;
    }

    return out;
}

struct operandStack *createOpStack(){
    struct operandStack *stack;
    stack = (struct operandStack*)malloc(sizeof(struct operandStack));
    stack->size = max;
    stack->top = NULL;

    return stack;
};

int opIsEmpty(struct operandStack *stack){
    return (stack->top == NULL);
}

int opPeek(struct operandStack *stack){
    int top;
    top = stack->top->data;
    return top;
}

int opPop(struct operandStack *stack){
    int pop;

    pop = stack->top->data;
    stack->top = stack->top->next;
   
   return pop;
}

void opPush(struct operandStack *stack, int op){
    struct operandNode *tmp;
    tmp = (struct operandNode*)malloc(sizeof(struct operandNode));
   
    tmp->data = op;
    tmp->next = stack->top;
    stack->top = tmp;
}

int operate(int a1, int a2, char t){
    if(t == '/') return a1 / a2;
    else if(t == '*') return a1 * a2;
    else if(t == '%') return a1 % a2;
    else if(t == '+') return a1 + a2;
    else return a1 - a2;
}

int evaluate(const char *exp){
    int out; // For storing output
    struct operandStack *stk;

    // Initialising the stack and out
    // out = (char*)malloc((strlen(exp) * sizeof(char)));
    stk = createOpStack();

    int i, op, op1, op2, res;

    // This loop checks every character
    for(i = 0;i < strlen(exp)-1;i++){

        char t;

        t = *(exp+i);

        // if t is operand, find the integer and
        // push into stack
        if(isOperand(t)){

            op = (int)t - 48;
            i++;
            t = *(exp+i);

            while(t != ' '){
                op = 10*op + ((int)t - 48);
                i++;
                t = *(exp+i);
            }            

            opPush(stk, op);

            continue;
        }

        // If t is operator, pop the last two values, perform
        // operation, and push the result into stack
        else if(isOperator(t)){

            op1 = opPop(stk);
            op2 = opPop(stk);

            res = operate(op2, op1, t);

            opPush(stk, res); 

            continue;
        }

    }

    // out is the top->data of stk
    out = opPop(stk);

    return out;
}

int main(){
    int sockfd, newsockfd;
    int clilen;
    int response;
    struct sockaddr_in serv_addr, cli_addr;

    int i;
    char buff[100];
    
    // Opening a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket");
        exit(0);
    }
    printf("TCP server socket created :)\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    response = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(response < 0){
        perror("Unable to bind to localhost");
        exit(0);
    }
    printf("Bound to port number: %d\n", SERVER_PORT);

    response = listen(sockfd, 5);
    if(response < 0){
        perror("Listen failed");
        exit(0);
    }
    printf("Listening...\n");

    while(1){
        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        if(newsockfd < 0){
            perror("Accept failed");
            exit(0);
        }

        response = recv(newsockfd, buff, strlen(buff)+1, 0);
        if(response < 0){
            perror("Cannot receive data");
            close(newsockfd);
            exit(0);
        }
        printf("Received expression: %s\n", buff);

        // DEAL WITH THE ARITHMETIC HERE
        char *result;

        result = convert(buff);

        int value = evaluate(result);

        sprintf(buff, "%d", value);

        response = send(newsockfd, buff, strlen(buff)+1, 0);
        if(response < 0){
            perror("Cannot send the result");
            close(newsockfd);
            exit(0);
        }

        close(newsockfd);
    }

    close(sockfd);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>



typedef struct node{
    char* line;
    struct node* next;
}textStruct;

typedef struct stackNode{
    textStruct* startLine;
    bool cmd;
    int linesNum;
    int firstLine;
    struct stackNode* next;
}stackNode;

int isEmpty(struct stackNode* root){
    return !root;
}

void push(struct stackNode** root, struct stackNode* data){
    data->next= *root;
    *root = data;
}

struct stackNode* pop(struct stackNode** root){
    if (isEmpty(*root)){
        return NULL;
    }
    struct stackNode* temp =*root;
    *root = (*root)->next;

    return temp;
    //ricordati di fare la free
}

struct stackNode* newNode(textStruct* start, bool cmd, int num, int startLine){
    struct stackNode* node = (struct stackNode*)malloc(sizeof(struct stackNode));
    node->startLine = start;
    node->next = NULL;
    node->cmd = cmd;
    node->linesNum = num;
    node->firstLine = startLine;
    return node;
}

textStruct* fullCopy(textStruct** head);
textStruct* fullCopyNew(textStruct** head);
textStruct* copyNew(textStruct** head);
textStruct* copyNewUndo(textStruct** head, textStruct* textToCopy);
textStruct* goTo(long addr, textStruct* current, textStruct* head);
textStruct* removeTry(textStruct** head, long position);
textStruct* removeTrySeq(textStruct** head, long position,int num);
textStruct* concatenateText(textStruct* base,textStruct* add);

int main(int argc, char* argv[]) {



    char comando[50];
    char charcomando;
    long addr1 , addr2;
    textStruct* textHead = NULL;
    textHead = (textStruct*)malloc(sizeof(textStruct));
    textHead->line = NULL;
    textHead->next = NULL;
    textStruct* current = textHead;
    textStruct* currentPrint = textHead;
    textStruct* currentUndo = textHead;
    int lastCposition = 1;
    long maxLine = 1;
    char *p;

    int undoCount = 0;
    int undoStackHeight = 0;
    int redoStackHeight = 0;


    struct stackNode *rootUndo = NULL;
    struct stackNode *rootRedo = NULL;

    bool clearText = false;
    bool copyAvailable = false;
    int lastUndoCount = 0;
    int undoOfCopy = 0;
    textStruct* textCopy;



    while (1) {
        fgets(comando,50,stdin);
        charcomando = comando[strlen(comando)-2];
        p = comando;
        addr1 = -10;
        while (*p) { // While there are more characters to process...
            if ( isdigit(*p) || ( (*p=='-'||*p=='+') && isdigit(*(p+1)) )) {
                // Found a number
                long val = strtol(p, &p, 10); // Read number
                if(addr1==-10){
                    addr1=val;
                }else{
                    addr2=val;
                }

            } else {
                // Otherwise, move on to the next character.
                p++;
            }
        }


        switch (charcomando) {
            case 'c':
                if (undoCount != 0) {
                    struct stackNode *nodeUndo = NULL;
                    if (undoCount > 0) { //faccio undo
                        while (undoCount != 0) {
                            nodeUndo = pop(&rootUndo);
                            undoStackHeight--;
                            switch (nodeUndo->cmd) {
                                textStruct *toSaveRedo;
                                case (false):
                                    if(nodeUndo->linesNum==-1){
                                        textStruct *tmp1 = nodeUndo->startLine;
                                        nodeUndo->startLine = textHead;
                                        textHead = tmp1;
                                        push(&rootRedo,nodeUndo);
                                        break;
                                    }
                                    //creo nodo per la redo
                                    1+1;
                                    toSaveRedo = NULL;
                                    textStruct* toAddRedo;


                                    toAddRedo = removeTrySeq(&textHead, nodeUndo->firstLine,nodeUndo->linesNum);
                                    toSaveRedo= toAddRedo;
                                    for(int i = 1; i<nodeUndo->linesNum; i++){
                                        toSaveRedo= toSaveRedo->next;
                                    }
                                    toSaveRedo->next=NULL;
                                    if (nodeUndo->startLine == NULL){
                                        nodeUndo->startLine = toAddRedo;
                                    }else{
                                        textStruct* tmp;
                                        if(nodeUndo->firstLine==1){
                                            currentUndo = textHead;
                                            tmp = textHead->next;
                                            textHead=nodeUndo->startLine;
                                            textStruct* tmp2 = nodeUndo->startLine;
                                            while(tmp2->next != NULL){
                                                tmp2 = tmp2->next;
                                            }
                                            tmp2->next = currentUndo;

                                        }else {
                                            currentUndo = goTo(nodeUndo->firstLine - 1, currentUndo, textHead);
                                            tmp = currentUndo->next;

                                            currentUndo->next = nodeUndo->startLine;
                                            textStruct *tmp2 = nodeUndo->startLine;
                                            while (tmp2->next != NULL) {
                                                tmp2 = tmp2->next;
                                            }
                                            tmp2->next = tmp;
                                        }
                                        nodeUndo->startLine = toAddRedo;
                                    }
                                    push(&rootRedo,nodeUndo);
                                    break;
                                case (true):

                                    if (nodeUndo->firstLine < 0) {
                                        nodeUndo->firstLine=-99;
                                        push(&rootRedo,nodeUndo);
                                        break;
                                    }

                                    textStruct *tmp1 = nodeUndo->startLine;
                                    nodeUndo->startLine = textHead;
                                    textHead = tmp1;

                                    push(&rootRedo,nodeUndo);
                                    break;
                            }
                            undoCount--;
                            redoStackHeight++;
                        }
                    } else if (undoCount < 0) { //faccio redo
                        while (undoCount != 0) {
                            nodeUndo = pop(&rootRedo);
                            redoStackHeight--;
                            textStruct* old_version;
                            textStruct* to_add;
                            switch (nodeUndo->cmd){
                                case (false):
                                    if(nodeUndo->linesNum==-1){
                                        textStruct *tmp1 = nodeUndo->startLine;
                                        nodeUndo->startLine = textHead;
                                        textHead = tmp1;
                                        push(&rootUndo,nodeUndo);
                                        undoStackHeight++;
                                        break;
                                    }
                                    old_version = NULL;
                                    currentUndo = goTo(nodeUndo->firstLine, currentUndo, textHead);
                                    for(int i = 1; i <= nodeUndo->linesNum; i++){
                                        textStruct *tmp = nodeUndo->startLine;
                                        nodeUndo->startLine = nodeUndo->startLine->next;
                                        tmp->next = NULL;
                                        to_add = copyNewUndo(&currentUndo, tmp);
                                        if(to_add != NULL){
                                            to_add->next = NULL;
                                            old_version = concatenateText(old_version,to_add);
                                        }
                                        currentUndo=currentUndo->next;
                                    }
                                    nodeUndo->startLine = old_version; //nodeU1->startLine = old_version;
                                    push(&rootUndo,nodeUndo);//push(&rootUndo,nodeU1);
                                    undoStackHeight++;

                                    break;
                                case (true):

                                    if(nodeUndo->firstLine < 0){
                                        nodeUndo->firstLine=-99;//nodeU2->firstLine=-99;
                                        push(&rootUndo,nodeUndo);//push(&rootUndo,nodeU2);
                                        undoStackHeight++;
                                        break;
                                    }


                                    textStruct *tmp1 = nodeUndo->startLine;
                                    nodeUndo->startLine = textHead;
                                    textHead = tmp1;


                                    push(&rootUndo,nodeUndo);
                                    undoStackHeight++;

                                    break;
                            }
                            undoCount++;
                        }
                    }
                    //printf("FACCIO UNDO PARI A: %d",undoCount);
                }
                if (addr2 > maxLine) {
                    maxLine = addr2;
                }

                struct stackNode *node = newNode(NULL, false, (addr2 + 1) - addr1, addr1);

                textStruct *toSave = NULL;
                bool flag = false;
                for (long int i = addr1; i <= addr2; i++) {
                    if (lastCposition + 1 == addr1) {
                        current = current->next;
                        flag = true;
                    } else {

                        if (i == addr1) {
                            node->startLine = fullCopyNew(&textHead);
                            node->linesNum = -1;
                            current = goTo(i, current, textHead);
                        } else {
                            current = current->next;
                        }
                    }
                    if(flag==false){
                        if (current != NULL) {
                            copyNew(&current);
                        }

                    }else {

                        if (current != NULL) {
                            toSave = copyNew(&current);
                        }
                        if (toSave != NULL) {
                            if (i == addr1) {
                                node->startLine = toSave;
                            } else {
                                textStruct *temp = node->startLine;
                                while (temp->next != NULL) {
                                    temp = temp->next;
                                }
                                temp->next = toSave;
                            }
                        }
                    }
                }
                push(&rootUndo, node);
                undoStackHeight++;

                //scarico la pila delle redo
                while(!isEmpty(rootRedo)){
                    rootRedo=NULL;
                }
                redoStackHeight=0;

                fgetc(stdin);
                fgetc(stdin);

                lastCposition = addr2;
                break;

            case 'd': {
                if (undoCount != 0) {
                    struct stackNode *nodeUndo = NULL;
                    if (undoCount > 0) { //faccio undo
                        while (undoCount != 0) {
                            nodeUndo = pop(&rootUndo);
                            undoStackHeight--;
                            switch (nodeUndo->cmd) {
                                case (false):
                                    if(nodeUndo->linesNum==-1){
                                        textStruct *tmp1 = nodeUndo->startLine;
                                        nodeUndo->startLine = textHead;
                                        textHead = tmp1;
                                        push(&rootRedo,nodeUndo);
                                        break;
                                    }
                                    //creo nodo per la redo
                                    1+1;
                                    textStruct* toSaveRedo = NULL;
                                    textStruct* toAddRedo;

                                    toAddRedo = removeTrySeq(&textHead, nodeUndo->firstLine,nodeUndo->linesNum);
                                    toSaveRedo= toAddRedo;
                                    for(int i = 1; i<nodeUndo->linesNum; i++){
                                        toSaveRedo= toSaveRedo->next;
                                    }
                                    toSaveRedo->next=NULL;
                                    if (nodeUndo->startLine == NULL){
                                        nodeUndo->startLine = toAddRedo;
                                    }else{
                                        textStruct* tmp;
                                        if(nodeUndo->firstLine==1){
                                            currentUndo = textHead;
                                            tmp = textHead->next;
                                            textHead=nodeUndo->startLine;
                                            textStruct* tmp2 = nodeUndo->startLine;
                                            while(tmp2->next != NULL){
                                                tmp2 = tmp2->next;
                                            }
                                            tmp2->next = currentUndo;

                                        }else {
                                            currentUndo = goTo(nodeUndo->firstLine - 1, currentUndo, textHead);
                                            tmp = currentUndo->next;

                                            currentUndo->next = nodeUndo->startLine;
                                            textStruct *tmp2 = nodeUndo->startLine;
                                            while (tmp2->next != NULL) {
                                                tmp2 = tmp2->next;
                                            }
                                            tmp2->next = tmp;
                                        }
                                        nodeUndo->startLine = toAddRedo;
                                    }
                                    push(&rootRedo,nodeUndo);
                                    break;
                                case (true):

                                    if (nodeUndo->firstLine < 0) {
                                        nodeUndo->firstLine=-99;
                                        push(&rootRedo,nodeUndo);
                                        break;
                                    }

                                    textStruct *tmp1 = nodeUndo->startLine;
                                    nodeUndo->startLine = textHead;
                                    textHead = tmp1;

                                    push(&rootRedo,nodeUndo);
                                    break;
                            }
                            undoCount--;
                            redoStackHeight++;
                        }
                    } else if (undoCount < 0) { //faccio redo
                        while (undoCount != 0) {
                            nodeUndo = pop(&rootRedo);
                            redoStackHeight--;
                            textStruct* old_version;
                            textStruct* to_add;
                            switch (nodeUndo->cmd){
                                case (false):
                                    if(nodeUndo->linesNum==-1){
                                        textStruct *tmp1 = nodeUndo->startLine;
                                        nodeUndo->startLine = textHead;
                                        textHead = tmp1;
                                        push(&rootUndo,nodeUndo);
                                        undoStackHeight++;
                                        break;
                                    }
                                    old_version = NULL;
                                    currentUndo = goTo(nodeUndo->firstLine, currentUndo, textHead);
                                    for(int i = 1; i <= nodeUndo->linesNum; i++){
                                        textStruct *tmp = nodeUndo->startLine;
                                        nodeUndo->startLine = nodeUndo->startLine->next;
                                        tmp->next = NULL;
                                        to_add = copyNewUndo(&currentUndo, tmp);
                                        if(to_add != NULL){
                                            to_add->next = NULL;
                                            old_version = concatenateText(old_version,to_add);
                                        }
                                        currentUndo=currentUndo->next;
                                    }
                                    nodeUndo->startLine = old_version;
                                    push(&rootUndo,nodeUndo);
                                    undoStackHeight++;

                                    break;
                                case (true):

                                    if(nodeUndo->firstLine < 0){
                                        nodeUndo->firstLine=-99;//nodeU2->firstLine=-99;
                                        push(&rootUndo,nodeUndo);//push(&rootUndo,nodeU2);
                                        undoStackHeight++;
                                        break;
                                    }


                                    textStruct *tmp1 = nodeUndo->startLine;
                                    nodeUndo->startLine = textHead;
                                    textHead = tmp1;

                                    push(&rootUndo,nodeUndo);//push(&rootUndo,nodeU2);
                                    undoStackHeight++;

                                    break;
                            }
                            undoCount++;
                        }
                    }
                }

                struct stackNode *node = newNode(NULL, true, 0, addr1-1);

                textStruct *start = goTo(addr1, current, textHead);
                if (start == NULL) {
                    if (addr1 == 0 && addr2 == 0) {
                        node->firstLine=-99;
                        push(&rootUndo, node);
                        undoStackHeight++;
                    }
                    node->firstLine=-99;
                    push(&rootUndo, node);
                    undoStackHeight++;
                    break;
                }

                //qui salvare
                node->startLine = fullCopy(&textHead);

                textStruct* tmp;
                for (long i = addr1; i <= addr2; i++) {
                    tmp = removeTry(&textHead, addr1);
                }


                //controllo che la delete abbia effettivamente cancellato qualcosa, in caso contrario setto un valore speciale a firstline
                /*if (tmp == NULL) {
                    node->firstLine = -99;
                }*/
                push(&rootUndo, node);
                undoStackHeight++;

                //scarico la pila delle redo
                while(!isEmpty(rootRedo)){
                    rootRedo=NULL;
                    //struct stackNode* toFree = pop(&rootRedo);
                    //free(toFree);
                }
                redoStackHeight=0;

                lastCposition = -1;
                break;
            }
            case 'p':
                if (undoCount != 0) {
                    struct stackNode *nodeUndo = NULL;
                    if (undoCount > 0) { //faccio undo
                        if(undoCount == undoStackHeight){
                            clearText=true;
                            lastUndoCount = undoCount;
                            undoCount=0;
                        }
                        if(undoCount>1000){
                            textCopy=fullCopyNew(&textHead);
                            undoOfCopy=undoCount;
                        }
                        while (undoCount != 0) {
                            nodeUndo = pop(&rootUndo);
                            undoStackHeight--;
                            switch (nodeUndo->cmd) {
                                case (false):
                                    //creo nodo per la redo
                                    if(nodeUndo->linesNum==-1){
                                        textStruct *tmp1 = nodeUndo->startLine;
                                        nodeUndo->startLine = textHead;
                                        textHead = tmp1;
                                        push(&rootRedo,nodeUndo);
                                        break;
                                    }


                                    1+1;

                                    textStruct* toSaveRedo = NULL;
                                    textStruct* toAddRedo;


                                    toAddRedo = removeTrySeq(&textHead, nodeUndo->firstLine,nodeUndo->linesNum);
                                    toSaveRedo= toAddRedo;
                                    for(int i = 1; i<nodeUndo->linesNum; i++){
                                        toSaveRedo= toSaveRedo->next;
                                    }
                                    toSaveRedo->next=NULL;
                                    if (nodeUndo->startLine == NULL){
                                        nodeUndo->startLine = toAddRedo;
                                    }else{
                                        textStruct* tmp;
                                        if(nodeUndo->firstLine==1){
                                            currentUndo = textHead;
                                            textHead=nodeUndo->startLine;
                                            textStruct* tmp2 = nodeUndo->startLine;
                                            while(tmp2->next != NULL){
                                                tmp2 = tmp2->next;
                                            }
                                            tmp2->next = currentUndo;

                                        }else {
                                            currentUndo = goTo(nodeUndo->firstLine - 1, currentUndo, textHead);
                                            tmp = currentUndo->next;

                                            currentUndo->next = nodeUndo->startLine;
                                            textStruct *tmp2 = nodeUndo->startLine;
                                            while (tmp2->next != NULL) {
                                                tmp2 = tmp2->next;
                                            }
                                            tmp2->next = tmp;
                                        }
                                        nodeUndo->startLine = toAddRedo;
                                    }

                                    push(&rootRedo,nodeUndo);
                                    break;
                                case (true):

                                    if (nodeUndo->firstLine < 0) {
                                        nodeUndo->firstLine=-99;
                                        push(&rootRedo,nodeUndo);
                                        break;
                                    }

                                    textStruct *tmp1 = nodeUndo->startLine;
                                    nodeUndo->startLine = textHead;
                                    textHead = tmp1;

                                    push(&rootRedo,nodeUndo);
                                    break;
                            }
                            undoCount--;
                            redoStackHeight++;
                        }
                    } else if (undoCount < 0) { //faccio redo
                        if(undoCount==-undoOfCopy){
                            copyAvailable=true;
                            lastUndoCount=undoCount;
                            undoCount=0;
                        }
                        while (undoCount != 0) {
                            nodeUndo = pop(&rootRedo);
                            redoStackHeight--;
                            textStruct* old_version;
                            textStruct* to_add;
                            switch (nodeUndo->cmd){
                                case (false):
                                    if(nodeUndo->linesNum==-1){
                                        textStruct *tmp1 = nodeUndo->startLine;
                                        nodeUndo->startLine = textHead;
                                        textHead = tmp1;
                                        push(&rootUndo,nodeUndo);
                                        undoStackHeight++;
                                        break;
                                    }
                                    old_version = NULL;
                                    currentUndo = goTo(nodeUndo->firstLine , currentUndo, textHead);

                                    for(int i = 1; i <= nodeUndo->linesNum; i++){
                                        textStruct *tmp = nodeUndo->startLine;
                                        nodeUndo->startLine = nodeUndo->startLine->next;
                                        tmp->next = NULL;
                                        to_add = copyNewUndo(&currentUndo, tmp);
                                        if(to_add != NULL){
                                            to_add->next = NULL;
                                            old_version = concatenateText(old_version,to_add);
                                        }
                                        currentUndo=currentUndo->next;
                                    }
                                    nodeUndo->startLine = old_version;
                                    push(&rootUndo,nodeUndo);
                                    undoStackHeight++;

                                    break;
                                case (true):

                                    if(nodeUndo->firstLine < 0){
                                        nodeUndo->firstLine=-99;
                                        push(&rootUndo,nodeUndo);
                                        undoStackHeight++;
                                        break;
                                    }


                                    textStruct *tmp1 = nodeUndo->startLine;
                                    nodeUndo->startLine = textHead;
                                    textHead = tmp1;


                                    push(&rootUndo,nodeUndo);
                                    undoStackHeight++;

                                    break;
                            }
                            undoCount++;
                        }
                    }
                    //printf("FACCIO UNDO PARI A: %d",undoCount);
                }
                if(clearText==true){
                    for(long i = addr1; i<= addr2; i++){
                        fputs(".\n",stdout);
                    }
                    undoCount=lastUndoCount;
                    clearText=false;
                    break;

                }
                if(copyAvailable==true){
                    copyAvailable=false;
                    undoCount=lastUndoCount;
                    currentPrint = goTo(addr1,currentPrint, textCopy);
                }else {
                    currentPrint = goTo(addr1, currentPrint, textHead);
                }
                for (long i = addr1; i <= addr2; i++) {
                    if (i > maxLine) {
                        for (long j = i; j <= addr2; j++) {
                            fputs(".\n", stdout);

                        }
                        break;
                    } else {


                        if (currentPrint == NULL) {
                            for (long j = i; j <= addr2; j++) {
                                fputs(".\n", stdout);
                            }
                            break;
                        } else {
                            if (currentPrint->line == NULL) {
                                fputs(".\n", stdout);
                            } else {
                                fputs(currentPrint->line, stdout);
                            }
                            currentPrint = currentPrint->next;
                        }
                    }
                }

                break;

            case 'u': {
                if(addr1 <= undoStackHeight-undoCount){
                    undoCount=undoCount+addr1;
                }
                else{
                    undoCount=undoStackHeight;
                }
            }
                break;
            case 'r':
                if(undoCount<=0 && redoStackHeight<=0){
                    break;
                }
                if(addr1 <= redoStackHeight+undoCount){
                    undoCount=undoCount-addr1;
                }
                else{
                    undoCount=-redoStackHeight;
                }
                break;
            default:
                return 0;
        }
    }
}

textStruct* goTo(long addr, textStruct* current, textStruct* head){


    current = head;
    if( addr == 0){
        return NULL;
    }
    for(long i = 1; i < addr; i++){
        if(current != NULL) {
            current = current->next;
        }
    }
    return current;
}

textStruct* copyNew(textStruct** head){
    char tmpLine[256];
    fgets(tmpLine,256,stdin);


    textStruct *tmp;
    textStruct *toReturn = NULL;
    tmp = *head;
    if(tmp->line == NULL){
        //int size = strlen(tmpLine)+2;
        tmp->line = malloc(strlen(tmpLine)+2);
        strcpy(tmp->line,tmpLine);
        //tmp->line=getStringUnlocked();
    } else{
        toReturn = malloc(sizeof(textStruct));
        toReturn->next = NULL;
        toReturn->line = tmp->line;
        char* tmp1 = malloc(strlen(tmpLine)+2);
        strcpy(tmp1,tmpLine);
        tmp->line = tmp1;
    }

    if(tmp->next == NULL) {
        tmp->next = malloc(sizeof(textStruct));
        tmp->next->next = NULL;
        tmp->next->line = NULL;
    }
    return toReturn;
}

textStruct* fullCopy(textStruct** head){
    textStruct* tmp = *head;
    textStruct* copy;
    copy = malloc(sizeof(textStruct));
    textStruct* toReturn = copy;
    while(tmp->next != NULL) {
        copy->line = malloc(strlen(tmp->line) + 2);
        strcpy(copy->line, tmp->line);
        copy->next = malloc(sizeof(textStruct));
        copy=copy->next;
        tmp=tmp->next;
    }
    copy->next=NULL;
    copy->line=NULL;
    return toReturn;
}

textStruct* fullCopyNew(textStruct** head){
    textStruct* headPtr = *head;
    textStruct* new = NULL;
    textStruct** newIt = &new;
    while(headPtr != NULL){
        *newIt = malloc(sizeof(textStruct));
        memcpy(*newIt,&head, sizeof(textStruct));
        (*newIt)->line = headPtr->line;
        headPtr=headPtr->next;
        newIt = &((*newIt)->next);
    }
    return new;
}



textStruct* copyNewUndo(textStruct** head,textStruct* textToCopy) {

    textStruct *tmp;
    textStruct *toReturn = NULL;
    tmp = *head;
    if (tmp->line == NULL) {
        tmp->line = textToCopy->line;
    } else {
        toReturn = malloc(sizeof(textStruct));
        toReturn->next = NULL;
        toReturn->line = tmp->line;

        tmp->line = textToCopy->line;
    }

    if (tmp->next == NULL) {
        tmp->next = malloc(sizeof(textStruct));
        tmp->next->next = NULL;
        tmp->next->line = NULL;
    }
    return toReturn;
}



textStruct* removeTry(textStruct** head, long position){
    if(*head == NULL){
        return NULL;
    }

    textStruct* tmp = *head;

    if(position == 1){
        if(tmp->next == NULL){
            return NULL;
        }
        *head = tmp->next;
        return tmp;
    }

    for(long i=1; tmp!=NULL && i<position-1; i++){
        tmp = tmp->next;
    }

    if(tmp == NULL || tmp->next == NULL){
        return NULL;
    }

    textStruct* next = tmp->next->next;

    textStruct* to_return = tmp->next;

    tmp->next = next;

    return to_return;

}

textStruct* removeTrySeq(textStruct** head, long position,int num){
    if(*head == NULL){
        return NULL;
    }

    textStruct* tmp = *head;
    textStruct* tmp1 = *head;
    if(position == 1){
        if(tmp->next == NULL){
            return NULL;
        }
        for(int i = 1;i<=num; i++){
            tmp=tmp->next;
        }
        *head = tmp;
        return tmp1;
    }

    for(long i=1; tmp!=NULL && i<position-1; i++){
        tmp = tmp->next;
    }

    if(tmp == NULL || tmp->next == NULL){
        return NULL;
    }

    textStruct* next = tmp->next->next;
    for(int i = 1;i<num; i++){
        next = next->next;
    }

    textStruct* to_return = tmp->next;

    tmp->next = next;

    return to_return;

}

textStruct* concatenateText(textStruct* base,textStruct* toAdd){
    textStruct* tmp = base;
    if(tmp == NULL){
        tmp=toAdd;
        return tmp;
    }

    while(tmp->next != NULL){
        tmp=tmp->next;
    }

    tmp->next=toAdd;
    return base;
}





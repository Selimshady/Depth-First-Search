#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 100
#define CITYNAME 30

typedef struct{
    int top;
    int arr[SIZE];
}STACK;

typedef struct tmp{ // To get data from file
    char srcCityName[CITYNAME]; 
    char destCityName[CITYNAME];
    int hour;
    int minute;
    int price;
}TMP;

typedef struct node{ // Graph nodes.
    int mark;
    char cityName[CITYNAME];
    int hour;
    int minute;
    int price;
    struct node *next;
}NODE;

typedef struct graph{ // To save all edges and nodes.
    int numOfCities;
    NODE **adjList;
}GRAPH;

typedef struct resultNode{ // It is for saving found routes.
    int numOfCities; // Length of cities array. All cities on path.
    int *cities; // Saving indices of cities.
    int hour;
    int minute;
    int price;
}RN;

typedef struct resultGraph{ // To print and sort all routes.
    int numOfResults;
    RN  *adjList[SIZE];
}RG;


int findIndex(GRAPH*,char*); // When name is given, this find the index of it in the graph.
int readFileIntoStruct(TMP**); // Read sample.txt file to struct.
GRAPH *createGraph(TMP*,int); // Initialize graph.
void findUniqueCities(GRAPH*,TMP*,int); // 
void printChart(TMP*,int); // Print sample.txt file
NODE *createNode(char[CITYNAME],int,int,int);//Graph nodes
void insertNode(NODE*,GRAPH*,int);// Add new edge to graph for both neighbour.
void printGraph(GRAPH*); // Print neighborhood
void findRoutes(int,int,GRAPH*,STACK*,RG*); // Recursive path finder
RN *createPath(GRAPH*,STACK*,int); // When it reaches to destination, It will save it to RG.
void push(STACK*,int); 
int pop(STACK*);
void printRG(GRAPH*,RG*,int); // Print result graph.

void qSortByPrice(RG*,int,int);
void qSortByTime(RG*,int,int);
int partitionTime(RG*,int,int);
int partitionPrice(RG*,int,int);
void swap(RN*,RN*);

int main(int argc, char **argv)
{
    STACK *stack = (STACK*)malloc(sizeof(STACK)); // Crate a stack and initialize it with 0.
    stack->top = 0;
    
    RG *resultGraph = (RG*)malloc(sizeof(RG));  // Create a result table to save routes in it. 
    resultGraph->numOfResults = 0;
    
    int N,transfer,indexSrc,indexDest;
	TMP *tmpStruct;
    char flag;
    char criter;
    
    char *srcCity = (char*)malloc(CITYNAME*sizeof(char)); 
    char *destCity = (char*)malloc(CITYNAME*sizeof(char));
    
        
    
    N = readFileIntoStruct(&tmpStruct); // Read sample.txt file into struct and return line length.
    printChart(tmpStruct,N); // Print it
    
    GRAPH *graph  = createGraph(tmpStruct,N); // Create graph for saving edges.
    printGraph(graph); // Print it
    
    do{
        printf("Ucus Bilgilerini Giriniz(Kalkis-Hedef-Aktarma Sayisi(En Fazla): ");
        scanf("%s %s %d",srcCity,destCity,&transfer); 
            
        indexSrc = findIndex(graph,srcCity);
        indexDest = findIndex(graph,destCity);
    
        findRoutes(indexSrc,indexDest,graph,stack,resultGraph);
        printf("Onceliginiz (s)ure mi / (m)aliyet mi: ");
        scanf(" %c",&criter);
        if(criter == 's') {
            qSortByTime(resultGraph,0,resultGraph->numOfResults-1);
        }
        else if (criter == 'm') {
            qSortByPrice(resultGraph,0,resultGraph->numOfResults-1);
        }
        printRG(graph,resultGraph,transfer);
        
        printf("Baska yolculuk aramak ister misiniz? (e)vet / (h)ayir: ");
        scanf(" %c",&flag);
    }while(flag == 'e');

    
    free(resultGraph);
    free(destCity);
    free(srcCity);
    free(tmpStruct);
    free(stack);
    free(graph);
	return 0;
}

void qSortByTime(RG *resultGraph,int l, int r) {
    if(l<r) {
        int pi = partitionTime(resultGraph, l, r);
        qSortByTime(resultGraph, l, pi - 1);
        qSortByTime(resultGraph, pi + 1, r);
    }
}


int partitionTime(RG *resultGraph, int low, int high) {
  
  // select the rightmost element as pivot
  int pivot = 60*(resultGraph->adjList[high]->hour) + resultGraph->adjList[high]->minute ;
  int j;
  // pointer for greater element
  int i = (low - 1);

  
  // compare them with the pivot
  for (j = low; j < high; j++) {
    if (60*(resultGraph->adjList[j]->hour) + resultGraph->adjList[j]->minute <= pivot) {
        
      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;
      
      // swap element at i with element at j
      swap(resultGraph->adjList[i], resultGraph->adjList[j]);
    }
  }

  // swap the pivot element with the greater element at i
  swap(resultGraph->adjList[i + 1],resultGraph->adjList[high]);
  
  // return the partition point
  return (i + 1);
}

void swap(RN *a, RN *b) {
  RN t = *a;
  *a = *b;
  *b = t;
}

void qSortByPrice(RG *resultGraph,int l, int r) {
    if(l<r) {
        int pi = partitionPrice(resultGraph, l, r);
        qSortByPrice(resultGraph, l, pi - 1);
        qSortByPrice(resultGraph, pi + 1, r);
    }
}

int partitionPrice(RG *resultGraph, int low, int high) {
  
  // select the rightmost element as pivot
  int pivot = resultGraph->adjList[high]->price ;
  int j;
  // pointer for greater element
  int i = (low - 1);

  
  // compare them with the pivot
  for (j = low; j < high; j++) {
    if (resultGraph->adjList[j]->price <= pivot) {
        
      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;
      
      // swap element at i with element at j
      swap(resultGraph->adjList[i], resultGraph->adjList[j]);
    }
  }

  // swap the pivot element with the greater element at i
  swap(resultGraph->adjList[i + 1],resultGraph->adjList[high]);
  
  // return the partition point
  return (i + 1);
}

void printRG(GRAPH *g,RG *rg,int transfer) {
    int i,j,flag = 0;
    FILE *fp;
    printf("\n");
    if(rg->numOfResults) {
        fp = fopen("result.txt","w"); // En son aratılan file dosyaya yazılır.
        for(i=0;i<rg->numOfResults;i++) {
            if(rg->adjList[i]->numOfCities-2 <= transfer) {
                fprintf(fp,"%s %s ",g->adjList[rg->adjList[i]->cities[0]]->cityName,g->adjList[rg->adjList[i]->cities[rg->adjList[i]->numOfCities-1]]->cityName);
                printf("%s %s ",g->adjList[rg->adjList[i]->cities[0]]->cityName,g->adjList[rg->adjList[i]->cities[rg->adjList[i]->numOfCities-1]]->cityName);
                if(rg->adjList[i]->numOfCities > 2) {
                    for(j=1;j<rg->adjList[i]->numOfCities-1;j++) {
                        fprintf(fp,"%s ",g->adjList[rg->adjList[i]->cities[j]]->cityName);
                        printf("%s ",g->adjList[rg->adjList[i]->cities[j]]->cityName);
                    }
                }
                else {
                    fprintf(fp,"%s ","NONE");
                    printf("%s ","NONE");
                }
                fprintf(fp,"%d %d %d\n",rg->adjList[i]->hour,rg->adjList[i]->minute,rg->adjList[i]->price);
                printf("%d %d %d\n",rg->adjList[i]->hour,rg->adjList[i]->minute,rg->adjList[i]->price);
                flag = 1;
            }
        }
    }
    else
        printf("Boyle bir yolculuk bulunamadi.");
    fclose(fp);
    if(!flag) {
            printf("Aktarmasiz yolculuk bulunamamistir.");
            remove("result.txt"); // Oyle bir yolculuk yok ise file silinir.
    }
    printf("\n");
    rg->numOfResults = 0;
}

void push(STACK *stack, int x) {
    stack->arr[stack->top]=x;
    stack->top++;
}

int pop(STACK *stack) {
    stack->top--;
    return stack->arr[stack->top];
}


RN *createPath(GRAPH *graph,STACK *stack,int destIndex) {
    RN *resultNode = (RN*)malloc(sizeof(RN)); // It is for adding into result graph.
    resultNode->cities = (int*)malloc(sizeof(int)*stack->top+1); // Length of stops. And I will add them destCity.
    
    STACK tmpStack = *stack; // We will use it to find stops but shouldn't change it at all.
    int i;
    NODE *tmpNode,*prevNode; //Checking edges and weights.
    int price = 0,minute = 0,hour = 0;
    
    
    resultNode->numOfCities = stack->top+1;
    resultNode->cities[stack->top] = destIndex;
    for(i=stack->top-1;i>=0;i--)
        resultNode->cities[i] = pop(stack);
    *stack = tmpStack;

    if(stack->top == 1) { // If there is direct link between src and dest city.
        tmpNode = graph->adjList[resultNode->cities[0]]->next;
        while(strcmp(tmpNode->cityName,graph->adjList[destIndex]->cityName))
            tmpNode = tmpNode->next;
        minute = tmpNode->minute;
        hour = tmpNode->hour;
        price = tmpNode->price;
    }
    else {
        prevNode = graph->adjList[resultNode->cities[0]]->next; // For all neighbours we should check their edges. So We need graph info.
        for(i=1;i<stack->top;i++) { 
            tmpNode = graph->adjList[resultNode->cities[i]];
            while(strcmp(tmpNode->cityName,prevNode->cityName))
                prevNode = prevNode->next;
            minute = prevNode->minute + minute;
            price+=prevNode->price;
            hour= 1 + hour + prevNode->hour+ minute/60; // Add one more hour per stop
            minute= minute%60;
            prevNode = tmpNode->next;
        }
        while(strcmp(graph->adjList[destIndex]->cityName,prevNode->cityName)) // And the destCity is not in stack so we'll check it by destIndex.
            prevNode = prevNode->next;
        minute = prevNode->minute + minute;
        price+=prevNode->price;
        hour=hour + prevNode->hour+minute/60;
        minute= minute%60;
    }    
    resultNode->minute = minute;
    resultNode->price = price;
    resultNode->hour = hour;
    return resultNode;
}

int findIndex(GRAPH *g,char *cityName) {
    int i = 0;
    while(strcmp(g->adjList[i]->cityName,cityName))
        i++;
    return i;
}


void findRoutes(int srcIndex,int destIndex,GRAPH *graph,STACK *stack,RG *resultGraph) {
    int nSrcIndex=0; //The new edge for diving.
    
    NODE *tmpNode; //Checking neighbourhood.
    
    graph->adjList[srcIndex]->mark = 1; // Mark as visited.
    push(stack,srcIndex); // Push it to stack for checking it later if necessary.
    
    tmpNode = graph->adjList[srcIndex]->next; // First neighbour.
    while(tmpNode) {
        if(strcmp(tmpNode->cityName,graph->adjList[destIndex]->cityName)) { // If the neighbour is not destination enter.
            nSrcIndex = findIndex(graph,tmpNode->cityName); // Find index of neighbour in graph.
            if(!graph->adjList[nSrcIndex]->mark) //If not visited 
                findRoutes(nSrcIndex,destIndex,graph,stack,resultGraph); // Jump // Recursive
        }
        else {
            resultGraph->adjList[resultGraph->numOfResults]=createPath(graph,stack,destIndex); //If neighbour is destination create a path.
            resultGraph->numOfResults++; //Increase the result table.
        }
        tmpNode = tmpNode->next; // Check for other neighbours and paths.
    }
    nSrcIndex=pop(stack); // We don't use it but have to pop because we are retreating.
    graph->adjList[nSrcIndex]->mark = 0; // And delete mark for other can use it again.
}

void printGraph(GRAPH* graph) {
    int i;
    NODE *temp = NULL;
    printf("\n");
    
    for(i=0;i<graph->numOfCities;i++) {
        printf("%s:->", graph->adjList[i]->cityName);
        temp = graph->adjList[i]->next;
            while(temp) {
            printf("%s -> ", temp->cityName);
            temp = temp->next;
        } 
        printf("\n");
    }
}

NODE* createNode(char *cityName,int hour,int minute,int price) {
        NODE* tmpNode = (NODE*)malloc(sizeof(NODE));
        tmpNode->mark = 0;
        strcpy(tmpNode->cityName,cityName);
        tmpNode->hour =hour;
        tmpNode->minute=minute;
        tmpNode->price=price;
        tmpNode->next=NULL;
        return tmpNode;
}

void printChart(TMP *tmpStruct,int N) {
    int i;
    printf("-----------------------------------------------------------------------\n");
    printf("| %11s | %11s | %11s | %11s | %11s |\n","Source","Destination","Hour","Minute","Price");
    printf("----------------------------------------------------------------------\n");
    for(i=0;i<N;i++) {
        printf("| %11s | %11s | %11d | %11d | %11d |\n",tmpStruct[i].srcCityName,tmpStruct[i].destCityName,tmpStruct[i].hour,tmpStruct[i].minute,tmpStruct[i].price);
        printf("-----------------------------------------------------------------------\n");
    }
    printf("\n\n");
}

void findUniqueCities(GRAPH* graph,TMP *tmpStruct,int N) {
    char **cities;
    int i,j,size=0;
    
    cities = (char**)malloc(sizeof(char*)*N);
    for(i=0;i<N;i++)
        cities[i] = (char*)malloc(sizeof(char)*CITYNAME);
    
    
    for(i=0;i<N;i++) {
        j=size;
        while(j>0 && strcmp(tmpStruct[i].srcCityName,cities[j-1])) 
            j--;
        if(!j) {
            strcpy(cities[size],tmpStruct[i].srcCityName); // If there is no city named same, put it array and increase size.(src)
            size++;
        }
        j=size;
        while(j>0 && strcmp(tmpStruct[i].destCityName,cities[j-1]))
            j--;
        if(!j) {
            strcpy(cities[size],tmpStruct[i].destCityName);// If there is no city named same, put it array and increase size.(dest)
            size++;
        }
    }
    
    graph->numOfCities = size;
    graph->adjList = (NODE**)malloc(sizeof(NODE*)*size);
    
    
    for(i=0;i<size;i++) 
        graph->adjList[i] = createNode(cities[i],0,0,0);
    
    
    for(i=0;i<N;i++)
        free(cities[i]);
    free(cities);
}

void insertNode(NODE *node,GRAPH *graph,int index) {
    if(graph->adjList[index]->next == NULL) // Insert the edge at the beginning if there is any edges before.
        graph->adjList[index]->next = node;
    else {
        node->next = graph->adjList[index]->next;
        graph->adjList[index]->next = node;
    }
}

GRAPH *createGraph(TMP* tmpStruct,int N) {
    int i,j;
    GRAPH *graph = (GRAPH*)malloc(sizeof(GRAPH));
    NODE *tmpNode;
    
    findUniqueCities(graph,tmpStruct,N); //N length of file. It checks all rows.
    for(i=0;i<N;i++) {
        j=0;
        
        while(strcmp(tmpStruct[i].srcCityName,graph->adjList[j]->cityName) && strcmp(tmpStruct[i].destCityName,graph->adjList[j]->cityName))  // Check both cities at the same time.
            j++;
            
        if(strcmp(tmpStruct[i].srcCityName,graph->adjList[j]->cityName)) { //If dest city name found first enter.
            
            tmpNode = createNode(tmpStruct[i].srcCityName,tmpStruct[i].hour,tmpStruct[i].minute,tmpStruct[i].price);
            insertNode(tmpNode,graph,j); // Add the edge for dest city.
            j++;

            while(strcmp(tmpStruct[i].srcCityName,graph->adjList[j]->cityName))
                j++;
                
            tmpNode = createNode(tmpStruct[i].destCityName,tmpStruct[i].hour,tmpStruct[i].minute,tmpStruct[i].price); 
            insertNode(tmpNode,graph,j); // Add the edge for src city.
        }
        else { // If src cit name found first enter.
            
            tmpNode = createNode(tmpStruct[i].destCityName,tmpStruct[i].hour,tmpStruct[i].minute,tmpStruct[i].price);
            insertNode(tmpNode,graph,j); // Add the edge for src city.
            j++;

            while(strcmp(tmpStruct[i].destCityName,graph->adjList[j]->cityName))
                j++;
                
            tmpNode = createNode(tmpStruct[i].srcCityName,tmpStruct[i].hour,tmpStruct[i].minute,tmpStruct[i].price);
            insertNode(tmpNode,graph,j); // Add the edge for dest city.
        }
    }
    return graph;
}

int readFileIntoStruct(TMP **ptrStruct) {
    
    FILE *fp; // To keep file pointer
    char *line = (char*)malloc(sizeof(char)*SIZE);//To get the line and values//Allocating memory
    TMP tmp[SIZE];
    int size = 0,i;
    TMP *tmpStruct; // Temporary struct
 
    if((fp = fopen("sample.txt","r")) == NULL) {//Opening the file to read
        printf("FILE ACCSESS ERROR!");
        exit(-1);//If there is a problem, exit the program
    } 
    
    while(fgets(line,100,fp) != NULL) {//Read the line
        sscanf(line, "%s %s %d %d %d" ,tmp[size].srcCityName,tmp[size].destCityName,&tmp[size].hour,&tmp[size].minute,&tmp[size].price); // Parsing three values to struct
        size++; //Counts the lines
    }
    
    tmpStruct = (TMP*)malloc(sizeof(TMP)*size); // Allocating the real struct
    
    for(i=0;i<size;i++) {
        tmpStruct[i] = tmp[i];
    }
    
    *ptrStruct = tmpStruct; // To pass address to main struct
    
    fclose(fp); //close the file
    free(line); // free the line address
    return size; //Return the length
}
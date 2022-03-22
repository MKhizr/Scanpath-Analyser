#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//structure to store data related to AOIs from file
typedef struct AOI {

    char Name;
    int TopLeftX;
    int Width;
    int TopLeftY;
    int Height;

}AOIarray;


//structure to store data related to fixations from file
typedef struct Fixations{

    int FixationID;
    int Xcoordinate;
    int Ycoordinate;
    int Duration;

}FixationArray;

//structure for head of graph
struct GraphHead {

    int count;
    struct GraphVertex *first;

};

//structure for vertices of graph
struct GraphVertex{

    struct GraphVertex *next;
    int ID;
    char scanpath[100];
    int InDegree;
    int OutDegree;
    int processed;
    struct GraphArc *firstArc;

};

//structure for edges of graph
struct GraphArc{

    struct GraphVertex*destination;
    int weight;
    struct GraphArc *nextArc;

};

//creates the head of the graph and initialises
struct GraphHead *CreateGraph(void) {

    struct GraphHead *Head = (struct GraphHead *) malloc(sizeof(struct GraphHead));
    Head->count=0;
    Head->first = NULL;
    return Head;

};

//inserts vertex in the graph
void InsertVertex(struct GraphHead *Head,char *path,int IDcounter) {

    //first allocates memory for a vertex then initalises it with some default values
    struct GraphVertex *vertex=(struct GraphVertex *)malloc(sizeof(struct GraphVertex));

    vertex->next=NULL;
    strcpy(vertex->scanpath,path);
    vertex->InDegree=0;
    vertex->OutDegree=0;
    vertex->processed=0;
    vertex->ID=IDcounter;
    vertex->firstArc=NULL;

    Head->count++;

    if(Head->first == NULL) {
        Head->first=vertex;
    }

    else {
        struct GraphVertex *temp= Head->first;

        while(temp->next != NULL)
            temp = temp->next;
        temp->next = vertex;
    }

}


struct GraphHead* createVertices(char *AOIname, char* ScanPathName) {

    char ch,space,ch2;
    int count=0,i,q,count2=0,j,k,counter=0,a,b,y,z,IDcounter=1;

    //opens the file to read the number of lines in AOIs txt file and the closes it
    FILE *infile;
    infile = fopen(AOIname, "r");

    while ( (ch= fgetc(infile)) != EOF ) {
        if (ch == '\n') {
            count=count+1;
        }
    }
    fclose(infile);


    //opens the file this time to read the data from the AOI file into the structure
    FILE *infile3;
    infile3 = fopen(AOIname, "r");

    AOIarray* temp = (AOIarray*) malloc(count * sizeof(AOIarray));

    //initialising
    for(q=0;q<count;q++) {

        temp[q].Name = NULL;
        temp[q].TopLeftX=0;
        temp[q].Width=0;
        temp[q].TopLeftY=0;
        temp[q].Height=0;
    }

    //storing data in structure of AOI
    for(i=0;i<count;i++) {

        fscanf(infile3, "%s", &temp[i].Name);
        fscanf(infile3, "%d", &temp[i].TopLeftX);
        fscanf(infile3, "%d", &temp[i].Width);
        fscanf(infile3, "%d", &temp[i].TopLeftY);
        fscanf(infile3, "%d", &temp[i].Height);

    }

    fclose(infile3);

    //creates an empty graph
    struct GraphHead *GRAPH= CreateGraph();

    //opens the file to read and calculate number of lines
    FILE *infile2;
    infile2 = fopen(ScanPathName, "r");

    while ( (ch2= fgetc(infile2)) != EOF ) {
        if (ch2 == '\n') {
            count2=count2+1;
        }
    }
    count2++;
    fclose(infile2);


    //opens the file this time to read the data from the fixations file into the structure
    FILE *infile4;
    infile4 = fopen(ScanPathName, "r");

    FixationArray* FixTemp = (FixationArray*) malloc(count2 * sizeof(FixationArray));

    //initialising
    for(a=0;a<count2;a++) {

        FixTemp[a].FixationID=0;
        FixTemp[a].Xcoordinate=0;
        FixTemp[a].Ycoordinate=0;
        FixTemp[a].Duration=0;

    }

    for(b=0;b<count2;b++) {

        fscanf(infile4, "%d", &FixTemp[b].FixationID);
        fscanf(infile4, "%d", &FixTemp[b].Xcoordinate);
        fscanf(infile4, "%d", &FixTemp[b].Ycoordinate);
        fscanf(infile4, "%d", &FixTemp[b].Duration);

    }

    fclose(infile4);


    char *scanpath = (char *) malloc(sizeof(char)*100);

    if(scanpath==NULL)
        exit(1);

     //scanpath for people is being created here
    for(j=0; j<count2; j++)
    {

        if(FixTemp[j].FixationID==0 && j!=0 )
        {
            InsertVertex(GRAPH, scanpath,IDcounter);
            free(scanpath);
            scanpath = (char *) malloc(sizeof(char)*100);
            counter = 0;
            IDcounter++;
        }

        //checks the necessary conditions to assign a character for string
        for(k=0; k<count; k++)
        {
            if ( (FixTemp[j].Xcoordinate >= temp[k].TopLeftX) && (FixTemp[j].Xcoordinate <= (temp[k].TopLeftX + temp[k].Width) ) &&
                (FixTemp[j].Ycoordinate >= temp[k].TopLeftY) && (FixTemp[j].Ycoordinate <= (temp[k].TopLeftY + temp[k].Height) ) )
            {
                scanpath[counter] = temp[k].Name;
                counter++;
                break;
            }
        }
        scanpath[counter] = NULL;

    }
    InsertVertex(GRAPH, scanpath,IDcounter);
    free(scanpath);

    return GRAPH;
}


//inserts edge to graph
int insertArc(struct GraphHead *Head,int fromKey,int toKey, int lcs) {

    struct GraphArc *arc= (struct GraphArc *)malloc(sizeof(struct GraphArc));
    struct GraphVertex *fromVertex = NULL;
    struct GraphVertex *toVertex = NULL;

    fromVertex = Head->first;

    while(fromVertex != NULL && fromVertex->ID != fromKey)
        fromVertex = fromVertex->next;

    if(fromVertex == NULL)
        return -2;

    toVertex=Head->first;

    while(toVertex != NULL && toVertex->ID != toKey)
        toVertex = toVertex->next;

    if(toVertex == NULL)
        return -3;

    fromVertex->OutDegree++;
    toVertex->InDegree++;

    //value of lcs which is the weight in this case is being assigned to the edge
    arc->destination=toVertex;
    arc->weight=lcs;
    arc->nextArc = NULL;

    if(fromVertex->firstArc == NULL)
        fromVertex->firstArc = arc;

    else{
        struct GraphArc *tempArc = fromVertex->firstArc;
        while(tempArc->nextArc != NULL) {
            tempArc= tempArc->nextArc;
        }
            tempArc->nextArc=arc;

    }

    return 1;
}




struct GraphHead* createEdges(struct GraphHead* Head) {

    int s,m,n,j,k,finalans,result1,result2;

    struct GraphVertex *tempvertex1 = NULL;
    struct GraphVertex *tempvertex2;

    tempvertex1 = Head->first;

    //need to keep one pointer static to calculate lcs with that particular node and rest of the nodes
    while (tempvertex1 != NULL) {
        tempvertex2= tempvertex1->next;

        while(tempvertex2 != NULL) {

            m=strlen(tempvertex1->scanpath);
            n=strlen(tempvertex2->scanpath);

            //lcs calculation being done below
            int C[m+1][n+1];

            for(j=1;j<=m;j++){
                C[j][0]=0;
            }

            for(k=0;k<=n;k++) {
                C[0][k]=0;
            }


            for(j=1;j<=m;j++) {

                for(k=1;k<=n;k++){

                    if( (tempvertex1->scanpath[j-1]) == (tempvertex2->scanpath[k-1]) ){
                        C[j][k]=C[j-1][k-1] + 1;
                    }
                    else if (C[j-1][k] >= C[j][k-1]) {
                        C[j][k] = C[j-1][k];
                    }
                    else {
                        C[j][k]=C[j][k-1];
                    }
                }
            }

            finalans=C[m][n];

            if(finalans>=5) {
                    //undirected graph hence edge being inserted twice
                    result1=insertArc(Head,tempvertex1->ID,tempvertex2->ID,finalans);
                    result2=insertArc(Head,tempvertex2->ID,tempvertex1->ID,finalans);
            }

            tempvertex2=tempvertex2->next;
        }
        tempvertex1=tempvertex1->next;
    }


return Head;
}

void display(struct GraphHead *Head){

    int max=0,min=100;
    int maxtempID,mintempID;

    struct GraphVertex *tempvertex = NULL;
    struct GraphArc *tempArc = NULL;

    tempvertex = Head->first;

    while(tempvertex != NULL) {

        printf("\n%d",tempvertex->ID);
        tempArc=tempvertex->firstArc;

        while(tempArc != NULL){
                printf(" -> %d | %d", tempArc->destination->ID, tempArc->weight);
                tempArc=tempArc->nextArc;
        }

        //calculates man number of neighbors
        if(tempvertex->OutDegree > max) {
            max = tempvertex->OutDegree;
            maxtempID = tempvertex->ID;
        }

        //calculates min number of neighbors
        if(tempvertex->OutDegree < min) {
            min = tempvertex->OutDegree;
            mintempID = tempvertex->ID;
        }

        tempvertex = tempvertex->next;
    }

    //displays max and min neighbors
    printf("\n\nVertex with maximum number of neighbors: %d has %d neighbors \n",maxtempID,max);
    printf("Vertex with minimum number of neighbors: %d has %d neighbors \n",mintempID,min);


}

int main () {

    char AOIname[50];
    char ScanPathName[50];

    printf("Enter AOIs file name > ");
    scanf(" %s",AOIname);

    printf("\n");

    printf("Enter scanpaths file name > ");
    scanf(" %s",ScanPathName);

    struct GraphHead *FINALGRAPH=createVertices(AOIname,ScanPathName);

    struct GraphHead *NEWFINALGRAPH=createEdges(FINALGRAPH);

    display(NEWFINALGRAPH);

    return 0;

}

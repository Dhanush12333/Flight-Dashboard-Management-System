#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum{SUCCESS,FAILURE} status_code;
typedef struct time{
    int hr;
    int min;
}TIME;

//Taking Doubly LL for flight plans
typedef struct plan{
    int flightid;
    TIME dep;
    TIME eta;
    struct plan* next;
    struct plan* prev;
}flightplan;

//Circular LL for Buckets
typedef struct buck{
    int buckid;
    TIME etabeg;
    TIME etaend;
    struct buck* next;
    flightplan* fp;
}Bucket;

Bucket* CreateBuck(){
    Bucket* bptr;
    bptr=(Bucket*)malloc(sizeof(Bucket));
    bptr->next=NULL;
    bptr->fp=NULL;
    return bptr;
}

Bucket* DeleteBucket(Bucket* bptr) {
    Bucket* nptr;
    //AS we identified the Bucket it can't be Null
    if (bptr == NULL) {
        // List is empty, nothing to delete
        printf("No Buckets in the Dashboard to delete\n");
    }

    nptr = bptr;

    if (nptr->next == nptr) {  // Single Bucket
        free(bptr);
        bptr=NULL;
        // Updated list is empty
        printf("All Buckets are deleted No Bucket to Display\n");
    } 
    else 
    {
        while (nptr->next != bptr) {
            nptr = nptr->next;
        }
        nptr->next = bptr->next;
        free(bptr);
        bptr=nptr;
        // Updated list with the next bucket
    }
    return bptr;
}

flightplan* CreateFlightPlan()
{
    flightplan* fptr;
    fptr=(flightplan*)malloc(sizeof(flightplan));
    fptr->next=NULL;
    fptr->prev=NULL;
    if(fptr==NULL){
        printf("Memory Allocation Error");
    }
    return fptr;
}

Bucket* CL_InsertBuckAtEnd(Bucket* list_ptr,Bucket* nptr){//nptr is node to be inserted
   Bucket *last;
   last=list_ptr;
   status_code sc=SUCCESS;
   if(last==NULL){
    last=nptr;
    nptr->next=nptr;
   }
   else{
    nptr->next=last->next;
    last->next=nptr;
    last=nptr;
   }
   
   return last;
}

flightplan* InsertFPAtEnd(flightplan* nptr,flightplan* lptr){
    
    if(lptr==NULL)//Single Node
    {
        lptr=nptr;
        lptr->prev=NULL;
        lptr->next=NULL;
       
    }
    else
    {
        flightplan* fptr;
        fptr=lptr;
        while(fptr->next!=NULL){
            fptr=fptr->next;
        }
        fptr->next=nptr;
        nptr->prev=fptr;
        
    }
    return lptr;
    
}

flightplan* AddFlightPlan(flightplan* fatr, long** filePosition) {
    FILE* fptr;
    int fID, etahr, etamin, dephr, depmin;
    flightplan* flp;

    // Open the file in read mode
    fptr = fopen("ATC.txt", "r");
    if (fptr == NULL) {
        printf("Error in opening file");
        fclose(fptr);
        return fatr; // Return fatr as it is, indicating an error
    }

    // If filePosition is not NULL, seek to the saved file position
    if (*filePosition != NULL && **filePosition != -1) {
        fseek(fptr, **filePosition, SEEK_SET);
    }
    char line[100];
    while (fgets(line, sizeof(line), fptr) != NULL && line[0] != '\n') 
    {
        sscanf(line, "%d %d:%d %d:%d", &fID, &etahr,&etamin,&dephr,&depmin);
        flp = CreateFlightPlan();
        flp->flightid = fID;
        flp->eta.hr = etahr;
        flp->eta.min = etamin;
        flp->dep.hr = dephr;
        flp->dep.min = depmin;
        // Save the current file position
        

        fatr = InsertFPAtEnd(flp, fatr);
    }
    **filePosition = ftell(fptr);
    
    // Close the file
    fclose(fptr);

    return fatr;
}

status_code CreateCL(int n,Bucket** lpptr,TIME t1,long* Filepn){
    Bucket *last,*cptr;
    last=*lpptr;
    int a;
    status_code sc = SUCCESS;
    for(int i=0;(i<n)&&(sc==SUCCESS);i++){
        cptr=CreateBuck();
        if (cptr == NULL) {
            sc=FAILURE; // Memory allocation failure
        }
        cptr->buckid=i+1;
        cptr->etabeg=t1;
        t1.min+=59;
        if(t1.min>59){
            t1.hr+=1;
            if(t1.hr>23){
                t1.hr=0;
            }
            t1.min=((t1.min)%60);
        }
        cptr->etaend=t1;
        cptr->fp=AddFlightPlan(cptr->fp,&Filepn);
        last = CL_InsertBuckAtEnd(last,cptr);
        t1.min+=1;
        if(t1.min>59){
            t1.hr+=1;
            if(t1.hr>23){
                t1.hr=0;
            }
            t1.min=((t1.min)%60);
        }
        
    }
    *lpptr=last->next;
    
    return sc;

}

void printFlightDetails(flightplan* rptr)
{
    printf("Flight ID : %d",rptr->flightid);
    printf(" Expected Time of Arrival %d :",rptr->eta.hr);
    printf(" %d ",rptr->eta.min);
    printf(" Expected Time of Departure %d : ",rptr->dep.hr);
    printf(" %d \n",rptr->dep.min);
}

void DisplayFlightPlan(flightplan* aptr)
{
    if(aptr==NULL){
        printf("No Flight plan for Corresponding Above Bucket\n");
        return;
    }
    
    flightplan* rptr;
    rptr=aptr;
    while(rptr!=NULL)
    {
        printFlightDetails(rptr);
        rptr=rptr->next;
    }

}

void PrintBuckDetails(Bucket* aptr)
{
    printf("      Bucket number %d \n\n",aptr->buckid);
    printf("ETA Interval %d :",aptr->etabeg.hr);
    printf(" %d - ",aptr->etabeg.min);
    printf("%d :",aptr->etaend.hr);
    printf(" %d \n",aptr->etaend.min);
    printf("Flight Plan : \n");
    DisplayFlightPlan(aptr->fp);
}

void DisplayInfo(Bucket* bptr)
{
    if(bptr==NULL)
    {
        printf("The dashboard is empty\n");
        return;
    }
    Bucket* aptr;
    aptr=bptr;
    PrintBuckDetails(aptr);
    Bucket* rptr;
    rptr=aptr;
    aptr=aptr->next;
    while(aptr!=rptr)
    {
        PrintBuckDetails(aptr);
        aptr=aptr->next;
    }
    printf("\n");
}

status_code InsertFPBefore(Bucket** dptr,flightplan** q,flightplan* p)
{
    status_code sc=SUCCESS;
    flightplan* x;
    x=*q;
    if(p==NULL||x==NULL)
    {
        sc=FAILURE;
    }
    else
    {
    if(x->prev==NULL)
    {
        /*When The Prev is NULL we need to insert at start
        Eventhough the insertion is succesful The Bucket points to the Node before Insertion 
        only So we need to change the Node that is pointed by bucket also*/
        Bucket* kptr;
        kptr=*dptr;
        p->prev = NULL;
        x->prev = p;
        p->next = x;
        *q = p;
        kptr->fp=p;
            
    }
    else
    {   
        /*We don't need to change bucket's flight plan head
        as we are Inserting at Middle */
        flightplan* prev_node;
        prev_node=x->prev;
        p->next=x;
        x->prev=p;
        prev_node->next=p;
        p->prev=prev_node;
        
    }
    }
    return sc;

}

// Definition of the timediff function
int timediff(TIME A, TIME B) {
    return (((A.hr - B.hr) * 60) + (A.min - B.min));
}

int maxtime(TIME A, TIME B) {
    if (A.hr == B.hr)
     {
        if (A.min == B.min)
         {
            return 0;  // A and B are equal
        } 
        else if (A.min < B.min) 
        {
            return 1;  // A precedes B
        } 
        else 
        {
            return -1;  // A succeeds B
        }
    } 
    else if (A.hr < B.hr) 
    {
        return 1;  // A precedes B
    } 
    else 
    {
        return -1;  // A succeeds B
    }
}

Bucket* InsertFP(Bucket* dptr,flightplan* nptr)
{
    flightplan *dummy;
    dummy=dptr->fp;
    int flag=0;
    while(flag==0)
    {   
    if((maxtime(nptr->dep,dummy->dep)==1)||(maxtime(nptr->dep,dummy->dep)==0))
    {
        status_code sc;
        sc=InsertFPBefore(&dptr,&dummy,nptr);
        if(sc==FAILURE)
        {
            printf("Error while insertion");
        }
        else
        {
            printf("Insertion Before One flight Done Succesfully");
        }
        flag=1;
    }
    else
    {
        if(dummy->next!=NULL)
        {
        dummy=dummy->next;
        }
        else
        {
            dummy=InsertFPAtEnd(nptr,dummy);
            flag=1;
        }
        
    }
    }
    return dptr;
}

status_code InsertNewFlightPlan(Bucket** bptr)
{
    flightplan* nptr;
    nptr=CreateFlightPlan();
    printf("Enter the Flight ID : ");
    scanf("%d",&(nptr->flightid));
    printf("Enter the Expected Time of Arrival of the Flight\n");
    scanf("%d:%d",&(nptr->eta.hr),&(nptr->eta.min));
    printf("Enter the departure time of the Flight \n");
    scanf("%d:%d",&(nptr->dep.hr),&(nptr->dep.min));
    Bucket* dptr;
    dptr=*bptr;
    int flag=0;
    status_code sc=FAILURE;
    while(flag==0){
        if((timediff(nptr->eta,dptr->etabeg)<60)&&(maxtime(nptr->eta,dptr->etabeg)!=1))
        {
            
           dptr=InsertFP(dptr,nptr);
           flag=1;
           sc=SUCCESS;
        }
        else
        {
            dptr=dptr->next;
        }
        
    }
    return sc;
}

Bucket* RemoveFlightPlan(Bucket* dptr,flightplan** fptr)
{
    flightplan *prev_flight,*next_flight;
    flightplan* dum;
    dum=*fptr;
    prev_flight=dum->prev;
    next_flight=dum->next;
    //dum can't be Null as it is the flight we identified
    if(prev_flight!=NULL)
    {
        prev_flight->next=next_flight;
        if(next_flight!=NULL)
        {
            next_flight->prev=prev_flight;
        }
        
    }
    else
    {
        *fptr=next_flight;
        dptr->fp=*fptr;//As dptr->fp always points to 1st flight plan and as if prev_flight is null we are deleting the 1st flight plan itself!
        if(next_flight!=NULL)
        {
            next_flight->prev=prev_flight;
        }
        
        else//Both previous and next flights are Null=>InActive Bucket
        {

            
            dptr=DeleteBucket(dptr);
                                    /*As Next flight is Null and prev Flight is also Null => we need to delete Single Node As 
                                    Bucket is Inactive we need to remove Bucket Also*/
        }
        
    }
    free(dum);

    
    
    return dptr;
}

status_code CancelFlightPlan(Bucket** bptr)
{
    int Flightid;
    printf("Enter Flight Number you want to remove\n");
    scanf("%d",&Flightid);
    int flag1,flag2;
    flag1=flag2=0;
    Bucket *dptr;
    dptr=*bptr;
    flightplan* dummy;
    dummy=dptr->fp;
    int count=1;
    status_code sc=FAILURE;
    while((flag1==0)&&(count<25))
    {
        flag2=0;
        while(flag2==0)
        {   
            if(dummy->flightid==Flightid)
            {
                
                dptr=RemoveFlightPlan(dptr,&dummy);
                sc=SUCCESS;
                flag1=1;
                flag2=1;
            }
            else if(dummy->next==NULL)
            {
                flag2=1;
            }
            if(flag2==0)
            //Since we don't want to execute this statement after flag2 becomes 1 and we directly wanna exit from loop
            {
            dummy=dummy->next;
            }
        }

        dptr=dptr->next;
        dummy=dptr->fp;
        count++;
    }
    return sc;
}

void ShowFlightStatus(Bucket* bptr)
{
    int Flightid;
    printf("Enter Flight Number to know Status\n");
    scanf("%d",&Flightid);
    int flag1,flag2;
    flag1=flag2=0;
    flightplan* dummy;
    dummy=bptr->fp;
    int count=1;
    while((flag1==0)&&(count<25))
    {
        flag2=0;
        while(flag2==0)
        {   
            if(dummy->flightid==Flightid)
            {
                
                printFlightDetails(dummy);
                flag1=1;
                flag2=1;
            }
            else if(dummy->next==NULL)
            {
                flag2=1;
            }
            dummy=dummy->next;
        }

        bptr=bptr->next;
        dummy=bptr->fp;
        count++;
    }
    if(count==25)
    {
        //We Traversed all Buckets But Flight Id doesn't match
        printf("The Flight Id doesn't exist on the DashBoard\n");
    }
}

status_code RearrangeBuck(Bucket** bptr){
    printf("Enter the present time ");
    TIME pr;
    scanf("%d:%d",&pr.hr,&pr.min);
    Bucket *aptr,*optr;
    aptr=*bptr;
    optr=*bptr;
    int flag=0;
    int flag1=0;
    int flag2=0;
    flightplan* ptr;
    while(flag==0)
    {
        ptr=aptr->fp;
        if((timediff(aptr->etabeg,pr)<60)&&(maxtime(aptr->etabeg,pr)!=1))//When timediff<60 and aptr->etabeg is ahead or equal
        {
            while(flag1==0)
            {
                if((timediff(ptr->eta,pr)<60)&&(maxtime(ptr->dep,pr)!=1))//When timediff<60 and flight plan's eta  is ahead or equal
                {   //It needs to be the starting interval of our bucket
                    flag1=1;
                    optr=InsertFPAtEnd(ptr,optr->fp);//Insertion Done But at End
                    aptr=DeleteFPpermanent(ptr,aptr);//Deleted the flightplans of Identified Bucket from the point it identify
                    aptr=aptr->next;//Went to next Buck as may be some flights may present
                    ptr=aptr->fp;
                    flightplan* dptr;
                    dptr=ptr;
                    while(flag2==0)
                    {
                        if((timediff(ptr->eta,pr)<60)&&(maxtime(ptr->dep,pr)!=1))
                        {
                            ptr=ptr->next;
                        }
                        else
                        {
                            optr=InsertFPAtEnduptoneFp(dptr,ptr);//..)
                            flag2=1;
                        }
                    }
                }
                else
                {
                    ptr=ptr->next;
                }
            }
        }
    }
    aptr->etabeg=pr;
    pr.min+=59;
    if(pr.min>59)
    {
            pr.hr+=1;
            if(pr.hr>23)
            {
                pr.hr=0;
            }
            pr.min=((pr.min)%60);
    }
}
int main()
{
    TIME a;
    a.hr=2;
    a.min=00;
    Bucket* bptr;
    int b;
    b=24;
    bptr=NULL;
    status_code sc=SUCCESS;
    long FilePosin=-1;
    sc=CreateCL(b,&bptr,a,&FilePosin);
    int flag=1;
    if(sc==FAILURE){
        printf("Error in Creating Buckets and Insertion of Flight Plans");
        flag=0;
    }
    int option;
    while(flag==1)
    {
        printf("\n       ------HOME--------\n");
        printf("1. Display Digital DashBoard\n");
        printf("2. Insert a New Flight Plan\n");
        printf("3. Cancel a flight Plan due to Emergency Situations\n");
        printf("4. Show Status of a particular Flight Plan\n");
        printf("5. Show Flight Plans within 1 Hour from present time\n");
        printf("6. Exit\n");
        printf("Choose an option from Above : ");
        scanf("%d",&option);
        switch(option)
        {
            case 1:
            {
                DisplayInfo(bptr);
                break;
            }
            case 2:
            {
                status_code a;
                a=FAILURE;
                a=InsertNewFlightPlan(&bptr);
                if(a==FAILURE)
                {
                    printf("Couldn't Insert the Flight Plan\n");
                }
                break;
            }
            case 3:
            {
                status_code b;
                b=FAILURE;
                b=CancelFlightPlan(&bptr);
                if(b==FAILURE)
                {
                    printf("The Flight Number doesn't Exist on DashBoard\n");
                }
                else
                {
                    printf("Flight plan cancelled Succesfully\n");
                }
                break;
            }
            case 4:
            {
                ShowFlightStatus(bptr);
                break;
            }
            case 5:
            {
                status_code r;
                r=FAILURE;
                r=RearrangeBuck(&bptr);
            }
            case 6:
            {
                flag=0;
                printf("Thank You!\n");
                break;
            }
        }
    }
}
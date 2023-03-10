/******************************************************************************************/
/***************************    0. Head Files needed     **********************************/
/******************************************************************************************/
#define WIN32_LEAN_AND_MEAN		
#include <cstdlib>
#include <cstdlib>
#include "stdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <ctime>
#include <vector>
#include <math.h>
#include <ctype.h>
#include <vector>
using namespace std;
/******************************************************************************************/
/********************    1. Data Structure and Global Variables    ************************/
/******************************************************************************************/
typedef struct Solution{
	int *p ;
	int **XG; 
	int *NG;
	double *SizeG; 
	double cost ; 
} Solution;  

typedef struct Neighborhood{
	int  type ; 
	int  v ;
    int  g ;
    int  x ;
    int  y ;	
} Neighborhood; 

typedef struct Pair{
    int i;
    int g; 
} Pair; 

char * File_Name;
char * Output_File_Name;
char * Solution_File;  

int N, K;  // node number and group number
double f, f_best;  
Solution CS, NS, GS, OS;     
 
Neighborhood *Neighbors;   
Pair *PairSet;  
double total_time, starting_time, Time_limit; 
int * p;      // patition array for each vertex
int * bestp ; // patition array for each vertex 
double * SizeG ;   
double ** Delta_Matrix;  // incremental matrix 
double ** Delta_Matrix1; 
double ** Delta; 
double ** Delta1; 
double ** D; // distance matrix between elements
double ** DT; 
double * LB; // Lower bound of the number of elements in the group i 
double * UB; // Upper bound of the number of elements in the group i 
double * w;  // node weights 

int **state1;
int **state2;
int **state3;

/******************************************************************************************/
/********************    2. Inputing Data and Assign Memeries   ***************************/
/******************************************************************************************/
void inputing()
{
    int i,j,k; 
    int x1, x2;
    float d;
	ifstream FIC;
	FIC.open(File_Name);
	if ( FIC.fail() )
	{
		cout << "### Erreur open, File_Name " << File_Name << endl;
		exit(0);
	}
	FIC >> N >> K; 
	char StrReading[100];  
	FIC >> StrReading; 
	if ( FIC.eof() ) 
	{
		cout << "### Error open, File_Name " << File_Name << endl;  
		exit(0);  
	}	
	if ( strcmp(StrReading, "ds" )==0 || strcmp(StrReading, "ss" )==0 ) 
	{
         LB = new double [K]; 
         UB = new double [K];  
         for(i=0;i<K;i++) { FIC >> LB[i];  FIC >> UB[i]; } 
		// for(i=0;i<K;i++)printf("%lf %lf\n",LB[i],UB[i]);
    }
    FIC >> StrReading;
	if ( FIC.eof() )
	{
		cout << "### Error open, File_Name " << File_Name << endl;
		exit(0);
	}	
	if ( strcmp(StrReading, "W" )==0)
	{
		 w  = new double [N];
		 for(i=0;i<N;i++) FIC >> w[i];
    }
//	for(i=0;i<N;i++)printf("%lf \n",w[i]);
    D = new double * [N];
    for(i=0;i<N;i++) D[i] = new double [N];
  //  for(i=0;i<N;i++)
  //    for(j=0;j<N;j++) D[i][j] = 0.0;  
      
    DT = new double * [N];
    for(i=0;i<N;i++) DT[i] = new double [N]; 
    
  //  for(i=0;i<N;i++)
  //    for(j=0;j<N;j++) DT[i][j] = 0.0; 
	while ( ! FIC.eof() )
	{
			FIC >> x1 >> x2 >> d;
			
			//cout << x1 <<"  "<< x2 <<"  "<<d<<" "<< endl;
			if ( x1<0 || x2<0 || x1>=N || x2 >=N )
			{
				cout << "### Error of node : x1="
					<< x1 << ", x2=" << x2 << endl; 
				exit(0);
			}
			if(x1 != x2)
			{	
                D[x2][x1] = d; 
                D[x1][x2] = D[x2][x1] ;
                DT[x2][x1] = 2.0*d;
                DT[x1][x2] = DT[x2][x1];
			}
	
     }
    for(i=0;i<N;i++) DT[i][i] = D[i][i] = 0.0; 
    FIC.close();
    //for(i=0;i<N;i++)
    //for(j=i+1;j<N;j++) printf("%lf\n",D[i][j]); 
 }

void AssignMemery()
{
    int i,j,x; 
	int c; 
	
    p = new int [N];
    bestp = new int [N];
    SizeG = new double [K];
	PairSet = new Pair [N*K];
	
    Delta_Matrix = new double *[N];
       for(i=0;i<N;i++) Delta_Matrix[i] = new double [K]; 
    Delta_Matrix1 = new double *[N]; 
       for(i=0;i<N;i++) Delta_Matrix1[i] = new double [K];     
       
    Delta = new double *[N]; 
       for(i=0;i<N;i++) Delta[i] = new double [K];   
    Delta1 = new double *[N]; 
       for(i=0;i<N;i++) Delta1[i] = new double [K];       
     
	CS.p = new int [N]; 
	NS.p = new int [N];  
	GS.p = new int [N];  
	OS.p = new int [N];  
	CS.NG = new int [K]; 
	NS.NG = new int [K]; 
	GS.NG = new int [K]; 
	OS.NG = new int [K]; 
    
    CS.SizeG = new double [K]; 
	NS.SizeG = new double [K]; 
	GS.SizeG = new double [K]; 
	OS.SizeG = new double [K]; 
	
    CS.XG = new int *[K]; 
    for(i=0;i<K;i++)
    {
		CS.XG[i] = new int [N] ; 
	}
    
    NS.XG = new int *[K]; 
    for(i=0;i<K;i++)
    {
		NS.XG[i] = new int [N] ; 
	}
    
    GS.XG = new int *[K]; 
    for(i=0;i<K;i++)
    { 
		GS.XG[i] = new int [N] ; 
	}
    
    OS.XG = new int *[K]; 
    for(i=0;i<K;i++)
    {
		OS.XG[i] = new int [N];  
	}
	  
	state1 = new int *[K];
	for(i=0;i<K;i++) state1[i] = new int [K]; 
	
    state2 = new int *[K];
	for(i=0;i<K;i++) state2[i] = new int [K]; 
	
	state3 = new int *[K];
	for(i=0;i<K;i++) state3[i] = new int [K]; 

  	Neighbors = new Neighborhood [N*(N-1)/2 + N*K ];  
}

void ReleaseMemery() 
{    
     int i;
     delete [] p; p = NULL;
	 delete [] bestp; bestp = NULL; 
	 delete [] SizeG; SizeG = NULL; 
	 
	 delete [] CS.p; CS.p = NULL;
	 delete [] CS.SizeG; CS.SizeG = NULL; 
	 delete [] GS.p; GS.p = NULL;
	 delete [] GS.SizeG; GS.SizeG = NULL; 
	 delete [] NS.p; NS.p = NULL;
	 delete [] NS.SizeG; NS.SizeG = NULL; 
	 delete [] OS.p; OS.p = NULL;
	 delete [] OS.SizeG; OS.SizeG = NULL; 
	
	 for(i=0;i<K;i++)
	 {
	   delete [] CS.XG[i]; CS.XG[i] = NULL; 
	   delete [] GS.XG[i]; GS.XG[i] = NULL; 
	   delete [] NS.XG[i]; NS.XG[i] = NULL; 
	   delete [] OS.XG[i]; OS.XG[i] = NULL;
     }   
     delete [] CS.NG; CS.NG = NULL; 
     delete [] GS.NG; GS.NG = NULL;
     delete [] NS.NG; NS.NG = NULL;
     delete [] OS.NG; OS.NG = NULL;
	 
	 delete [] LB; LB = NULL; 
	 delete [] UB; UB = NULL; 
	 delete [] Neighbors; Neighbors = NULL; 
	 
	 for(i=0;i<N;i++)
	 { 
	   delete [] Delta_Matrix[i]; Delta_Matrix[i] = NULL ;
       delete [] Delta_Matrix1[i]; Delta_Matrix1[i] = NULL ;
       delete [] Delta[i]; Delta[i] = NULL ;
       delete [] Delta1[i]; Delta1[i] = NULL ;
	   delete [] D[i]; D[i] = NULL; 
       delete [] DT[i]; DT[i] = NULL; 
	 }
	
}
/******************************************************************************************/
/*********************************    3. OutPuting Results   ******************************/
/******************************************************************************************/
int Proof(Solution &S)
{   
    int i,j;
    double ff;
    int flag ;
    ff = 0.0;
    for( i = 0 ; i < N ; i++ )  
	   for( j = i+1 ; j < N ; j++ )
	      {
	          if(S.p[i] == S.p[j])
               {   	
				  ff += D[i][j];
	           }
	      }
    S.cost = ff; 
  //  for(i=0;i<K;i++) S.SizeG[i] = 0.0; 
  //  for(i=0;i<N;i++) S.SizeG[S.p[i]] +=  w[i]; 
    flag = 1;
    for(i=0; i < K; i++) if(S.SizeG[i] < LB[i] - 1.0e-10 || S.SizeG[i]> UB[i] + 1.0e-10) { flag = 0; break;} 
    if(flag ==0) printf("Infeasible  f=%lf **********! \n",ff);
    else printf("\n Feasible  f = %lf  **********! \n",ff); 
  //  for(i=0; i < K; i++) printf("%lf  %lf  %lf\n",LB[i], UB[i],S.SizeG[i]);
    return flag; 
}

void Outputing(Solution &S, char *filename)
{
    int i;int r;
	FILE *fp; 
	char buff[80];
	r= rand()%1000;
	if(Proof(S)==0) return;
    sprintf(buff,"%s.sol",filename);
    fp=fopen(buff,"a+");
    fprintf(fp,"N = %d  G = %d  f = %lf\n", N , K, S.cost); 
    for(i=0;i<K;i++)
    fprintf(fp,"%lf   %lf   %lf\n", LB[i], UB[i], S.SizeG[i]); 
    printf("\n");
    for(i=0;i<N;i++)
    fprintf(fp,"%5.4d   %5.3d\n",i, S.p[i]); 
	fclose(fp);
}

void Out_results(double best , double ave,  double worst, double AvgTime, double deviation, char *filename, char instance[])
{
    int i;
	FILE *fp; 
	char buff[80];
    sprintf(buff,"%s",filename);
    fp = fopen(buff,"a+");
    fprintf(fp,"%s   %lf   %lf   %lf   %lf   %lf\n", instance, best, ave, worst, deviation, AvgTime); 
	fclose(fp); 
}

double Deviation(double arr[],int n)
{
    int i;
    double sum = 0,tmp = 0, x_avg;
    for(i = 0; i < n; ++i) sum += arr[i];
    x_avg = sum / n;
    for(i = 0; i < n; ++i)  tmp += (arr[i] - x_avg)*(arr[i] - x_avg);
    return sqrt(tmp/n); 
} // standard deviation  

/******************************************************************************************/
/****************   4. Constructive Heuristic  for Initial Solution   *********************/
/******************************************************************************************/
void RandomInitiaSol(Solution &S)
{     
     int i,j;
     int p1;
     int count, c1, Nc;
     int *Flag = new int [N];
     double v_max; int j_max;
     double *SizeGroup = new double [K];
     int *G = new int [K];
     int *VN = new int [N]; 
     for(i=0;i<K;i++) SizeGroup[i] = 0.0;
     for(i=0;i<N;i++) Flag[i] = 0;
     for(i=0;i<K;i++) S.NG[i] = 0; 
     S.cost = 0; 
     count = 0; 
     v_max = -99999.0;
     for(i=0;i<K;i++) if(SizeGroup[i] < LB[i]) G[count++] = i;  
     for(j=0;j<N;j++) if(Flag[j]==0 && w[j] > v_max) { v_max = w[j];j_max = j; }
     
     while(count > 0)
     {  
        while(1)
        { 
          p1 = j_max; 
          c1 = G[rand()%count]; 
          if(SizeGroup[c1] + w[p1] <= UB[c1])
          {
            S.p[p1] = c1; 
            Flag[p1] = 1; 
            SizeGroup[c1] += w[p1]; 
            S.XG[c1][S.NG[c1]] = p1;
            S.NG[c1] ++ ; 
            break;
          } 
        }    
     count = 0; 
     v_max = -99999.0;
     for(i=0;i<K;i++) if(SizeGroup[i] < LB[i]) G[count++] = i;  
     for(j=0;j<N;j++) if(Flag[j]==0 && w[j] > v_max) { v_max = w[j];j_max = j; }
     }
   // for(i=0;i<K;i++) printf("%lf  %lf ",LB[i], SizeGroup[i]);   
     count = 0; 
     v_max = -99999.0;
     Nc = 0; 
     for(i=0;i<K;i++) if(SizeGroup[i] < UB[i]) G[count++] = i;  
     for(j=0;j<N;j++) if(Flag[j]==0 && w[j] > v_max) { v_max = w[j];j_max = j; }
     for(j=0;j<N;j++) if(Flag[j]==0) Nc++;
     while(Nc > 0)
     {
        while(1)
        {
          p1 = j_max; 
          c1 = G[rand()%count]; 
          if(SizeGroup[c1] + w[p1] <= UB[c1])
          {
            S.p[p1] = c1; 
            Flag[p1] = 1; 
            SizeGroup[c1] += w[p1]; 
            S.XG[c1][S.NG[c1]] = p1; 
            S.NG[c1] ++ ;  
            break;
          } 
         // for(j=0;j<K;j++) printf("%lf  ",SizeGroup[j]);  printf("%d",count);
         // for(j=0;j<N;j++) if(Flag[j]==0) printf(" %lf ", w[j]); printf("\n");
        }   
      count = 0; 
      v_max = -99999.0;
      Nc = 0; 
      for(i=0;i<K;i++) if(SizeGroup[i] < UB[i]) G[count++] = i;  
      for(j=0;j<N;j++) if(Flag[j]==0 && w[j] > v_max) { v_max = w[j]; j_max = j; }
      for(j=0;j<N;j++) if(Flag[j]==0) Nc++;   
     } 

     for(i=0;i<K;i++) S.SizeG[i] = SizeGroup[i];
     
     delete [] SizeGroup; SizeGroup= NULL;
     delete [] Flag; Flag = NULL; 
     delete [] G; G = NULL;
     delete [] VN; VN = NULL; 
    // printf("\n finish construction \n");
    // for(i=0;i<K;i++) printf("%lf   %lf   %lf\n",LB[i], UB[i], S.SizeG[i]);
}
/******************************************************************************************/
/**********    5.   Variable Neighborhood Desecent method (VND)      **********************/
/******************************************************************************************/
/*void BuildNeighbors()
{
     int i,j,g;
     int count;
     int SN = N*(N-1)/2 + N*K;
     count = 0; 
     for(i=0;i<N;i++)
       for(g=0;g<K;g++)
       {
          Neighbors[count].type = 1;
          Neighbors[count].v = i ;
          Neighbors[count].g = g; 
          count ++; 
       } 
     for(i=0;i<N;i++)
       for(j=i+1;j<N;j++)
       {
          Neighbors[count].type = 2;
          Neighbors[count].x = i; 
          Neighbors[count].y = j;
          count ++; 
       }         
}
*/
//2.1 Clear delta matrix
void Clear_Delta_Matrix( )
{
	int i, j, x, g ;
	for( x = 0 ; x < N ; x++ )
		for( g = 0 ; g < K ; g++ )
		  Delta_Matrix[ x ][ g ] = 0.0 ; 	
		  
	for(i=0;i<K;i++)
	   for(j=0;j<K;j++) 
	   {
	   	 state1[i][j] = 0;
		 state2[i][j] = 0;
		 state3[i][j] = 0; 
	   }	 
	for(i=0;i<K;i++)
	{
		state1[i][i] = 1;
		state2[i][i] = 1;
		state3[i][i] = 1; 
	}	  	
}

//2.2 Build delta matrix
void Build_Delta_Matrix()
{
	int i,j;
	Clear_Delta_Matrix( );
	for(i = 0; i < N ; i++ )
	   for( j = 0 ; j < N ; j++ )
	       Delta_Matrix[ i ][ p[j] ]  +=  D[i][j];	
//	for(i = 0; i < N ; i++ )
//	   for( j = 0 ; j < K ; j++ )
//       Delta[i][j] = Delta_Matrix[ i ][ j ] - Delta_Matrix[ i ][ p[i] ];     
    f = 0.0;
    for( i = 0 ; i < N ; i++ )
       f += Delta_Matrix[i][p[i]];
    f = f/2;                   
  //  printf("\n build f= %lf ********** \n", f);  
}

//2.2 Update one move delta matrix
inline void One_Move_Update_Delta_Matrix(int i, int g0, int g1)
{
    int j;
    for(j=0;j<N;j++)
     {  
           if(j!=i)
           {
             Delta_Matrix[ j ][ g0 ] -= D[i][j];
             Delta_Matrix[ j ][ g1 ] += D[i][j];
           } 
     }                                                              
}

void LSN1(Solution &S) 
{
	 int i, j, v, swap; 
	 int g1, g2, flag, FLAG;  
	 int gi, gj; 
     double delt, ff;  
     
	 ff = S.cost;
	 
	 label: 
	 FLAG = 0; 
	 g1 = rand()%K; 
	 for(gi = 0; gi < K; gi++)
	  {
	  	g1++;
	  	if(g1==K) g1 = 0; 
	  	g2= rand()%K;
	    for(gj = 0; gj < K; gj++)
	    {
	        g2++;
	        if(g2==K) g2 = 0; 
			//if(g1 == g2 || state1[g1][g2] == 1) continue; 
	    	if(g1 == g2 || state1[g1][g2] == 1 || S.SizeG[g2] >= UB[g2] || S.SizeG[g1] <= LB[g1]) continue;   
	    	state1[g1][g2] = 1; 
	    		
	    	flag = 0; 
	    	for(i = 0; i < S.NG[g1]; i++)
	    	{
	    		if(i >= S.NG[g1]) continue;
	    		
				v = S.XG[g1][i]; 
	    		if(  ( S.SizeG[ g1 ] - w[v] >= LB[ g1 ] )  && ( S.SizeG[g2] + w[v] <= UB[g2] )  )
	    		{
	    		   	delt = Delta_Matrix[ v ][ g2 ] - Delta_Matrix[ v ][ g1 ];  
                    if(delt > 1.0e-10) 
                    {               
				      One_Move_Update_Delta_Matrix(v, g1, g2); 
				      S.SizeG[g1] = S.SizeG[g1] - w[v]; 
                      S.SizeG[g2] = S.SizeG[g2] + w[v]; 
                      
					  S.XG[g2][S.NG[g2]] = v; 
					  swap = S.XG[g1][i]; 
					  S.XG[g1][i] = S.XG[g1][S.NG[g1]-1];  
					  S.XG[g1][S.NG[g1]-1] = swap;  
					  
					  S.NG[g1] --;  
					  S.NG[g2] ++;  
				      S.p[v] = g2; 	
					     
			  	      flag = 1; 
			  	      FLAG = 1; 
			  	      ff += delt; 
			  	      i--;  
                    }
				}	
			}
			if(flag)
			{
				//a. update state1
				for(i=0;i<K;i++) 
				  if(i == g1) continue; 
				  else 
				  {
				    state1[i][g1] = 0; 
				    state1[g1][i] = 0; 
				  }
				for(j=0;j<K;j++) 
				  if(j==g2) continue; 
				  else 
				  {
				    state1[j][g2] = 0;
				    state1[g2][j] = 0;
			      }
			    //b. update state2
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state2[i][g1] = 0; 
				    state2[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state2[j][g2] = 0; 
			      	 state2[g2][j] = 0; 
				  }
			    //c. update state3
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state3[i][g1] = 0; 
				    state3[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state3[j][g2] = 0; 
			      	 state3[g2][j] = 0; 
				  }
			}
		}
      }
	 if(FLAG ==1) goto label; 
	 S.cost = ff; 
}
void LSN10(Solution &S) 
{
	 int i, j, v, swap; 
	 int g1, g2, flag, FLAG;  
     double delt, ff;  
     
	 ff = S.cost;
	 
	 label: 
	 FLAG = 0; 
	 for(g1 = 0; g1 < K; g1++)
	   for(g2 = 0; g2 < K; g2++)
	    {
	    
			//if(g1 == g2 || state1[g1][g2] == 1) continue; 
	    	if(g1 == g2 || state1[g1][g2] == 1 || S.SizeG[g2] >= UB[g2] || S.SizeG[g1] <= LB[g1]) continue;   
	    	state1[g1][g2] = 1; 
	    		
	    	flag = 0; 
	    	for(i = 0; i < S.NG[g1]; i++)
	    	{
	    		if(i >= S.NG[g1]) continue;
	    		
				v = S.XG[g1][i]; 
	    		if(  ( S.SizeG[ g1 ] - w[v] >= LB[ g1 ] )  && ( S.SizeG[g2] + w[v] <= UB[g2] )  )
	    		{
	    		   	delt = Delta_Matrix[ v ][ g2 ] - Delta_Matrix[ v ][ g1 ];  
                    if(delt > 1.0e-10) 
                    {               
				      One_Move_Update_Delta_Matrix(v, g1, g2); 
				      S.SizeG[g1] = S.SizeG[g1] - w[v]; 
                      S.SizeG[g2] = S.SizeG[g2] + w[v]; 
                      
					  S.XG[g2][S.NG[g2]] = v; 
					  swap = S.XG[g1][i]; 
					  S.XG[g1][i] = S.XG[g1][S.NG[g1]-1];  
					  S.XG[g1][S.NG[g1]-1] = swap;  
					  
					  S.NG[g1] --;  
					  S.NG[g2] ++;  
				      S.p[v] = g2; 	
					     
			  	      flag = 1; 
			  	      FLAG = 1; 
			  	      ff += delt; 
			  	      i--;  
                    }
				}	
			}
			if(flag)
			{
				//a. update state1
				for(i=0;i<K;i++) 
				  if(i == g1) continue; 
				  else 
				  {
				    state1[i][g1] = 0; 
				    state1[g1][i] = 0; 
				  }
				for(j=0;j<K;j++) 
				  if(j==g2) continue; 
				  else 
				  {
				    state1[j][g2] = 0;
				    state1[g2][j] = 0;
			      }
			    //b. update state2
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state2[i][g1] = 0; 
				    state2[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state2[j][g2] = 0; 
			      	 state2[g2][j] = 0; 
				  }
			    //c. update state3
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state3[i][g1] = 0; 
				    state3[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state3[j][g2] = 0; 
			      	 state3[g2][j] = 0; 
				  }
			}
		}
	 if(FLAG ==1) goto label; 
	 S.cost = ff; 
}
void LocalSearch_N1(int p1[], double SizeG[], double *cost)
{
     int v, g, counter, old_g;
     double delt, ff; 
     
     counter = 0; 
     ff = *cost;
     while(counter < N*K)
     {
       for(v=0;v<N;v++)
       {
         for(g=0;g<K;g++)
           if( ( p1[v] != g ) && ( SizeG[ p1[v] ] - w[v] >= LB[ p1[v] ] ) && (SizeG[g] + w[v] <= UB[g] )  )
             {
               delt = Delta_Matrix[ v ][ g ] - Delta_Matrix[ v ][ p1[ v ] ]; 
               //printf("delt =%lf\n",delt);
               if(delt > 1.0e-11)
               {  
                  old_g = p1[v] ;              
				  One_Move_Update_Delta_Matrix(v, old_g, g);
				  SizeG[old_g] = SizeG[old_g] - w[v];
                  SizeG[g] = SizeG[g] + w[v];
				  p1[v] = g;	 
			  	  ff += delt; 
			  	  counter = 0; 
			  	 // printf(" delt = %lf improve !\n",delt);
               }
               else counter++; 
             }
             else counter ++;
        if(counter >= N*K) break;   
       }
     }
     *cost = ff; 
     
    // printf("N1 f=%lf\n",ff); 
}

int LSN20(Solution &S)
{
	 int i, j, v, u, swap;
	 int g1, g2, flag; 
     double delt, ff; 
     
	 ff = S.cost;
	 flag = 0; 
	 for(g1 = 0; g1 < K; g1++)
	   for(g2 = g1 + 1 ; g2 < K; g2++)
	    {
	    	if(g1 == g2 || state2[g1][g2] == 1) continue;  
	    	state2[g1][g2] = 1;  
	    	state2[g2][g1] = 1;   
	    	
	    	flag = 0;  
	    	for(i = 0; i < S.NG[g1]; i++) 
	    	  for(j = 0; j < S.NG[g2]; j++) 
	    	  {
				v = S.XG[g1][i];  
				u = S.XG[g2][j];  
				
	    		if(  ( S.SizeG[ g1 ] - w[v] + w[u] >= LB[ g1 ] ) && (S.SizeG[ g1 ] - w[v] + w[u] <= UB[ g1 ]) && (S.SizeG[g2] + w[v] - w[u] >= LB[g2] )  && ( S.SizeG[g2] + w[v] - w[u] <= UB[g2] )  )
	    		{
	    		   	delt =  (Delta_Matrix[ v ][ g2 ] - Delta_Matrix[ v ][ g1 ]) + (Delta_Matrix[ u ][ g1 ] - Delta_Matrix[ u ][ g2 ]) - DT[v][u];
                    if(delt > 1.0e-10) 
                    {               
				      One_Move_Update_Delta_Matrix(v, g1, g2); 
				      One_Move_Update_Delta_Matrix(u, g2, g1); 
				      
				      S.SizeG[g1] += ( w[u] - w[v]); 
                      S.SizeG[g2] += ( w[v] - w[u]); 
                      
					  swap = S.XG[g1][i]; 
					  S.XG[g1][i] = S.XG[g2][j];  
					  S.XG[g2][j] = swap;  
					  
				      S.p[v] = g2; 	
					  S.p[u] = g1; 
					 
			  	      flag = 1; 
			  	      ff += delt; 
                    }
				}	
		      }
		      
			if(flag)
			{
				//a. update state1
				for(i=0;i<K;i++) 
				  if(i == g1) continue; 
				  else 
				  {
				    state1[i][g1] = 0; 
				    state1[g1][i] = 0; 
				  }
				for(j=0;j<K;j++) 
				  if(j==g2) continue; 
				  else 
				  {
				    state1[j][g2] = 0;
				    state1[g2][j] = 0;
			      }
			    //b. update state2
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state2[i][g1] = 0; 
				    state2[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state2[j][g2] = 0; 
			      	 state2[g2][j] = 0; 
				  }
			    //c. update state3
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state3[i][g1] = 0; 
				    state3[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state3[j][g2] = 0; 
			      	 state3[g2][j] = 0; 
				  }
				goto label; 
			}
		}
	 label: 	
	 S.cost = ff; 	
	 if(flag ==1) return 1; 
	 else return 0; 
}
int LSN2(Solution &S)
{
	 int i, j, v, u, swap;
	 int g1, g2, flag; 
	 int gi,gj; 
     double delt, ff; 
     
	 ff = S.cost;
	 flag = 0; 
	 g1 = rand()%K; 
	 for(gi = 0; gi < K; gi++)
	  {
	   g1 ++; 
	   if(g1==K) g1 = 0; 
	   g2 = rand()%K;
	   for(gj = 0 ; gj < K; gj++)
	    {
	    	g2 ++; 
	    	if(g2==K) g2 = 0; 
	    	
			if(g1 == g2 || state2[g1][g2] == 1) continue;  
	    	state2[g1][g2] = 1;  
	    	state2[g2][g1] = 1;   
	    	
	    	flag = 0;  
	    	for(i = 0; i < S.NG[g1]; i++) 
	    	  for(j = 0; j < S.NG[g2]; j++) 
	    	  {
				v = S.XG[g1][i];  
				u = S.XG[g2][j];  
				
	    		if(  ( S.SizeG[ g1 ] - w[v] + w[u] >= LB[ g1 ] ) && (S.SizeG[ g1 ] - w[v] + w[u] <= UB[ g1 ]) && (S.SizeG[g2] + w[v] - w[u] >= LB[g2] )  && ( S.SizeG[g2] + w[v] - w[u] <= UB[g2] )  )
	    		{
	    		   	delt =  (Delta_Matrix[ v ][ g2 ] - Delta_Matrix[ v ][ g1 ]) + (Delta_Matrix[ u ][ g1 ] - Delta_Matrix[ u ][ g2 ]) - DT[v][u];
                    if(delt > 1.0e-10) 
                    {               
				      One_Move_Update_Delta_Matrix(v, g1, g2); 
				      One_Move_Update_Delta_Matrix(u, g2, g1); 
				      
				      S.SizeG[g1] += ( w[u] - w[v]); 
                      S.SizeG[g2] += ( w[v] - w[u]); 
                      
					  swap = S.XG[g1][i]; 
					  S.XG[g1][i] = S.XG[g2][j];  
					  S.XG[g2][j] = swap;  
					  
				      S.p[v] = g2; 	
					  S.p[u] = g1; 
					 
			  	      flag = 1; 
			  	      ff += delt; 
                    }
				}	
		      }
		      
			if(flag)
			{
				//a. update state1
				for(i=0;i<K;i++) 
				  if(i == g1) continue; 
				  else 
				  {
				    state1[i][g1] = 0; 
				    state1[g1][i] = 0; 
				  }
				for(j=0;j<K;j++) 
				  if(j==g2) continue; 
				  else 
				  {
				    state1[j][g2] = 0;
				    state1[g2][j] = 0;
			      }
			    //b. update state2
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state2[i][g1] = 0; 
				    state2[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state2[j][g2] = 0; 
			      	 state2[g2][j] = 0; 
				  }
			    //c. update state3
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state3[i][g1] = 0; 
				    state3[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state3[j][g2] = 0; 
			      	 state3[g2][j] = 0; 
				  }
				goto label; 
			}
		}
		
      }
	 label: 	
	 S.cost = ff; 	
	 if(flag ==1) return 1; 
	 else return 0; 
}
int LocalSearch_N2(int p1[], double SizeG[], double *cost)
{
     int x, y, g, counter;
     double delt,ff; 
     int improve_cutoff; 
     int old_g, old_g1, swap;
     
     ff = *cost; 
     counter = 0; 
     improve_cutoff = 0;
     while(counter < N*(N-1)/2)
     {
        for(x=0;x<N;x++)
        {
           for(y=x+1;y<N;y++)
             if((p1[x] != p1[y])&&(SizeG[p1[x]] + w[y] - w[x] >= LB[p1[x]]) && (SizeG[p1[x]] + w[y] - w[x] <= UB[p1[x]]) && (SizeG[p1[y]] + w[x] - w[y] >= LB[p1[y]])&&(SizeG[p1[y]] + w[x] - w[y] <= UB[p1[y]]) )
              {  
              // delt = Delta[ x ][ p1[y] ] + Delta[ y ][ p1[x] ] - DT[x][y];   
               delt =  (Delta_Matrix[ x ][ p1[y] ] - Delta_Matrix[ x ][ p1[ x ] ]) + (Delta_Matrix[ y ][ p1[x] ] - Delta_Matrix[ y ][ p1[ y ] ]) - DT[x][y];
               if(delt > 1.0e-10)
               {
                old_g  = p1[ x ]; 
                old_g1 = p1[ y ];   
                SizeG[old_g]  += (w[y]-w[x]);
                SizeG[old_g1] += (w[x]-w[y]);     
				One_Move_Update_Delta_Matrix( x, old_g, old_g1);
				
				swap    = p1[ x ];
				p1[ x ] = p1[ y ];
				One_Move_Update_Delta_Matrix( y, old_g1, old_g);
				p1[ y ] = swap; 
                  
				ff += delt; 
			   //printf("swap delt = %lf\n",delt);
				counter = 0; 
				
				improve_cutoff ++; 
				if(improve_cutoff >= 10) {*cost = ff; return 1;}
               }
               else counter ++;
             }
             else counter ++;
          if(counter >= N*(N-1)/2) break; 
        }
     }
    *cost = ff; 
   // printf("N2 f=%lf\n",ff);
    if(improve_cutoff > 0) return 1; 
    else return 0; 
}

int LSN3(Solution &S)
{
	 int i, j, k, x, y, z, swap;
	 int g1, g2, flag; 
	 int old_g, old_g1, old_g2;  
	 int gi, gj; 
     double delt, ff; 
     
	 ff = S.cost;
	 
	 flag = 0; 
	 
	 for(g1 = 0; g1 < K; g1 ++)
	  {
	    for(g2 = 0; g2 < K; g2 ++)
	    {
			if(g1 == g2 || state3[g1][g2] == 1) continue;  
	    	state3[g1][g2] = 1;   
	    	
	    	flag = 0;  
	    	for(i = 0; i < S.NG[g1]; i++) 
	    	 for(j = i+1; j < S.NG[g1]; j++) 
	    	  for(k = 0; k < S.NG[g2]; k++)
	    	  {
			
				x = S.XG[g1][i];  
				y = S.XG[g1][j]; 
				z = S.XG[g2][k];  
				
				if(i==j || i >= S.NG[g1] || j >= S.NG[g1] ||k >= S.NG[g2]) continue; 
			    // if(S.p[x] != g1 || S.p[y] != g1 || S.p[z] != g2) printf("error in XG !!!!\n"); 
				
	    		if(  ( S.SizeG[ g1 ] - w[x] - w[y] + w[z] >= LB[ g1 ] ) && (S.SizeG[ g1 ] - w[x] - w[y] + w[z] <= UB[ g1 ]) && (S.SizeG[g2] + w[x] + w[y] - w[z] >= LB[g2] )  && ( S.SizeG[g2] + w[x] + w[y] - w[z] <= UB[g2] )  )
	    		{
	    		   	delt =  (Delta_Matrix[ x ][ g2 ] - Delta_Matrix[ x ][ g1 ]) + (Delta_Matrix[ z ][ g1 ] - Delta_Matrix[ z ][ g2 ]) - DT[x][z];
                    delt += ((Delta_Matrix[ y ][ g2 ] - Delta_Matrix[ y ][ g1 ]) + (DT[y][x]- DT[y][z]));  
	    		   	
                    if(delt > 1.0e-10) 
                    {               
                    
                     S.SizeG[g1]  += (w[z]-w[y]-w[x]);
                     S.SizeG[g2]  += (w[x]+w[y]-w[z]);   
                   
				     One_Move_Update_Delta_Matrix( x, g1, g2);
				     One_Move_Update_Delta_Matrix( z, g2, g1);
                     One_Move_Update_Delta_Matrix( y, g1, g2); 
				     
				     S.p[ x ] = g2;
				     S.p[ y ] = g2;
				     S.p[ z ] = g1;
				     
                     swap = S.XG[g1][i] ;
                     S.XG[g1][i] = S.XG[g2][k];
                     S.XG[g2][k] = swap; 
                     
                     swap = S.XG[g1][j] ;
                     S.XG[g1][j] = S.XG[g1][S.NG[g1]-1];
                     S.XG[g1][S.NG[g1]-1] = swap; 
                     
                     S.XG[g2][S.NG[g2]] = S.XG[g1][S.NG[g1]-1]; 
                     
                     S.NG[g1] --;
					 S.NG[g2] ++; 
					
					// printf("two-one swap move LS improved !\n");
					 
			  	     flag = 1; 
			  	     ff += delt; 
                    }
				}	
		      }
		      
			if(flag)
			{
				//a. update state1 
				for(i=0;i<K;i++) 
				  if(i == g1) continue; 
				  else 
				  {
				    state1[i][g1] = 0; 
				    state1[g1][i] = 0; 
				  }
				for(j=0;j<K;j++) 
				  if(j==g2) continue; 
				  else 
				  {
				    state1[j][g2] = 0;
				    state1[g2][j] = 0;
			      }
			    //b. update state2
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state2[i][g1] = 0; 
				    state2[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state2[j][g2] = 0; 
			      	 state2[g2][j] = 0; 
				  }
			    //c. update state3
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state3[i][g1] = 0; 
				    state3[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state3[j][g2] = 0; 
			      	 state3[g2][j] = 0; 
				  }
				 goto label;
			}
		}
	}
	 label:
	 S.cost = ff; 	
	 if(flag ==1) return 1; 
	 else return 0; 
}
int LSN32(Solution &S)
{
	 int i, j, k, x, y, z, swap;
	 int g1, g2, flag; 
	 int old_g, old_g1, old_g2;  
	 int gi, gj; 
     double delt, ff; 
     
	 ff = S.cost;
	 
	 flag = 0; 
	 
	 g1 = rand()%K; 
	 for(gi = 0; gi < K; gi ++)
	  {
	    g1++;
		if(g1==K) g1 = 0; 
		
		g2 = rand()%K; 
	    for(gj = 0; gj < K; gj ++)
	    {
	    	g2++;
	    	if(g2==K) g2 = 0; 
	    	
			if(g1 == g2 || state3[g1][g2] == 1) continue;  
	    	state3[g1][g2] = 1;   
	    	
	    	flag = 0;  
	    	for(i = 0; i < S.NG[g1]; i++) 
	    	 for(j = i+1; j < S.NG[g1]; j++) 
	    	  for(k = 0; k < S.NG[g2]; k++)
	    	  {
			
				x = S.XG[g1][i];  
				y = S.XG[g1][j]; 
				z = S.XG[g2][k];  
				
				if(i==j || i >= S.NG[g1] || j >= S.NG[g1] ||k >= S.NG[g2]) continue; 
			    // if(S.p[x] != g1 || S.p[y] != g1 || S.p[z] != g2) printf("error in XG !!!!\n"); 
				
	    		if(  ( S.SizeG[ g1 ] - w[x] - w[y] + w[z] >= LB[ g1 ] ) && (S.SizeG[ g1 ] - w[x] - w[y] + w[z] <= UB[ g1 ]) && (S.SizeG[g2] + w[x] + w[y] - w[z] >= LB[g2] )  && ( S.SizeG[g2] + w[x] + w[y] - w[z] <= UB[g2] )  )
	    		{
	    		   	delt =  (Delta_Matrix[ x ][ g2 ] - Delta_Matrix[ x ][ g1 ]) + (Delta_Matrix[ z ][ g1 ] - Delta_Matrix[ z ][ g2 ]) - DT[x][z];
                    delt += ((Delta_Matrix[ y ][ g2 ] - Delta_Matrix[ y ][ g1 ]) + (DT[y][x]- DT[y][z]));  
	    		   	
                    if(delt > 1.0e-10) 
                    {               
                    
                     S.SizeG[g1]  += (w[z]-w[y]-w[x]);
                     S.SizeG[g2]  += (w[x]+w[y]-w[z]);   
                   
				     One_Move_Update_Delta_Matrix( x, g1, g2);
				     One_Move_Update_Delta_Matrix( z, g2, g1);
                     One_Move_Update_Delta_Matrix( y, g1, g2); 
				     
				     S.p[ x ] = g2;
				     S.p[ y ] = g2;
				     S.p[ z ] = g1;
				     
                     swap = S.XG[g1][i] ;
                     S.XG[g1][i] = S.XG[g2][k];
                     S.XG[g2][k] = swap; 
                     
                     swap = S.XG[g1][j] ;
                     S.XG[g1][j] = S.XG[g1][S.NG[g1]-1];
                     S.XG[g1][S.NG[g1]-1] = swap; 
                     
                     S.XG[g2][S.NG[g2]] = S.XG[g1][S.NG[g1]-1]; 
                     
                     S.NG[g1] --;
					 S.NG[g2] ++; 
					
					// printf("two-one swap move LS improved !\n");
					 
			  	     flag = 1; 
			  	     ff += delt; 
                    }
				}	
		      }
		      
			if(flag)
			{
				//a. update state1 
				for(i=0;i<K;i++) 
				  if(i == g1) continue; 
				  else 
				  {
				    state1[i][g1] = 0; 
				    state1[g1][i] = 0; 
				  }
				for(j=0;j<K;j++) 
				  if(j==g2) continue; 
				  else 
				  {
				    state1[j][g2] = 0;
				    state1[g2][j] = 0;
			      }
			    //b. update state2
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state2[i][g1] = 0; 
				    state2[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state2[j][g2] = 0; 
			      	 state2[g2][j] = 0; 
				  }
			    //c. update state3
			    for(i=0;i<K;i++)
			      if(i==g1) continue; 
			      else
			      {
			      	state3[i][g1] = 0; 
				    state3[g1][i] = 0;  
				  }
			    for(j=0;j<K;j++) 
			      if(j==g2) continue; 
			      else
			      {
			      	 state3[j][g2] = 0; 
			      	 state3[g2][j] = 0; 
				  }
				 goto label;
			}
		}
	}
	 label:
	 S.cost = ff; 	
	 if(flag ==1) return 1; 
	 else return 0; 
}
int LocalSearch_N3(int p1[], double SizeG[], double *cost)
{
     int x, y, z, g, counter;
     double delt,ff; 
     int improve_cutoff; 
     int old_g, old_g1, old_g2, swap;
     
     ff = *cost; 
     counter = 0; 
     improve_cutoff = 0;
     while(counter < N*N*(N-1)/2)
     {
        for(x=0;x<N;x++)
        {
           for(y=x+1;y<N;y++)
           { 
             for(z=0;z<N;z++)
             {
              if( (p1[x] == p1[y] && p1[x] != p1[z]) && (SizeG[p1[x]] - w[y] - w[x] + w[z] >= LB[p1[x]]) && (SizeG[p1[x]] - w[y] - w[x] + w[z] <= UB[p1[x]]) && (SizeG[p1[z]] + w[x] + w[y] - w[z] >= LB[p1[z]]) && (SizeG[p1[z]] + w[x] + w[y] - w[z] <= UB[p1[z]]) )
              {  
              // delt = (Delta[ x ][ p[z] ] + Delta[ z ][ p[x] ] - DT[x][z]) + Delta[ y ][ p[z] ] + DT[y][x]- DT[y][z];  
               delt =  (Delta_Matrix[ x ][ p1[z] ] - Delta_Matrix[ x ][ p1[ x ] ]) + (Delta_Matrix[ z ][ p1[x] ] - Delta_Matrix[ z ][ p1[ z ] ]) - DT[x][z];
               delt += ((Delta_Matrix[ y ][ p1[z] ] - Delta_Matrix[ y ][ p1[ y ] ]) + (DT[y][x]- DT[y][z]));
               if(delt > 1.0e-10)
               {
                old_g  = p1[ x ]; 
                old_g1 = p1[ y ];  
                old_g2 = p1[ z ]; 
                SizeG[old_g]  += (w[z]-w[y]-w[x]);
                SizeG[old_g2] += (w[x]+w[y]-w[z]);     
                   
				One_Move_Update_Delta_Matrix( x, old_g, old_g2);
				
				swap   = p1[ x ];
				p1[ x ] = p1[ z ];
				One_Move_Update_Delta_Matrix( z, old_g2, old_g);
				p1[ z ] = swap; 
                  
                One_Move_Update_Delta_Matrix( y, old_g1, old_g2);
                p1[y] = old_g2;
               // printf("N3 improve %lf\n",delt);
				ff += delt; 
				counter = 0; 
				improve_cutoff ++; 
				if(improve_cutoff >= 10) {*cost = ff; return 1;}
               }
               else counter ++;
             }
             else counter ++;
             if(counter >= N*N*(N-1)/2) break; 
           }
             if(counter >= N*N*(N-1)/2) break; 
          }
             if(counter >= N*N*(N-1)/2) break; 
       }
    }
    
    *cost = ff; 
   // printf("N3 f=%lf\n",ff);
    if(improve_cutoff > 0) return 1; 
    else return 0; 
}

void VND2(Solution &S)
{ 
     int i,rez;
     
     for(i=0;i<N;i++) p[i] = S.p[i];
     Build_Delta_Matrix(); 
     S.cost = f;     
     
     do
     {
        LSN1(S);
        rez =  LSN2(S); 
     } while(rez);

}

void VND3(Solution &S)
{ 
     int i,rez;
     
     for(i=0;i<N;i++) p[i] = S.p[i];
     Build_Delta_Matrix(); 
     S.cost = f;  
     
	 do
	 {
       do 
       { 
          LSN1(S); 
          rez =  LSN2(S);  
       } while(rez); 
      rez = LSN3(S);  
    } while(rez);
}

void Extend_VND(int partition[], double SizeG[], double *cost)
{ 
     int i,rez;
     double fc;
     for(i=0;i<N;i++) p[i] = partition[i];
     for(i=0;i<K;i++) SizeG[i] = 0.0;
	 for(i=0;i<N;i++) SizeG[p[i]] += w[i]; 
	 
     Build_Delta_Matrix(); 
     fc = f;     
     do
     {
       do
       {
          LocalSearch_N1(partition, SizeG, &fc);
          rez  =  LocalSearch_N2(partition, SizeG, &fc);
       } while(rez);
       rez = LocalSearch_N3(partition, SizeG, &fc);
     } while(rez);
     
     *cost = fc;  
}

/******************************************************************************************/
/*************************   6. Perturbation Operators    *********************************/
/******************************************************************************************/
double distance1(int S1[], int S2[]) 
{
    int i,j; 
    int Ecount; 
    double dist;
    Ecount = 0; 
    for(i=0;i<N;i++)
      for(j=i+1;j<N;j++)
        if( (S1[i] == S1[j] && S2[i] != S2[j])  ||  (S1[i] != S1[j] && S2[i] == S2[j]) ) Ecount++;   
    dist = 1.0*(Ecount*K)/(1.0*N*N);  
    //printf("Dist =%d  dis = %lf \n",Ecount, dist); 
    return dist; 
} 
double distance(int S1[], int S2[]) 
{
    int i,j; 
    int Ecount, Ecount1; 
    double dist;
    Ecount = 0; Ecount1 = 0; 
    for(i=0;i<N;i++)
      for(j=i+1;j<N;j++)
		{
		  if( (S1[i] == S1[j] && S2[i] != S2[j])  ||  (S1[i] != S1[j] && S2[i] == S2[j]) ) Ecount++;   
		  if(S1[i]==S1[j] || S2[i] == S2[j]) Ecount1++;  
    	}
    //dist = 1.0*(Ecount*K)/(1.0*N*N);  
    dist = (double) Ecount/Ecount1; 
    //printf("Dist =%d  dis = %lf \n",Ecount, dist); 
    return dist; 
} 
void Shake(Solution &S, int theta)
{
    int g1,g2, x, y, r1, r2;
    int swap ;
    int count = 0; 
    
    do
	{     
             g1 =  rand()%K; 
             g2 =  rand()%K; 
             if(g1 == g2) continue; 
             r1 = rand()%S.NG[g1]; 
             r2 = rand()%S.NG[g2]; 
             
             x = S.XG[g1][r1];
             y = S.XG[g2][r2]; 
             
             if( (S.p[x] != S.p[y]) && (S.SizeG[ S.p[x] ] + (w[y]-w[x]) >= LB[S.p[x]]) && (S.SizeG[ S.p[x] ] + (w[y]-w[x]) <= UB[S.p[x]]) && (S.SizeG[ S.p[y] ] + (w[x]-w[y]) >= LB[S.p[y]]) && (S.SizeG[ S.p[y] ] + (w[x]-w[y]) <= UB[S.p[y]]) )
             { 
                  	
                S.SizeG[ S.p[x] ] += (w[y]-w[x]);
                S.SizeG[ S.p[y] ] += (w[x]-w[y]);
                	
				swap   = S.p[ x ];
				S.p[ x ] = S.p[ y ];  
				S.p[ y ] = swap; 
				
			    swap = S.XG[g1][r1]; 
			    S.XG[g1][r1] = S.XG[g2][r2] ;
			    S.XG[g2][r2] = swap; 
			    
				count ++; 
             }      
            	                            
    } while(count < theta);  
    
 }

void NDVNS()
{     
     int i,j; 
     int k, k_min = 1, k_step = 1, k_max = N/K; 
     double r, alpha = 0.01;  
     GS.cost = -99999.0; 
   //  if(N/K < k_max) k_max = N/K; 
     starting_time = clock(); 
     
     RandomInitiaSol(CS);  
     VND3(CS); 
	      
     for(i=0;i<N;i++) GS.p[i] = CS.p[i];
     for(j=0;j<K;j++) GS.SizeG[j] = CS.SizeG[j];
     for(j=0;j<K;j++) GS.NG[j] = CS.NG[j]; 
     for(i=0;i<K;i++)
       for(j=0;j<GS.NG[i]; j++) GS.XG[i][j] = CS.XG[i][j]; 
     GS.cost = CS.cost; 
    
     k = k_min; 
     while( 1.0*(clock()- starting_time)/CLOCKS_PER_SEC  <  Time_limit )
     {
        for(i=0;i<N;i++) NS.p[i] = CS.p[i]; 
        for(j=0;j<K;j++) NS.SizeG[j] = CS.SizeG[j]; 
        for(j=0;j<K;j++) NS.NG[j] = CS.NG[j]; 
        for(i=0;i<K;i++)
          for(j=0;j<NS.NG[i]; j++) NS.XG[i][j] = CS.XG[i][j]; 
        NS.cost = CS.cost; 
        Shake(NS,k); 
        VND2(NS);  
       // printf("k=%d\n",k);
        if( (1.0*NS.cost/CS.cost + alpha*distance(NS.p, CS.p) > 1.0) && (1.0*NS.cost/GS.cost + alpha*distance(NS.p, GS.p) > 1.0)  )
        {
           for(i=0;i<N;i++) CS.p[i] = NS.p[i]; 
           for(j=0;j<K;j++) CS.SizeG[j] = NS.SizeG[j]; 
           for(j=0;j<K;j++) CS.NG[j] = NS.NG[j]; 
           for(i=0;i<K;i++)
             for(j=0;j<CS.NG[i]; j++) CS.XG[i][j] = NS.XG[i][j]; 
           CS.cost = NS.cost;
           
           if(CS.cost > GS.cost) 
           {
             for(i=0;i<N;i++) GS.p[i] = CS.p[i]; 
             for(j=0;j<K;j++) GS.SizeG[j] = CS.SizeG[j]; 
             for(j=0;j<K;j++) GS.NG[j] = CS.NG[j]; 
             for(i=0;i<K;i++)
             for(j=0;j<GS.NG[i]; j++) GS.XG[i][j] = CS.XG[i][j]; 
             GS.cost = CS.cost; 
            // printf("f=%lf  k=%d  %lf \n",GS.cost,k, 1.0*(clock()- starting_time)/CLOCKS_PER_SEC );  
             total_time = 1.0*(clock()- starting_time)/CLOCKS_PER_SEC ;
             k = k_min;
           }
           else
           {
           	 r = (double)(rand()%RAND_MAX)/RAND_MAX; 
           	 if(r <= 0.2) k = k_min; 
           	 else k = k + k_step; 
           }   
        } 
        else  k = k + k_step;   
        if(k > k_max) k = k_min;
    }         
} 

/******************************************************************************************/
/********************    13. Main Function for Multiple Runs     **************************/
/******************************************************************************************/
int main(int argc, char *argv[])
{
    int i,j;
    int i1,j1;
    int seed; 
    const int  Times = 20; 
    double F[Times];
    double Ctime[Times];
    double AvgTime = 0.0; 
    double F_best= -99999999, F_worst = 999999999, F_ave = 0.0, deviation;
    seed = 0 ; 
	srand( seed );	 

   // File_Name = "MDG-a_21.txt"; 
   // Output_File_Name = "ss.txt"; 
   // Solution_File = "RanReal240_01.txt.sol"; 
     
    File_Name = argv[1]; 
    //Solution_File = argv[2];
    Output_File_Name = argv[2];
	//Time_limit = atoi(argv[4]); 
    Output_File_Name = "CCP_NDVNS.txt";
     
    inputing();  
    AssignMemery();
    Time_limit = 1.0*N;
    // BuildNeighbors();
    
    OS.cost = -9999999.0; 
    for(j=0;j<Times;j++) {F[j] = 0.0; Ctime[j] = 0; }
    for(i=0; i < Times; i++)
    { 
    
     NDVNS();
     if(Proof(GS)) 
      { 
        F[i] = GS.cost; 
        Ctime[i] = total_time; 
        if(F[i]> OS.cost)
        {
            for(i1=0;i1<N;i1++) OS.p[i1] = GS.p[i1]; 
            for(j1=0;j1<K;j1++) OS.SizeG[j1] = GS.SizeG[j1]; 
            OS.cost = GS.cost;  
        }
      }
      else i--;
	   
    //  printf("f[%d] = %lf \n",i, F[i]);
    }
    for(i=0;i<Times;i++)   
    {
       if(F[i] > F_best )  F_best = F[i];    
       if(F[i] < F_worst)  F_worst = F[i];  
       F_ave += F[i];   
       AvgTime += Ctime[i];
    }
    F_ave   /=  Times; 
    AvgTime /=  Times; 
    deviation = Deviation(F,Times);
    Out_results(F_best, F_ave, F_worst, AvgTime, deviation, Output_File_Name, File_Name); 
    Outputing(OS, File_Name);
    ReleaseMemery();
    return 0;
}

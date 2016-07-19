using namespace std;

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "gmp.h"
#include "bdd.h"
#include "math.h"
#include "bdd_probability.h"
#include <iostream>
#include <fstream>

const int INITBDDNODES       = 1000;
const int INITBDDCACHE       = 1000000;
const int INITBDDMAXINCREASE = 3000000;

/* return "a - b" in seconds */
double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}//time_diff

int main(int argc, char **argv)  
{
	/* Initilize buddy */
	bdd_init(INITBDDNODES,INITBDDCACHE);
    bdd_setmaxincrease(INITBDDMAXINCREASE);
    
    struct timeval t_ini, t_fin;
    double secs;
    
	bdd solutionSpace;
	double sat;
    
    FILE *f;
    
    int count, method;  
    mpq_t *prob_rational;
    
    if (argc != 3) 
    {
       cerr << "Usage: " << argv[0]
            << " <bdd file> <option method>" << endl;
        exit(-1);
    }
   
    ifstream mybdd(argv[1]);
    method = atoi(argv[2]);
    
    if (!mybdd) 
    {
        cerr << "File " << argv[1] << " not found" << endl;
        exit(-1);
    }
    
    /* delete comment if you want to use automatic reordering techniques*/
    //bdd_autoreorder(BDD_REORDER_SIFT);
    
    /*load the bdd*/
	bdd_fnload((char *)argv[1], solutionSpace);
	
	cout << "File " << argv[1] << " loaded" << endl;
	
	bdd_varblockall();
	bdd_reorder(BDD_REORDER_SIFT);
		
	//Initialize time	
    gettimeofday(&t_ini, NULL);		
    
    switch(method)
    {
	  case 1://brute force approach
	    
		sat = bdd_satcount(solutionSpace);
		for (int j=0; j<bdd_varnum(); ++j)
			cout << j << ": " << bdd_satcount(solutionSpace&bdd_ithvar(j))/sat << endl;
		
		gettimeofday(&t_fin, NULL);
		secs = timeval_diff(&t_fin, &t_ini);
	
		fprintf(stdout, "time in %.16g seconds\n", secs);
		break;
		
	  case 2://our approach
	 
		prob_rational = (mpq_t *)malloc((bdd_varnum())*sizeof(mpq_t));
		sat = bdd_get_probability(solutionSpace, prob_rational);
		
		for (int j=0; j<bdd_varnum(); ++j)
			cout << j << ": " << mpq_get_d(prob_rational[j]) << endl;
		
		gettimeofday(&t_fin, NULL);
		secs = timeval_diff(&t_fin, &t_ini);
	
		fprintf(stdout, "time in %.16g seconds\n", secs);
		for (int j=0; j<bdd_varnum(); ++j)
			mpq_clear(prob_rational[j]);
		
   	    break;
   	    
   	  default:
		fprintf(stdout, "invalid option %d\n", method);
			
	}//end switch
		
	bdd_done();	
	
}//End test

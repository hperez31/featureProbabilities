#include "bdd_probability.h"

using namespace std;

int get_sat_prob_rec(const bdd r, mpq_t *formula_sat_prob, int *marks, int count)
{
    int n, level;
    mpq_t temp;
    
    if (!marks[r.id()])
    {    
		marks[r.id()] = 1; 
		if (r.id() == 1) mpq_set_ui(formula_sat_prob[1], 1, 1);
		else {
			if (bdd_high(r).id() != 0)
				count = get_sat_prob_rec(bdd_high(r), formula_sat_prob, marks, count);
			if (bdd_low(r).id() != 0)
				count = get_sat_prob_rec(bdd_low(r), formula_sat_prob, marks, count);
			
			mpq_init(temp);
			mpq_div_2exp(temp, formula_sat_prob[bdd_high(r).id()], 1);				
			mpq_add(formula_sat_prob[r.id()], formula_sat_prob[r.id()], temp);
			mpq_div_2exp(temp, formula_sat_prob[bdd_low(r).id()], 1);				
			mpq_add(formula_sat_prob[r.id()], formula_sat_prob[r.id()], temp);
			
			mpq_clear(temp);
			count++;
			
		}//end else		
	}//end if 
   	return count;					
}//end get_sat_prob

int get_diff(const bdd r, mpq_t *weight_sat_prob, int count){
	 
	int parent, low, high, levelLow, levelHigh;
    mpq_t flowlow, flowhigh, temphalf, temp2;
    map<BDD, bool> mark;
    list<BDD> stack;
    
    mark[0] = true;
    mark[1] = true;
    
    BDD root = r.id();
    mpq_set_ui(weight_sat_prob[root], 1, 1);
    
    mpq_init(temp2);
    mpq_init(temphalf);
    
    vector<pair<int, int> > theHeap;
    theHeap.push_back(pair<int, int>(-1, root));
    push_heap(theHeap.begin(), theHeap.end());
    
    while (! theHeap.empty()) {
        parent = theHeap.front().second;
        stack.push_front(parent);
        pop_heap(theHeap.begin(), theHeap.end());
        theHeap.pop_back();
        
        low       = bdd_low(parent);
        high      = bdd_high(parent);
        
        mpq_div_2exp(temphalf, weight_sat_prob[parent], 1);
        
        mpq_add(weight_sat_prob[low], weight_sat_prob[low], temphalf);
        mpq_add(weight_sat_prob[high], weight_sat_prob[high], temphalf);
        count++;
        if (!mark[low]) {
            theHeap.push_back(pair<int, int>(-bdd_var2level(bdd_var(low)),low));
            mark[low] = true;
            push_heap(theHeap.begin(), theHeap.end());
        }
    
        if (!mark[high]) {
            theHeap.push_back(pair<int, int>(-bdd_var2level(bdd_var(high)), high));
            mark[high] = true;
            push_heap(theHeap.begin(), theHeap.end());
        }
    }
    mpq_clear(temp2);
    mpq_clear(temphalf);
    return count;
}

int get_diff_prob_rec(const bdd r, mpq_t *formula_sat_prob, mpq_t *weight_sat_prob, mpq_t *prob, int *marks, int count)
{
    mpq_t temp, temp2;
        
    if (marks[r.id()])
    {    
		marks[r.id()] = 0; 
		count++;
		mpq_init(temp);
        mpq_init(temp2);
        mpq_div_2exp(temp, weight_sat_prob[r.id()], 1);
        
        if (bdd_high(r).id() != 1 && bdd_high(r).id() != 0) 
			count = get_diff_prob_rec(bdd_high(r), formula_sat_prob, weight_sat_prob, prob, marks, count);
		
		if (bdd_low(r).id() != 1 && bdd_low(r).id() != 0) 
			count = get_diff_prob_rec(bdd_low(r), formula_sat_prob, weight_sat_prob, prob, marks, count);
				
		//gmp_fprintf(stderr, "Number of solutions %d %Qd %Qd %Qd\n", r.id(), temp, formula_sat_prob[bdd_low(r).id()], formula_sat_prob[bdd_high(r).id()]);
		mpq_sub(temp2, formula_sat_prob[bdd_high(r).id()], formula_sat_prob[bdd_low(r).id()]);
		mpq_mul(temp2, temp2, temp);
		//gmp_fprintf(stderr, "Number of solutions %d %d %Qd %Qd %Qd\n", r.id(), bdd_var2level(bdd_var(r)), temp, formula_sat_prob[bdd_low(r).id()], formula_sat_prob[bdd_high(r).id()]);
		mpq_add(prob[bdd_var2level(bdd_var(r))], prob[bdd_var2level(bdd_var(r))], temp2);
		
		mpq_clear(temp);
		mpq_clear(temp2);
	} 
	return count;
   
}//end get_diff_prob_rec

double bdd_get_probability(bdd r, mpq_t *prob_rational)
{
	double numconf;
	mpq_t temp;
	int count = 0;
	
	mpq_t *formula_sat_prob;
	mpq_t *weight_sat_prob;
	
	int *marks = (int *)calloc((bddnodesize), sizeof(int)); 
	
	formula_sat_prob = (mpq_t *)malloc((bddnodesize)*sizeof(mpq_t)); 
	weight_sat_prob = (mpq_t *)malloc((bddnodesize)*sizeof(mpq_t)); 
	
	for (int i = 0; i < bddnodesize; ++i) 
    {
		mpq_init(formula_sat_prob[i]);
		mpq_init(weight_sat_prob[i]);
    }
    
    for (int i = 0; i < bdd_varnum(); ++i) 
    {
		mpq_init(prob_rational[i]);
    }
    
    mpq_init(temp);
    mpq_set_ui(temp, 1, 1);
    	
	count = get_sat_prob_rec(r, formula_sat_prob, marks, count);	
	
	mpq_mul_2exp(temp, temp, bdd_varnum());
	numconf = pow(2,bdd_varnum())*(mpq_get_d(formula_sat_prob[r.id()]));
	
	mpq_set_ui(weight_sat_prob[r.id()], 1, 1); 
	count = get_diff(r, weight_sat_prob, count);
	count = get_diff_prob_rec(r, formula_sat_prob, weight_sat_prob, prob_rational, marks, count);
	
	mpq_init(temp);
	
	for (int i=0; i<bdd_varnum(); i++)
	{
		mpq_div(prob_rational[i], prob_rational[i], formula_sat_prob[r.id()]);
		mpq_set_ui(temp, 1, 1);
		mpq_add(prob_rational[i], temp, prob_rational[i]); 
		mpq_div_2exp(prob_rational[i], prob_rational[i], 1);						
	}
	
	for (int j=0; j<bddnodesize; ++j)
    {
		mpq_clear(formula_sat_prob[j]);
		mpq_clear(weight_sat_prob[j]);
	}
	
	mpq_clear(temp);
	
	free(marks);
    free(formula_sat_prob);
    free(weight_sat_prob);
    
    //cerr << "number of nodes" << count << endl;
    
    return numconf;	
	
}//end bdd_get_probability

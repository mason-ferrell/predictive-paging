/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */


/*int global_lru_out(Pentry q[MAXPROCESSES], int timestamps[MAXPROCESSES][MAXPROCPAGES], int lruTime)
{
    int proctmp;
    int pagetmp;
    int lruProc;
    int lruPage;
    int lastPageUse;
    //printf("ticks initially passed: %d\n", lruTime);

    for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	//printf("proctmp: %d\n", proctmp);
	for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++){
	    lastPageUse = timestamps[proctmp][pagetmp];
	    //printf("lastPageUse: %d\n", lastPageUse);
	    //printf("q.pages: %d\n", (int) q[proctmp].pages[pagetmp]);
	    if(q[proctmp].pages[pagetmp]==1 && lastPageUse <= lruTime)
	    {
		lruTime = lastPageUse;
		lruProc = proctmp;
		lruPage = pagetmp;
		//printf("lruProc: %d\n", lruProc);
	    }
	}
    }
    //printf("%d, %d, %d\n", lruTime, lruProc, lruPage);

    if(pageout(lruProc, lruPage) ) return 1;
    return 0;
}*/

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

typedef struct {
    int prevPg;
    int nextPg[MAXPROCPAGES];
} pageCalls;

typedef struct {
    int proc;
    int page;
} frame;

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static pageCalls procPgPred[MAXPROCESSES];
    //static int freePages = PHYSICALPAGES;
    
    /* Local vars */
    int proctmp;
    int pagetmp;
    int page;
    frame pageInPred[MAXPROCESSES*MAXPROCPAGES];
    int pageInPredSize = 0;
    frame pageOutPred[MAXPROCESSES*MAXPROCPAGES];
    int pageOutPredSize = 0;

    /* initialize static vars on first run */
    if(!initialized){
	/* Init complex static vars here */
	for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++){
		procPgPred[proctmp].nextPg[pagetmp] = MAXPROCPAGES + 1;
	    }
	    procPgPred[proctmp].prevPg = MAXPROCPAGES + 1;
	}
	initialized = 1;
    }
    
    /* TODO: Implement Predictive Paging */

    for(proctmp = 0; proctmp<MAXPROCESSES; proctmp++) {
	if(q[proctmp].active) {
	    page = q[proctmp].pc/PAGESIZE;
	    if(procPgPred[proctmp].prevPg > MAXPROCPAGES)
		procPgPred[proctmp].prevPg = page;
	    if(page!=procPgPred[proctmp].prevPg) {
		int prevPg = procPgPred[proctmp].prevPg;
		procPgPred[proctmp].nextPg[prevPg] = page;
		procPgPred[proctmp].prevPg = page;
	    }
	}
    }

    for(proctmp = 0; proctmp<MAXPROCESSES; proctmp++) {
	if(q[proctmp].active) {
	    page = q[proctmp].pc/PAGESIZE;
	    int nextPage = procPgPred[proctmp].nextPg[page];
	    
	    if(nextPage<=MAXPROCPAGES && !q[proctmp].pages[nextPage]){
		pageInPred[pageInPredSize].proc = proctmp;
		pageInPred[pageInPredSize].page = nextPage;
		pageInPredSize++;
	    }

	    for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++){
		if(q[proctmp].pages[pagetmp] && pagetmp!=page && procPgPred[proctmp].nextPg[page]!=pagetmp){
		    pageOutPred[pageOutPredSize].proc = proctmp;
		    pageOutPred[pageOutPredSize].page = pagetmp;
		    pageOutPredSize++;
		}
	    }
	} 
	else {
	    for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++) {
		if(q[proctmp].pages[pagetmp]){
		    pageOutPred[pageOutPredSize].proc = proctmp;
		    pageOutPred[pageOutPredSize].page = pagetmp;
		    pageOutPredSize++;
		}
	    }
	}
    }

    for(int i = pageOutPredSize - 1; i>=0; i--) {
	if(!pageout(pageOutPred[i].proc, pageOutPred[i].page)) break;
	pageOutPredSize--;
    }

    for(proctmp = 0; proctmp<MAXPROCESSES; proctmp++) {
	if(q[proctmp].active) {
	    page = q[proctmp].pc/PAGESIZE;
	    if(!q[proctmp].pages[page]){
		pagein(proctmp, page);
	    }
	}
    }

    for(int i = pageOutPredSize - 1; i>=0; i--) {
	pageout(pageOutPred[i].proc, pageOutPred[i].page);
    }

    for(int i = pageInPredSize - 1; i>=0; i--) {
	pagein(pageInPred[i].proc, pageInPred[i].page);
    }

    /* advance time for next pageit iteration */
    tick++;
    
} 

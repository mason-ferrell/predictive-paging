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


#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

typedef struct {
    int prevPg;
    int nextPg[MAXPROCPAGES][MAXPROCPAGES];
} pageCalls;

typedef struct {
    int proc;
    int page;
} frame;

int global_lru_out(Pentry q[MAXPROCESSES], int timestamps[MAXPROCESSES][MAXPROCPAGES], int lruTime)
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
}

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static pageCalls procPgPred[MAXPROCESSES];
    //static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    
    /* Local vars */

    int proctmp;
    int pagetmp;
    int page;
    int procType;
    frame pageOutPred[MAXPROCESSES*MAXPROCPAGES];
    int pageOutPredSize = 0;
    frame pageInPred[MAXPROCESSES*MAXPROCPAGES];
    int pageInPredSize = 0;

    /* initialize static vars on first run */
    if(!initialized){
	/* Init complex static vars here */
	for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++){
		for(int j=0; j<MAXPROCPAGES; j++){
		    procPgPred[proctmp].nextPg[pagetmp][j]=0;
		}
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
		++procPgPred[proctmp].nextPg[prevPg][page];
		procPgPred[proctmp].prevPg = page;
	    }
	}
    }
    static int flag[MAXPROCESSES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for(proctmp = 0; proctmp<MAXPROCESSES; proctmp++) {
	if(q[proctmp].active) {
	    page = q[proctmp].pc/PAGESIZE;

	    if(procPgPred[proctmp].nextPg[1535/PAGESIZE][0] || procPgPred[proctmp].nextPg[1402/PAGESIZE][0]) procType = 1;
    	    else if(procPgPred[proctmp].nextPg[1130/PAGESIZE][0]) procType = 2;
    	    else if(procPgPred[proctmp].nextPg[1684/PAGESIZE][1168/PAGESIZE] || procPgPred[proctmp].nextPg[1684/PAGESIZE][0]) procType = 3;
    	    else if(procPgPred[proctmp].nextPg[502/PAGESIZE][0]) procType = 5;
    	    else procType = 4;

	    if(procType!=4 && !flag[proctmp]){
		for(int i=0; i<MAXPROCPAGES; i++) {
		    pageout(proctmp, i);
		}
		flag[proctmp] = 1;
	    }

	    if(procType==1) {
		if(page==0) {pageout(proctmp, 5); pageout(proctmp,6);}
		else if(page==2) {pageout(proctmp, 1); pageout(proctmp, 5);}
		else if(page==5) {pageout(proctmp, 4); pageout(proctmp, 1); pageout(proctmp, 2);}
		else pageout(proctmp, page - 1);

		if(page==1) {pagein(proctmp, 2); pagein(proctmp,5);}
		else if(page==5) {pagein(proctmp,0); pagein(proctmp, 6);}
		else pagein(proctmp, page + 1);
	    }

	    if(procType==2) {
		if(page==0) pageout(proctmp, 4);
		else pageout(proctmp, page - 1);

		if(page==4) pagein(proctmp, 0);
		else pagein(proctmp, page + 1);
	    }

	    if(procType==3) {
		if(page==0) {pageout(proctmp, 4); pageout(proctmp, 6);}
		else if(page==4) {pageout(proctmp, 3); pageout(proctmp, 0); pageout(proctmp, 6);}
		else pageout(proctmp, page - 1);

		if(page==6) {pagein(proctmp, 0); pagein(proctmp, 4);}
		else pagein(proctmp, page + 1);
	    }

	    if(procType==5) {
		if(page==0) {pageout(proctmp, 1); pageout(proctmp, 2);}
		else pageout(proctmp, page - 1);

		if(page==1) pagein(proctmp, 0);
		else pagein(proctmp, page + 1);
	    }

	    if(procType==4) {
		for(int i=0; i<MAXPROCPAGES; i++){
	    	    int nextPage = procPgPred[proctmp].nextPg[page][i];
	    	    if(nextPage<=MAXPROCPAGES && !q[proctmp].pages[nextPage]){
    		        pageInPred[pageInPredSize].proc = proctmp;
		        pageInPred[pageInPredSize].page = nextPage;
		        pageInPredSize++;
	    	    }
		    else if(nextPage > MAXPROCPAGES) break;
	        }

	        for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++){
		    int pageOut = 1;
		    for(int i=0; i<MAXPROCPAGES; i++) {
		        if(!q[proctmp].pages[pagetmp] || pagetmp==page 
		          || procPgPred[proctmp].nextPg[page][i]==pagetmp) pageOut = 0;
		    }

		    if(pageOut==1){
		        pageOutPred[pageOutPredSize].proc = proctmp;
		        pageOutPred[pageOutPredSize].page = pagetmp;
		        pageOutPredSize++;
		    }
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
		if(!pagein(proctmp, page) && pageOutPredSize>0){
		    pageout(pageOutPred[pageOutPredSize-1].proc, pageOutPred[pageOutPredSize-1].page);
		    pageOutPredSize--;
		}
	    }
	}
    }

    for(int i = pageOutPredSize - 1; i>=0; i--) {
	pageout(pageOutPred[i].proc, pageOutPred[i].page);
    }

    for(int i = pageInPredSize - 1; i>=0; i--) {
	if(!pagein(pageInPred[i].proc, pageInPred[i].page) && pageOutPredSize>0){
	    pageout(pageOutPred[pageOutPredSize-1].proc, pageOutPred[pageOutPredSize-1].page);
	    pageOutPredSize--;
	}
    }

    /* advance time for next pageit iteration */
    tick++;
    
} 

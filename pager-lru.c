/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;
    int page;
    int lruPage;
    int lruTime = tick;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }
    
    /* TODO: Implement LRU Paging */


    for(proctmp = 0; proctmp<MAXPROCESSES; proctmp++) {

	if(q[proctmp].active) {
	    page = q[proctmp].pc/PAGESIZE;
	    if(!q[proctmp].pages[page]) {
		if(!pagein(proctmp, page)) {
		    for(int i=0; i<MAXPROCESSES; i++){
			for(int j=0; j<MAXPROCPAGES; j++) {
  			    if(timestamps[i][j] <= lruTime
			      && q[i].pages[j]==1) {
			        lruTime = timestamps[i][j];
			        lruPage = j;
			    }
			}
		    }
		    pageout(proctmp, lruPage);
		}
	    }
	}
    }

    for(proctmp = 0; proctmp<MAXPROCESSES; proctmp++) {
	if(q[proctmp].active){
	    page = q[proctmp].pc/PAGESIZE;
	    timestamps[proctmp][page] = tick;
	}
    }


    /* advance time for next pageit iteration */
    tick++;
} 

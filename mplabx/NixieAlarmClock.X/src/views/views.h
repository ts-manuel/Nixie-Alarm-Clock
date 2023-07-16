/* 
 * File:   views.h
 * Author: ts-manuel
 *
 * 
 */

#ifndef _VIEWS_VIEWS_H_
#define	_VIEWS_VIEWS_H_


typedef enum {e_VIEW_HOME, e_VIEW_SET_TIME, e_VIEW_SET_ALARM} View_t;


void VIEWS_Initialize(void);

void VIEWS_Update(void);

#endif	/* _VIEWS_VIEWS_H_ */


/*
 * GenericPro.h
 *
 *  Created on: 2016年12月16日
 *      Author: Administrator
 */

#ifndef GENERICPRO_H_
#define GENERICPRO_H_

#include "CommonType.h"

#define CATCHER_MAX 10
#define CATCHER_ERROR CATCHER_MAX+1

typedef struct {
	char use,bRecvHeader;
	int cnt;
}PROCAT;		//捕捉器,状态,初始化为0

typedef struct {
	const char *header;				//包头
	const char headerSize,cath_max;	//包头大小,捕捉器非零缓存区个数
	const int minSize,maxSize;		//信息最小长度,完整包最大长度
	int (*getLength)(char *,int);	//获取长度信息
	void (*callBack)(char *,int);	//回调函数
	char (*check)(char *,int);		//校验
	char *buf[CATCHER_MAX];			//捕捉器个数,减少丢包
	PROCAT state[CATCHER_MAX];		//捕捉器状态
}GENPRO;

extern BOOL Protocol_Init(GENPRO *);
extern void Protocol_AddBytes(GENPRO *,char *,int); //字节接收

#endif /* GENERICPRO_H_ */

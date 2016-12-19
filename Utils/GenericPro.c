/*
 * GenericPro.c
 *
 *  Created on: 2016年12月16日
 *      Author: Administrator
 *  一般协议捕捉,不进行拼包
 *  协议中含有:包头+长度,则可以使用该函数
 */
#include "GenericPro.h"
#include <memory.h>

static BOOL mock_check(char *buf,int size){
	return 1;
}

static int getFreeSlot(GENPRO *pro){
	int i=0;
	for(;i<pro->cath_max;i++){
		if(!pro->state[i].use)
			return i;
	}
	return CATCHER_ERROR;
}

static char isRecvHeaderOK(GENPRO *pro,int index){
	int i=0,len;
	for(;i<pro->headerSize;i++)
		if(pro->buf[index][i]!=pro->header[i])
			return 0;
	len=pro->getLength(pro->buf[index],pro->state[index].cnt);
	if(len>pro->maxSize||len<0)
			return 0;
	return 1;
}

static void add(GENPRO *pro,char *recv,int size){
	int i=0,j,tmp;
	for(;i<size;i++){
		for(j=0;j<pro->cath_max;j++){
			if(pro->state[j].use&&pro->state[j].cnt<pro->maxSize){	//填充缓冲区
				pro->buf[j][pro->state[j].cnt]=recv[i];
				pro->state[j].cnt++;
			}
		}
		if(recv[i]==pro->header[0]){							//首字节相同,使能缓冲区
			if((tmp=getFreeSlot(pro))!=CATCHER_ERROR){
				pro->state[tmp].use=1;
				pro->state[tmp].cnt=1;
				pro->buf[tmp][0]=recv[i];
				pro->state[i].bRecvHeader=0;
			}
		}
	}
}

static void header(GENPRO *pro){
	int i=0;
	for(i=0;i<pro->cath_max;i++){
		if(!pro->state[i].use)				//无效缓冲,不检测
			continue;
		if(pro->state[i].cnt<pro->minSize)	//过小,不检测
			continue;
		if(pro->state[i].bRecvHeader)		//已经检测 ,不检测
			continue;
		if(!isRecvHeaderOK(pro,i)){			//检测
			pro->state[i].use=0;
			continue;
		}else{
			pro->state[i].bRecvHeader=1;
		}
	}
}

static void complete(GENPRO *pro){
	int i=0,length;
	for(i=0;i<pro->cath_max;i++){
		if(!pro->state[i].use)
			continue;
		if(!pro->state[i].bRecvHeader)
			continue;
		length=pro->getLength(pro->buf[i],pro->state[i].cnt);	//缓冲长度OK
		if(pro->state[i].cnt<length)
			continue;
		if(pro->check(pro->buf[i],pro->state[i].cnt))			//校验OK,则回调
			pro->callBack(pro->buf[i],length);
		pro->state[i].use=0;									//缓冲无效
	}
}

BOOL Protocol_Init(GENPRO *pro){
	if(pro->getLength==NULL||pro->callBack==NULL)
		return FALSE;
	if(pro->check==NULL)
		pro->check=mock_check;
	memset(&pro->state,0,sizeof(pro->state));
}

void Protocol_AddBytes(GENPRO *pro,char *recv,int size){
	add(pro,recv,size);
	header(pro);
	complete(pro);
}


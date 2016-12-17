/*
 * Demo_GenericPro.c
 *
 *  Created on: 2016年12月17日
 *      Author: Administrator
 */

#include "../Utils/GenericPro.h"

const static char debugSendBuf[]="AABB0020AAAA001356688";

static char recvBuf[CATCHER_MAX][1024]={0};

static int debugGetLength(char * buf,int size){
	int i=0,ret;
	ret=0;
	for(i=4;i<8;i++){
		ret*=10;
		ret+=(buf[i]-0x30);
	}
	return ret;
}

static char debugCheck(char *buf,int size){
	if(buf[size-1]=='8')
		return 1;
	else{
		return 0;
	}

}

static void debugCallBack(char *buf,int size){
	buf[size]=0;
	Log("recvOK",buf);
}


GENPRO myDebugPro={
		.header="AA",
		.headerSize=2,
		.cath_max=10,
		.minSize=8,
		.maxSize=100,
		.getLength=debugGetLength,
		.callBack=debugCallBack,
		.check=debugCheck,
};

void Demo_GenericPro(){
	int i=0,n;
	char tmp;
	Protocol_Init(&myDebugPro);
	for(i=0,n=0;i<10000;i++){
		if(n>sizeof(debugSendBuf))
			n=0;
		tmp=debugSendBuf[n++];
		Protocol_AddBytes(&myDebugPro,&tmp,1);
	}
}


#include "stdafx.h"
#include "AiBytes.h"


AiBytes::AiBytes(){  
	d=0;  
	len=0;  
}  

AiBytes::~AiBytes(){  
	delete[] d;  
}  

void AiBytes::assign(char* p,int l){  
	if(d){  
		delete[] d;  
		len=0;  
	}  
	d=new char[l];  
	len=l;  
	memcpy(d,p,l);  
}  

void AiBytes::add(char ch){  
	len++;  

	char* t=new char[len];  
	if(d){  
		memcpy(t,d,len-1);  
	}  
	t[len-1]=ch;  
	delete []d;  
	d=t;  

}  
void AiBytes::add(char* s,int l){  
	len+=l;  

	char* t=new char[len];  
	if(d){  
		memcpy(t,d,len-l);  
	}  
	memcpy(t+len-l,s,l);  
	delete []d;  
	d=t;  
}  
int AiBytes::remove(int s,int l){  
	if(s+l>len){  
		return 0;  
	}  
	char* t=new char[len-l];  
	int i1=0;  
	for(int i=0;i1<len-l;i++){  
		if(i<s || i>s+l-1){  
			t[i1++]=d[i];  
		}  
	}  
	delete[] d;  
	d=t;  
	len=len-l;  
	return 1;  
}  

char* AiBytes::data(){  
	return d;  
}  

int AiBytes::getlen(){  
	return len;  
}  

char* AiBytes::find(int s,char* str,int l){  
	if(s+l>len){  
		return 0;  
	}  
	char* t=d+s;  
	for(int i=0;i<=len-l;i++){  
		int flag=0;  
		for(int i1=0;i1<l;i1++){  
			if(t[i+i1]!=str[i1]){  
				flag=1;  
				break;  
			}  
		}  
		if(flag==0){  
			return t+i;  
		}  
	}  
	return 0;  
}  
char* AiBytes::find(int s,char ch){  
	if(s+1>len){  
		return 0;  
	}  
	char* t=d+s;  
	for(int i=0;i<len;i++){  
		if(t[i]==ch){  
			return t+i;  
		}  
	}  
	return 0;  
}  
void AiBytes::clear(){  
	delete[] d;  
	d=0;  
	len=0;  
}  
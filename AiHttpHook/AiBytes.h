#include <iostream>  
using namespace std;  


class AiBytes{  
public:  
	AiBytes();  
	~AiBytes();  
	void assign(char*,int);  
	void add(char*,int);  
	void add(char);  
	int remove(int,int);  
	void clear();  
	char* find(int,char*,int);  
	char* find(int,char);  
	int getlen();  
	char* data();  
private:  
	char *d;  
	int len;  
};  



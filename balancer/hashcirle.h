#include "hashnode.h"
#include "md5.h"
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
using namespace std;
class HashCon
{
	private:map<string,HaNode*> HaC;
		   	vector< HaNode*> HacNode;
	public:
		string Md5Work(const string &str);
		void AddHaNode(int fd, string ip);
		void DelHaNode(int fd);
		void AddVirtualNd(string str, HaNode* node);
		HaNode* GetServer(string ip);
			
};
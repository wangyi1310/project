#ifndef _Hash_Node_
#define _Hash_Node_
#include <string>
using namespace std;
struct HaNode
{
		HaNode(string ip,int fd):mfd(fd),mIp(ip)
		{
			mVirNodecount=0;
		}
		void setNodefd(int fd)
		{
			mfd=fd;
		}
		void addVN()
		{
				mVirNodecount++;
		}
		void ResVN()
		{
				mVirNodecount--;
		}
		int mfd;
		int mVirNodecount;
		string mIp;
};
#endif

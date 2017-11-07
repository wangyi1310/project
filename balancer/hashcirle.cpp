#include "hashcirle.h"
string HashCon::Md5Work(const string &str)
{
	MD5 md5(str);
	return string(md5.md5());
}
void HashCon::AddHaNode(int fd, string ip)
{
	HaNode *node = new HaNode(ip, fd);
	HacNode.push_back(node);
	string str = Md5Work(ip);
	HaC.insert(make_pair(str, node));
	AddVirtualNd(ip, node);

}
void HashCon::DelHaNode(int fd)
{
	vector<HaNode*>::iterator it = HacNode.begin();
	for (; it != HacNode.end(); it++)
	{
		if ((*it)->mfd==fd)
		{
			HacNode.erase(it);
			break;
		}
	}
	map<string, HaNode*>::iterator it1 = HaC.begin();
	for (; it1 != HaC.end();)
	{
		if(it1->second->mfd == fd)
		{
			HaC.erase(it1++);
		}
		else
		{
			it1++;
		}
	}
}
void HashCon::AddVirtualNd(string str, HaNode* node)
{
	map<string, HaNode*>::iterator it1 = HaC.begin();
	for (int i = 0; i < 3; i++)
	{
		str += 1;
		string str1 = Md5Work(str.c_str());
		HaC.insert(make_pair(str1, node));
	}
}
HaNode*HashCon::GetServer(string ip)
{
	string str = Md5Work(ip.c_str());
	map<string, HaNode*>::iterator it1 = HaC.upper_bound(str);
	if (it1 != HaC.end())
	{
		return it1->second;
	}
	it1 = HaC.begin();
	return it1->second;
}

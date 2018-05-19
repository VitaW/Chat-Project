#ifndef VIEW_H
#define VIEW_H

class View
{
	public:
		virtual void process(int fd,char* json){ }
		virtual void response(){ }
	private:
		int _fd;
		const char* _str;
};
#endif

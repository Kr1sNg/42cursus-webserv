#ifndef _CGIPHANDLER_HPP_
# define _CGIPHANDLER_HPP_

#include "../webserv.hpp"
#include "Connection.hpp"

class Connection;

enum PipeType
{
    PIPE_READ,
    PIPE_WRITE
};

class CGIPipeHandler: public IEventHandler
{
    private:
        Connection  *_connection;
        int         _pipeFd;
        PipeType    _type;

        CGIPipeHandler();
        CGIPipeHandler(const CGIPipeHandler &src);
        CGIPipeHandler  &operator=(const CGIPipeHandler &rhs);

    public:
        CGIPipeHandler(Connection *conn, int fd, PipeType type);
        virtual ~CGIPipeHandler();

        virtual int     getFd(void) const;
        virtual void    handleEvent(uint32_t events);

};



#endif
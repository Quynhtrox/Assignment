#ifndef __Q_IOCTL_H__
#define __Q_IOCTL_H__

#include <linux/ioctl.h>

#define Q_MAGIC             'Q'
#define Q_GET_BUFFER_SIZE   _IOR(Q_MAGIC, 1, int)
#define Q_CLEAR_BUFFER      _IO(Q_MAGIC, 0)

#endif // __Q_IOCTL_H__
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake spec of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLATFORMDEFS_H
#define QPLATFORMDEFS_H

// Get Qt defines/settings

#include "qglobal.h"

// Set any POSIX/XOPEN defines at the top of this file to turn on specific APIs

#include <unistd.h>


// We are hot - unistd.h should have turned on the specific APIs we requested


#ifndef QT_NO_THREAD
#include <pthread.h>
#endif
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#ifndef QT_NO_IPV6IFNAME
#include <net/if.h>
#endif

#define QT_NO_USE_FSEEKO
#include "../common/posix/qplatformdefs.h"

#undef QT_OFF_T
#undef QT_SOCKLEN_T

#define QT_OFF_T                off_t

#if defined(_POSIX_PII_SOCKET)
#define QT_SOCKLEN_T            socklen_t
#elif defined(_XOPEN_SOURCE_EXTENDED)
#define QT_SOCKLEN_T            size_t
#else
#define QT_SOCKLEN_T            int
#endif

#if defined(_XOPEN_SOURCE) && defined(_OSF_SOURCE)
// Not available in the <unistd.h> header file of Tru64 4.0F.
// Fixed in the <unistd.h> header of Tru64 5.0A so we copy/paste from there...
extern "C" int usleep(useconds_t);
#endif

#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE-0 >= 400)
// Tru64 5.0 and better
#define QT_SNPRINTF		::snprintf
#define QT_VSNPRINTF		::vsnprintf
#endif

#endif // QPLATFORMDEFS_H

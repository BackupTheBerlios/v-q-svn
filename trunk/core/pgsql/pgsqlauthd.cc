/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2004 Pawel Niewiadomski (new-baz.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "vq_conf.h"
#include "sig.h"
#include "fd.h"
#include "fdstr.h"
#include "util.h"
#include "lock.h"
#include "pgsqlcommon.h"
#include "sys.h"
#include "cvq.h"
#include "cdaemonauth.h"
#include "cdaemonchild.h"
#include "cdaemonmaster.h"
#include "lower.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ctime>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>


using namespace std;
using namespace vq;



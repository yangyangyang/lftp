/*
 * lftp - file transfer program
 *
 * Copyright (c) 1996-2012 by Alexander V. Lukyanov (lav@yars.free.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Id: buffer_ssl.h,v 1.10 2008/11/27 05:56:34 lav Exp $ */

#ifndef BUFFER_SSL_H
#define BUFFER_SSL_H

#include "buffer.h"

#if USE_SSL
#include "lftp_ssl.h"
class IOBufferSSL : public IOBuffer
{
   Ref<lftp_ssl> my_ssl;
   const Ref<lftp_ssl>& ssl;

   int Get_LL(int size);
   int Put_LL(const char *buf,int size);
   int PutEOF_LL();

public:
   IOBufferSSL(lftp_ssl *s,dir_t m) : IOBuffer(m), my_ssl(s), ssl(my_ssl) {}
   IOBufferSSL(const Ref<lftp_ssl>& s,dir_t m) : IOBuffer(m), ssl(s) {}
   ~IOBufferSSL();
   int Do();
   bool Done() { return IOBuffer::Done() && ssl->handshake_done; }
};
#endif

#endif//BUFFER_SSL_H

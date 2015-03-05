/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* the following corresponds to the error codes of the Linux kernel used by the Android platform
 * these are distinct from the OpenBSD ones, which is why we need to redeclare them here
 *
 * this file may be included several times to define either error constants or their
 * string representation
 */

#ifndef __BIONIC_ERRDEF
#error "__BIONIC_ERRDEF must be defined before including this file"
#endif
__BIONIC_ERRDEF( EPERM          , EPERM          , "Operation not permitted" )
__BIONIC_ERRDEF( ENOENT         , ENOENT         , "No such file or directory" )
__BIONIC_ERRDEF( ESRCH          , ESRCH          , "No such process" )
__BIONIC_ERRDEF( EINTR          , EINTR          , "Interrupted system call" )
__BIONIC_ERRDEF( EIO            , EIO            , "I/O error" )
__BIONIC_ERRDEF( ENXIO          , ENXIO          , "No such device or address" )
__BIONIC_ERRDEF( E2BIG          , E2BIG          , "Argument list too long" )
__BIONIC_ERRDEF( ENOEXEC        , ENOEXEC        , "Exec format error" )
__BIONIC_ERRDEF( EBADF          , EBADF          , "Bad file number" )
__BIONIC_ERRDEF( ECHILD         , ECHILD         , "No child processes" )
__BIONIC_ERRDEF( EAGAIN         , EAGAIN         , "Try again" )
__BIONIC_ERRDEF( ENOMEM         , ENOMEM         , "Out of memory" )
__BIONIC_ERRDEF( EACCES         , EACCES         , "Permission denied" )
__BIONIC_ERRDEF( EFAULT         , EFAULT         , "Bad address" )
__BIONIC_ERRDEF( EBUSY          , EBUSY          , "Device or resource busy" )
__BIONIC_ERRDEF( EEXIST         , EEXIST         , "File exists" )
__BIONIC_ERRDEF( EXDEV          , EXDEV          , "Cross-device link" )
__BIONIC_ERRDEF( ENODEV         , ENODEV         , "No such device" )
__BIONIC_ERRDEF( ENOTDIR        , ENOTDIR        , "Not a directory" )
__BIONIC_ERRDEF( EISDIR         , EISDIR         , "Is a directory" )
__BIONIC_ERRDEF( EINVAL         , EINVAL         , "Invalid argument" )
__BIONIC_ERRDEF( ENFILE         , ENFILE         , "File table overflow" )
__BIONIC_ERRDEF( EMFILE         , EMFILE         , "Too many open files" )
__BIONIC_ERRDEF( ENOTTY         , ENOTTY         , "Not a typewriter" )
__BIONIC_ERRDEF( ETXTBSY        , ETXTBSY        , "Text file busy" )
__BIONIC_ERRDEF( EFBIG          , EFBIG          , "File too large" )
__BIONIC_ERRDEF( ENOSPC         , ENOSPC         , "No space left on device" )
__BIONIC_ERRDEF( ESPIPE         , ESPIPE         , "Illegal seek" )
__BIONIC_ERRDEF( EROFS          , EROFS          , "Read-only file system" )
__BIONIC_ERRDEF( EMLINK         , EMLINK         , "Too many links" )
__BIONIC_ERRDEF( EPIPE          , EPIPE          , "Broken pipe" )
__BIONIC_ERRDEF( EDOM           , EDOM           , "Math argument out of domain of func" )
__BIONIC_ERRDEF( ERANGE         , ERANGE         , "Math result not representable" )
__BIONIC_ERRDEF( EDEADLK        , EDEADLK        , "Resource deadlock would occur" )
__BIONIC_ERRDEF( ENAMETOOLONG   , ENAMETOOLONG   , "File name too long" )
__BIONIC_ERRDEF( ENOLCK         , ENOLCK         , "No record locks available" )
__BIONIC_ERRDEF( ENOSYS         , ENOSYS         , "Function not implemented" )
__BIONIC_ERRDEF( ENOTEMPTY      , ENOTEMPTY      , "Directory not empty" )
__BIONIC_ERRDEF( ELOOP          , ELOOP          , "Too many symbolic links encountered" )
__BIONIC_ERRDEF( ENOMSG         , ENOMSG         , "No message of desired type" )
__BIONIC_ERRDEF( EIDRM          , EIDRM          , "Identifier removed" )
__BIONIC_ERRDEF( ENOSTR         , ENOSTR         , "Device not a stream" )
__BIONIC_ERRDEF( ENODATA        , ENODATA        , "No data available" )
__BIONIC_ERRDEF( ETIME          , ETIME          , "Timer expired" )
__BIONIC_ERRDEF( ENOSR          , ENOSR          , "Out of streams resources" )
__BIONIC_ERRDEF( ENOLINK        , ENOLINK        , "Link has been severed" )
__BIONIC_ERRDEF( EPROTO         , EPROTO         , "Protocol error" )
__BIONIC_ERRDEF( EMULTIHOP      , EMULTIHOP      , "Multihop attempted" )
__BIONIC_ERRDEF( EBADMSG        , EBADMSG        , "Not a data message" )
__BIONIC_ERRDEF( EOVERFLOW      , EOVERFLOW      , "Value too large for defined data type" )
__BIONIC_ERRDEF( EILSEQ         , EILSEQ         , "Illegal byte sequence" )
__BIONIC_ERRDEF( ENOTSOCK       , ENOTSOCK       , "Socket operation on non-socket" )
__BIONIC_ERRDEF( EDESTADDRREQ   , EDESTADDRREQ   , "Destination address required" )
__BIONIC_ERRDEF( EMSGSIZE       , EMSGSIZE       , "Message too long" )
__BIONIC_ERRDEF( EPROTOTYPE     , EPROTOTYPE     , "Protocol wrong type for socket" )
__BIONIC_ERRDEF( ENOPROTOOPT    , ENOPROTOOPT    , "Protocol not available" )
__BIONIC_ERRDEF( EPROTONOSUPPORT, EPROTONOSUPPORT, "Protocol not supported" )
__BIONIC_ERRDEF( EOPNOTSUPP     , EOPNOTSUPP     , "Operation not supported on transport endpoint" )
__BIONIC_ERRDEF( EAFNOSUPPORT   , EAFNOSUPPORT   , "Address family not supported by protocol" )
__BIONIC_ERRDEF( EADDRINUSE     , EADDRINUSE     , "Address already in use" )
__BIONIC_ERRDEF( EADDRNOTAVAIL  , EADDRNOTAVAIL  , "Cannot assign requested address" )
__BIONIC_ERRDEF( ENETDOWN       , ENETDOWN       , "Network is down" )
__BIONIC_ERRDEF( ENETUNREACH    , ENETUNREACH    , "Network is unreachable" )
__BIONIC_ERRDEF( ENETRESET      , ENETRESET      , "Network dropped connection because of reset" )
__BIONIC_ERRDEF( ECONNABORTED   , ECONNABORTED   , "Software caused connection abort" )
__BIONIC_ERRDEF( ECONNRESET     , ECONNRESET     , "Connection reset by peer" )
__BIONIC_ERRDEF( ENOBUFS        , ENOBUFS        , "No buffer space available" )
__BIONIC_ERRDEF( EISCONN        , EISCONN        , "Transport endpoint is already connected" )
__BIONIC_ERRDEF( ENOTCONN       , ENOTCONN       , "Transport endpoint is not connected" )
__BIONIC_ERRDEF( ETIMEDOUT      , ETIMEDOUT      , "Connection timed out" )
__BIONIC_ERRDEF( ECONNREFUSED   , ECONNREFUSED   , "Connection refused" )
__BIONIC_ERRDEF( EHOSTUNREACH   , EHOSTUNREACH   , "No route to host" )
__BIONIC_ERRDEF( EALREADY       , EALREADY       , "Operation already in progress" )
__BIONIC_ERRDEF( EINPROGRESS    , EINPROGRESS    , "Operation now in progress" )
__BIONIC_ERRDEF( ESTALE         , ESTALE         , "Stale NFS file handle" )
__BIONIC_ERRDEF( EDQUOT         , EDQUOT         , "Quota exceeded" )
__BIONIC_ERRDEF( ECANCELED      , ECANCELED      , "Operation Canceled" )

/* the following is not defined by Linux but needed for the BSD portions of the C library */
__BIONIC_ERRDEF( EFTYPE, 1000, "Stupid C library hack !!" )

#undef __BIONIC_ERRDEF

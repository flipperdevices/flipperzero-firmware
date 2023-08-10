/* wolfSSL.cs
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace wolfSSL.CSharp {
    public class wolfssl
    {
        private const string wolfssl_dll = "wolfssl.dll";

        /* wait for 6 seconds default on TCP socket state poll if timeout not set */
        private const int WC_WAIT = 6000000;

        /********************************
         * Class for DTLS connections
         */
        /// <summary>
        /// Contains information regarding a DTLS connection having UdpClient udp and IPEndPoint ep.
        /// Used to keep memory alive.
        /// </summary>
        public class DTLS_con
        {
            public UdpClient udp;
            public IPEndPoint ep;
        }


        /********************************
         * Class for keeping ctx handles alive
         */
        [StructLayout(LayoutKind.Sequential)]
        private class ctx_handle
        {
            private GCHandle rec_cb;
            private GCHandle snd_cb;
            private GCHandle psk_cb;
            private GCHandle vrf_cb;
            private IntPtr ctx;

            public void set_receive(GCHandle input)
            {
                this.rec_cb = input;
            }
            public GCHandle get_receive()
            {
                return this.rec_cb;
            }

            public void set_send(GCHandle input)
            {
                this.snd_cb = input;
            }
            public GCHandle get_send()
            {
                return this.snd_cb;
            }

            public void set_psk(GCHandle input)
            {
                this.psk_cb = input;
            }
            public GCHandle get_psk()
            {
                return this.psk_cb;
            }

            public void set_vrf(GCHandle input)
            {
                if (!Object.Equals(this.vrf_cb, default(GCHandle)))
                {
                    this.vrf_cb.Free();
                }
                this.vrf_cb = input;
            }
            public GCHandle get_vrf()
            {
                return this.vrf_cb;
            }

            public void set_ctx(IntPtr input)
            {
                this.ctx = input;
            }
            public IntPtr get_ctx()
            {
                return this.ctx;
            }

            /// <summary>
            /// Called to free the pointers keeping handles alive
            /// </summary>
            public void free()
            {
                log(INFO_LOG, "freeing ctx handle");
                if (!Object.Equals(this.rec_cb, default(GCHandle)))
                {
                    this.rec_cb.Free();
                }
                if (!Object.Equals(this.snd_cb, default(GCHandle)))
                {
                    this.snd_cb.Free();
                }
                if (!Object.Equals(this.psk_cb, default(GCHandle)))
                {
                    this.psk_cb.Free();
                }
                if (!Object.Equals(this.vrf_cb, default(GCHandle)))
                {
                    this.vrf_cb.Free();
                }
            }
        }

        /********************************
         * Class for keeping ssl handle alive
         */
        [StructLayout(LayoutKind.Sequential)]
        private class ssl_handle
        {
            private GCHandle fd_pin;
            private GCHandle psk_cb;
            private GCHandle vrf_cb;
            private IntPtr ssl;

            public void set_fd(GCHandle input)
            {
                this.fd_pin = input;
            }
            public GCHandle get_fd()
            {
                return this.fd_pin;
            }

            public void set_psk(GCHandle input)
            {
                this.psk_cb = input;
            }
            public GCHandle get_psk()
            {
                return this.psk_cb;
            }

            public void set_vrf(GCHandle input)
            {
                if (!Object.Equals(this.vrf_cb, default(GCHandle)))
                {
                    this.vrf_cb.Free();
                }
                this.vrf_cb = input;
            }
            public GCHandle get_vrf()
            {
                return this.vrf_cb;
            }

            public void set_ssl(IntPtr input)
            {
                this.ssl = input;
            }
            public IntPtr get_ssl()
            {
                return this.ssl;
            }
            public void free()
            {
                log(INFO_LOG, "freeing ssl handle");

                if (!Object.Equals(this.fd_pin, default(GCHandle)))
                {
                    this.fd_pin.Free();
                }
                if (!Object.Equals(this.psk_cb, default(GCHandle)))
                {
                    this.psk_cb.Free();
                }
                if (!Object.Equals(this.vrf_cb, default(GCHandle)))
                {
                    this.vrf_cb.Free();
                }
            }
        }


        /********************************
         * Init wolfSSL library
         */
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_Init();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_Cleanup();


        /********************************
         * Methods of connection
         */
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfTLSv1_2_server_method();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfTLSv1_3_server_method();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSLv23_server_method();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfTLSv1_2_client_method();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfTLSv1_3_client_method();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSLv23_client_method();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfDTLSv1_2_server_method();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfDTLSv1_2_client_method();


        /********************************
         * Call backs
         */
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int CallbackIORecv_delegate(IntPtr ssl, IntPtr buf, int sz, IntPtr ctx);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_SetIORecv(IntPtr ctx, CallbackIORecv_delegate recv);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_SetIOReadCtx(IntPtr ssl, IntPtr rctx);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_GetIOReadCtx(IntPtr ssl);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int CallbackIOSend_delegate(IntPtr ssl, IntPtr buf, int sz, IntPtr ctx);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_SetIOSend(IntPtr ctx, CallbackIOSend_delegate send);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_SetIOWriteCtx(IntPtr ssl, IntPtr wctx);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_GetIOWriteCtx(IntPtr ssl);


        /********************************
         * CTX structure
         */
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_CTX_new(IntPtr method);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_use_certificate_file(IntPtr ctx, string file, int type);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_load_verify_locations(IntPtr ctx, string file, string path);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_use_PrivateKey_file(IntPtr ctx, string file, int type);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_CTX_free(IntPtr ctx);



        /********************************
         * PSK
         */
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate uint psk_delegate(IntPtr ssl, string identity, IntPtr key, uint max_sz);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate uint psk_client_delegate(IntPtr ssl, string hint, IntPtr identity, uint id_max_len, IntPtr key, uint max_sz);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_set_psk_server_callback(IntPtr ssl, psk_delegate psk_cb);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_CTX_set_psk_server_callback(IntPtr ctx, psk_delegate psk_cb);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_CTX_set_psk_client_callback(IntPtr ctx, psk_client_delegate psk_cb);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_use_psk_identity_hint(IntPtr ctx, StringBuilder identity);


        /********************************
         * SSL Structure
         */
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_new(IntPtr ctx);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_accept(IntPtr ssl);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_connect(IntPtr ssl);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_read(IntPtr ssl, IntPtr buf, int sz);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_write(IntPtr ssl, IntPtr buf, int sz);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_shutdown(IntPtr ssl);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_free(IntPtr ssl);


        /********************************
         * Cipher lists
         */
        /* only supports full name from cipher_name[] delimited by : */
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_set_cipher_list(IntPtr ctx, StringBuilder ciphers);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_set_cipher_list(IntPtr ssl, StringBuilder ciphers);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_get_ciphers(StringBuilder ciphers, int sz);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_get_cipher(IntPtr ssl);
        [DllImport(wolfssl_dll, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_CIPHER_get_name(IntPtr cipher);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_get_current_cipher(IntPtr ssl);
        [DllImport(wolfssl_dll, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_get_version(IntPtr ssl);
        [DllImport(wolfssl_dll, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_get_cipher_list(IntPtr ssl);


        /********************************
         * Error logging
         */
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private extern static IntPtr wolfSSL_ERR_error_string(uint err, StringBuilder errOut);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_get_error(IntPtr ssl, int err);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void loggingCb(int lvl, StringBuilder msg);
        private static loggingCb internal_log;
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_Debugging_ON();
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_Debugging_OFF();


        /********************************
         * DH
         */
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_SetMinDhKey_Sz(IntPtr ctx, short size);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_SetTmpDH_file(IntPtr ssl, StringBuilder dhParam, int type);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_CTX_SetTmpDH_file(IntPtr ctx, StringBuilder dhParam, int type);


        /********************************
         * Verify Callback
         */
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int CallbackVerify_delegate(int ret, IntPtr x509_ctx);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_CTX_set_verify(IntPtr ctx, int mode, CallbackVerify_delegate vc);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_set_verify(IntPtr ssl, int mode, CallbackVerify_delegate vc);


        /********************************
         * X509 Store
         */
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_X509_STORE_CTX_get_current_cert(IntPtr x509Ctx);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_X509_STORE_CTX_get_error(IntPtr sk);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_X509_STORE_GetCerts(IntPtr x509Ctx);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static int wolfSSL_sk_X509_num(IntPtr sk);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static void wolfSSL_sk_X509_free(IntPtr sk);
        [DllImport(wolfssl_dll, CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr wolfSSL_sk_X509_pop(IntPtr sk);


        /********************************
         * Enum types from wolfSSL library
         */
        public static readonly int SSL_FILETYPE_PEM = 1;
        public static readonly int SSL_FILETYPE_ASN1 = 2;
        public static readonly int SSL_FILETYPE_RAW = 3;

        public static readonly int SSL_VERIFY_NONE = 0;
        public static readonly int SSL_VERIFY_PEER = 1;
        public static readonly int SSL_VERIFY_FAIL_IF_NO_PEER_CERT = 2;
        public static readonly int SSL_VERIFY_CLIENT_ONCE = 4;
        public static readonly int SSL_VERIFY_POST_HANDSHAKE = 8;
        public static readonly int SSL_VERIFY_FAIL_EXCEPT_PSK = 16;

        public static readonly int CBIO_ERR_GENERAL = -1;
        public static readonly int CBIO_ERR_WANT_READ = -2;
        public static readonly int CBIO_ERR_WANT_WRITE = -2;
        public static readonly int CBIO_ERR_CONN_RST = -3;
        public static readonly int CBIO_ERR_ISR = -4;
        public static readonly int CBIO_ERR_CONN_CLOSE = -5;
        public static readonly int CBIO_ERR_TIMEOUT = -6;

        public static readonly int ERROR_LOG = 0;
        public static readonly int INFO_LOG = 1;
        public static readonly int ENTER_LOG = 2;
        public static readonly int LEAVE_LOG = 3;
        public static readonly int OTHER_LOG = 4;

        public static readonly int SUCCESS = 1;
        public static readonly int FAILURE = 0;


        private static IntPtr unwrap_ctx(IntPtr ctx)
        {
            try {
                GCHandle gch = GCHandle.FromIntPtr(ctx);
                ctx_handle handles = (ctx_handle)gch.Target;
                return handles.get_ctx();
            } catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl ctx pointer is incorrect " + e);
                return IntPtr.Zero;
            }
        }
        private static IntPtr unwrap_ssl(IntPtr ssl)
        {
            try {
                GCHandle gch = GCHandle.FromIntPtr(ssl);
                ssl_handle handles = (ssl_handle)gch.Target;
                return handles.get_ssl();
            } catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl pointer is incorrect " + e);
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Call back to allow receiving TLS information
        /// </summary>
        /// <param name="ssl">structure of ssl passed in</param>
        /// <param name="buf">buffer to contain received msg</param>
        /// <param name="sz">size of buffer</param>
        /// <param name="ctx">optional information passed in</param>
        /// <returns>size of message received</returns>
        private static int wolfSSLCbIORecv(IntPtr ssl, IntPtr buf, int sz, IntPtr ctx)
        {
            if (sz <= 0)
            {
                log(ERROR_LOG, "wolfssl receive error, size less than 0");
                return wolfssl.CBIO_ERR_GENERAL;
            }

            int amtRecv = 0;

            try
            {
                System.Runtime.InteropServices.GCHandle gch;
                gch = GCHandle.FromIntPtr(ctx);
                Socket con = (System.Net.Sockets.Socket)gch.Target;
                Byte[] msg = new Byte[sz];
                amtRecv = con.Receive(msg, msg.Length, 0);
                if (amtRecv == 0)
                {
                    /* No data received so check for a response to see if connection is still open */
                    if (con.Poll((con.ReceiveTimeout > 0) ? con.ReceiveTimeout : WC_WAIT, SelectMode.SelectRead))
                    {
                        log(ERROR_LOG, "socket connection issue, suspected connection termination.");
                        return wolfssl.CBIO_ERR_CONN_CLOSE;
                    }
                }
                Marshal.Copy(msg, 0, buf, sz);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "Error in receive " + e.ToString());
                return wolfssl.CBIO_ERR_CONN_CLOSE;
            }

            return amtRecv;
        }


        /// <summary>
        /// Call back used for sending TLS information
        /// </summary>
        /// <param name="ssl">pointer to ssl struct</param>
        /// <param name="buf">buffer containing information to send</param>
        /// <param name="sz">size of buffer to send</param>
        /// <param name="ctx">optional information</param>
        /// <returns>amount of information sent</returns>
        private static int wolfSSLCbIOSend(IntPtr ssl, IntPtr buf, int sz, IntPtr ctx)
        {
            if (sz <= 0)
            {
                log(ERROR_LOG, "wolfssl send error, size less than 0");
                return wolfssl.CBIO_ERR_GENERAL;
            }

            try
            {
                System.Runtime.InteropServices.GCHandle gch;
                gch = GCHandle.FromIntPtr(ctx);

                Socket con = (System.Net.Sockets.Socket)gch.Target;
                Byte[] msg = new Byte[sz];
                Marshal.Copy(buf, msg, 0, sz);
                if (con.Send(msg, 0, msg.Length, SocketFlags.None) == 0 && sz != 0)
                {
                    /* no data sent and msg size is larger then 0, check for lost connection */
                    if (con.Poll((con.SendTimeout > 0) ? con.SendTimeout : WC_WAIT, SelectMode.SelectWrite))
                    {
                        log(ERROR_LOG, "socket connection issue, suspect connection termination");
                        return wolfssl.CBIO_ERR_CONN_CLOSE;
                    }
                }
                return sz;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "socket connection issue " + e.ToString());
                return wolfssl.CBIO_ERR_CONN_CLOSE;
            }
        }


        /// <summary>
        /// Call back used for sending DTLS information
        /// </summary>
        /// <param name="ssl">pointer to ssl struct</param>
        /// <param name="buf">buffer containing information to send</param>
        /// <param name="sz">size of buffer to send</param>
        /// <param name="ctx">optional information</param>
        /// <returns>amount of information sent</returns>
        private static int wolfSSL_dtlsCbIOSend(IntPtr ssl, IntPtr buf, int sz, IntPtr ctx)
        {
            if (sz <= 0)
            {
                log(ERROR_LOG, "wolfssl dtls send error, size less than 0");
                return wolfssl.CBIO_ERR_GENERAL;
            }

            try
            {
                System.Runtime.InteropServices.GCHandle gch;
                gch = GCHandle.FromIntPtr(ctx);

                DTLS_con con = (DTLS_con)gch.Target;

                Byte[] msg = new Byte[sz];
                Marshal.Copy(buf, msg, 0, sz);
                con.udp.Send(msg, msg.Length, con.ep);
                return msg.Length;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "socket connection issue " + e.ToString());
                return wolfssl.CBIO_ERR_CONN_CLOSE;
            }
        }


        /// <summary>
        /// Call back to allow receiving DTLS information
        /// </summary>
        /// <param name="ssl">structure of ssl passed in</param>
        /// <param name="buf">buffer to contain received msg</param>
        /// <param name="sz">size of buffer</param>
        /// <param name="ctx">optional information passed in</param>
        /// <returns>size of message received</returns>
        private static int wolfSSL_dtlsCbIORecv(IntPtr ssl, IntPtr buf, int sz, IntPtr ctx)
        {
            if (sz <= 0)
            {
                log(ERROR_LOG, "wolfssl dtls receive error, size less than 0");
                return wolfssl.CBIO_ERR_GENERAL;
            }

            try
            {
                System.Runtime.InteropServices.GCHandle gch;
                gch = GCHandle.FromIntPtr(ctx);
                DTLS_con con = (DTLS_con)gch.Target;

                Byte[] msg = con.udp.Receive(ref con.ep);
                if (msg.Length > sz)
                {
                    log(ERROR_LOG, "wolfssl DTLS packet received was larger than buffer");
                    return wolfssl.CBIO_ERR_GENERAL;
                }

                Marshal.Copy(msg, 0, buf, msg.Length);
                return msg.Length;
            }
            catch (Exception e)
            {
                /* issue with receive or size of buffer */
                log(ERROR_LOG, "socket read issue " + e.ToString());
                return wolfssl.CBIO_ERR_CONN_CLOSE;
            }
        }


        /// <summary>
        /// Create a new ssl structure
        /// </summary>
        /// <param name="ctx">structure to create ssl structure from</param>
        /// <returns>pointer to ssl structure</returns>
        public static IntPtr new_ssl(IntPtr ctx)
        {
            if (ctx == IntPtr.Zero)
                return IntPtr.Zero;

            try
            {
                ssl_handle io;
                IntPtr local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "new_ssl ctx unwrap error");
                    return IntPtr.Zero;
                }

                io = new ssl_handle();
                io.set_ssl(wolfSSL_new(local_ctx));

                /* check if null */
                if (io.get_ssl() == IntPtr.Zero)
                {
                    return IntPtr.Zero;
                }

                /* keep memory pinned to be able to reference by address */
                return GCHandle.ToIntPtr(GCHandle.Alloc(io, GCHandleType.Pinned));
            }
            catch (Exception e)
            {
                log(ERROR_LOG, e.ToString());
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Used for a server to accept a connection
        /// </summary>
        /// <param name="ssl">structure containing info for connection</param>
        /// <returns>1 on success</returns>
        public static int accept(IntPtr ssl)
        {
            if (ssl == IntPtr.Zero)
                return FAILURE;
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "accept ssl unwrap error");
                    return FAILURE;
                }

                return wolfSSL_accept(sslCtx);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "accept error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Used for a client to connect
        /// </summary>
        /// <param name="ssl">structure containing connection info</param>
        /// <returns>1 on success</returns>
        public static int connect(IntPtr ssl)
        {
            if (ssl == IntPtr.Zero)
                return FAILURE;
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "connect ssl unwrap error");
                    return FAILURE;
                }

                return wolfSSL_connect(sslCtx);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "connect error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Read message from secure connection
        /// </summary>
        /// <param name="ssl">structure containing info about connection</param>
        /// <param name="buf">object to hold incoming message (Unicode format)</param>
        /// <param name="sz">size of available memory in buf</param>
        /// <returns>amount of data read on success</returns>
        public static int read(IntPtr ssl, StringBuilder buf, int sz)
        {
            if (ssl == IntPtr.Zero)
                return FAILURE;
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                IntPtr data;
                int ret;
                byte[] msg;

                buf.Clear(); /* Clear incoming buffer */

                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "read ssl unwrap error");
                    return FAILURE;
                }
                data = Marshal.AllocHGlobal(sz);

                ret = wolfSSL_read(sslCtx, data, sz);

                if (ret >= 0)
                {
                    /* Get data that was sent across and store it using a literal read of
                     * the conversion from bytes to character. Takes care of if
                     * a null terminator is part of the message read.
                     */
                    msg = new byte[ret];
                    Marshal.Copy(data, msg, 0, ret);
                    for (int i = 0; i < ret; i++)
                    {
                        buf.Append(@Convert.ToChar(msg[i]));
                    }
                }
                Marshal.FreeHGlobal(data);

                return ret;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl read error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Read message from secure connection using a byte array
        /// </summary>
        /// <param name="ssl">structure containing info about connection</param>
        /// <param name="buf">object to hold incoming message (raw bytes)</param>
        /// <param name="sz">size of available memory in buf</param>
        /// <returns>amount of data read on success</returns>
        public static int read(IntPtr ssl, byte[] buf, int sz)
        {
            if (ssl == IntPtr.Zero)
                return FAILURE;
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                IntPtr data;
                int ret;

                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "read ssl unwrap error");
                    return FAILURE;
                }
                data = Marshal.AllocHGlobal(sz);

                ret = wolfSSL_read(sslCtx, data, sz);

                if (ret >= 0)
                {
                    Marshal.Copy(data, buf, 0, ret);
                }
                Marshal.FreeHGlobal(data);

                return ret;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl read error " + e.ToString());
                return FAILURE;
            }
        }



        /// <summary>
        /// Write message to secure connection
        /// </summary>
        /// <param name="ssl">structure containing connection info</param>
        /// <param name="buf">message to send</param>
        /// <param name="sz">size of the message</param>
        /// <returns>amount sent on success</returns>
        public static int write(IntPtr ssl, StringBuilder buf, int sz)
        {
            if (ssl == IntPtr.Zero)
                return FAILURE;
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                IntPtr data;
                int ret;

                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "write ssl unwrap error");
                    return FAILURE;
                }

                data = Marshal.AllocHGlobal(sz);
                Marshal.Copy(System.Text.Encoding.Default.GetBytes(buf.ToString()), 0,
                       data, System.Text.Encoding.Default.GetByteCount(buf.ToString()));
                ret = wolfSSL_write(sslCtx, data, sz);
                Marshal.FreeHGlobal(data);
                return ret;

            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl write error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Write message to secure connection
        /// </summary>
        /// <param name="ssl">structure containing connection info</param>
        /// <param name="buf">message to send</param>
        /// <param name="sz">size of the message</param>
        /// <returns>amount sent on success</returns>
        public static int write(IntPtr ssl, byte[] buf, int sz)
        {
            if (ssl == IntPtr.Zero)
                return FAILURE;
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                IntPtr data;
                int ret;

                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "write ssl unwrap error");
                    return FAILURE;
                }
                data = Marshal.AllocHGlobal(sz);
                Marshal.Copy(buf, 0, data, sz);
                ret = wolfSSL_write(sslCtx, data, sz);
                Marshal.FreeHGlobal(data);
                return ret;

            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl write error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Free information stored in ssl struct
        /// </summary>
        /// <param name="ssl">pointer to ssl struct to free</param>
        public static void free(IntPtr ssl)
        {
            try
            {
                IntPtr sslCtx;
                GCHandle gch = GCHandle.FromIntPtr(ssl);
                ssl_handle handles = (ssl_handle)gch.Target;

                sslCtx = handles.get_ssl();
                wolfSSL_free(sslCtx);
                handles.free();
                gch.Free();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl free error " + e.ToString());
            }
        }


        /// <summary>
        /// Shutdown a connection
        /// </summary>
        /// <param name="ssl">pointer to ssl struct to close connection of</param>
        /// <returns>1 on success</returns>
        public static int shutdown(IntPtr ssl)
        {
            if (ssl == IntPtr.Zero)
                return FAILURE;
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "shutdown ssl unwrap error");
                    return FAILURE;
                }

                return wolfSSL_shutdown(sslCtx);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl shutdwon error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Optional, can be used to set a custom receive function
        /// </summary>
        /// <param name="ctx">structure to set receive function in</param>
        /// <param name="func">function to use when reading socket</param>
        public static void SetIORecv(IntPtr ctx, CallbackIORecv_delegate func)
        {
            try
            {
                GCHandle gch = GCHandle.FromIntPtr(ctx);
                ctx_handle handles = (ctx_handle)gch.Target;

                /* check if already stored handle needs freed */
                gch = handles.get_receive();
                if (!Object.Equals(gch, default(GCHandle)))
                {
                    gch.Free();
                }

                /* keep new function alive */
                handles.set_receive(GCHandle.Alloc(func));

                wolfSSL_CTX_SetIORecv(handles.get_ctx(), func);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl setIORecv error " + e.ToString());
            }
        }


        /// <summary>
        /// Optional, can be used to set a custom send function
        /// </summary>
        /// <param name="ctx">structure to set function in</param>
        /// <param name="func">function to use when sending data</param>
        public static void SetIOSend(IntPtr ctx, CallbackIOSend_delegate func)
        {
            try
            {
                GCHandle gch = GCHandle.FromIntPtr(ctx);
                ctx_handle handles = (ctx_handle)gch.Target;

                /* check if already stored handle needs freed */
                gch = handles.get_send();
                if (!Object.Equals(gch, default(GCHandle)))
                {
                    gch.Free();
                }

                /* keep new function alive */
                handles.set_send(GCHandle.Alloc(func));

                wolfSSL_CTX_SetIOSend(handles.get_ctx(), func);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl setIOSend error " + e.ToString());
            }
        }


        /// <summary>
        /// Create a new CTX structure
        /// </summary>
        /// <param name="method">method to use such as TLSv1.2</param>
        /// <returns>pointer to CTX structure</returns>
        public static IntPtr CTX_new(IntPtr method)
        {
            try
            {
                IntPtr ctx = wolfSSL_CTX_new(method);
                if (ctx == IntPtr.Zero)
                    return ctx;

                ctx_handle io = new ctx_handle();
                io.set_ctx(ctx);

                CallbackIORecv_delegate recv = new CallbackIORecv_delegate(wolfssl.wolfSSLCbIORecv);
                io.set_receive(GCHandle.Alloc(recv));
                wolfSSL_CTX_SetIORecv(ctx, recv);

                CallbackIOSend_delegate send = new CallbackIOSend_delegate(wolfssl.wolfSSLCbIOSend);
                io.set_send(GCHandle.Alloc(send));
                wolfSSL_CTX_SetIOSend(ctx, send);

                /* keep memory pinned */
                return GCHandle.ToIntPtr(GCHandle.Alloc(io, GCHandleType.Pinned));
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "ctx_new error " + e.ToString());
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Create a new CTX structure for a DTLS connection
        /// </summary>
        /// <param name="method">Method to use in connection ie DTLSv1.2</param>
        /// <returns></returns>
        public static IntPtr CTX_dtls_new(IntPtr method)
        {
            try
            {
                IntPtr ctx = wolfSSL_CTX_new(method);
                if (ctx == IntPtr.Zero)
                    return ctx;

                ctx_handle io = new ctx_handle();
                io.set_ctx(ctx);

                CallbackIORecv_delegate recv = new CallbackIORecv_delegate(wolfssl.wolfSSL_dtlsCbIORecv);
                io.set_receive(GCHandle.Alloc(recv));
                wolfSSL_CTX_SetIORecv(ctx, recv);

                CallbackIOSend_delegate send = new CallbackIOSend_delegate(wolfssl.wolfSSL_dtlsCbIOSend);
                io.set_send(GCHandle.Alloc(send));
                wolfSSL_CTX_SetIOSend(ctx, send);

                /* keep memory pinned */
                return GCHandle.ToIntPtr(GCHandle.Alloc(io, GCHandleType.Pinned));
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "ctx_dtls_new error " + e.ToString());
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Free information used in CTX structure
        /// </summary>
        /// <param name="ctx">structure to free</param>
        public static void CTX_free(IntPtr ctx)
        {
            try
            {
                GCHandle gch = GCHandle.FromIntPtr(ctx);
                ctx_handle handles = (ctx_handle)gch.Target;
                wolfSSL_CTX_free(handles.get_ctx());
                handles.free();
                gch.Free();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl ctx free error " + e.ToString());
            }
        }


        /// <summary>
        /// Set identity hint to use
        /// </summary>
        /// <param name="ctx">pointer to structure of ctx to set hint in</param>
        /// <param name="hint">hint to use</param>
        /// <returns>1 on success</returns>
        public static int CTX_use_psk_identity_hint(IntPtr ctx, StringBuilder hint)
        {
            try
            {
                IntPtr local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "CTX use psk identity hint unwrap error");
                    return FAILURE;
                }

                return wolfSSL_CTX_use_psk_identity_hint(local_ctx, hint);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl psk identity hint error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Set the function to use for PSK connections
        /// </summary>
        /// <param name="ctx">pointer to CTX that the function is set in</param>
        /// <param name="psk_cb">PSK function to use</param>
        public static void CTX_set_psk_server_callback(IntPtr ctx, psk_delegate psk_cb)
        {
            try
            {
                GCHandle gch = GCHandle.FromIntPtr(ctx);
                ctx_handle handles = (ctx_handle)gch.Target;

                handles.set_psk(GCHandle.Alloc(psk_cb));
                wolfSSL_CTX_set_psk_server_callback(handles.get_ctx(), psk_cb);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl psk server callback error " + e.ToString());
            }
        }


        /// <summary>
        /// Set the function to use for PSK connections
        /// </summary>
        /// <param name="ctx">pointer to CTX that the function is set in</param>
        /// <param name="psk_cb">PSK function to use</param>
        public static void CTX_set_psk_client_callback(IntPtr ctx, psk_client_delegate psk_cb)
        {
            try
            {
                GCHandle gch = GCHandle.FromIntPtr(ctx);
                ctx_handle handles = (ctx_handle)gch.Target;

                handles.set_psk(GCHandle.Alloc(psk_cb));
                wolfSSL_CTX_set_psk_client_callback(handles.get_ctx(), psk_cb);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl psk client callback error " + e.ToString());
            }
        }


        /// <summary>
        /// Set the function to use for PSK connections on a single TLS/DTLS connection
        /// </summary>
        /// <param name="ctx">pointer to SSL that the function is set in</param>
        /// <param name="psk_cb">PSK function to use</param>
        public static void set_psk_server_callback(IntPtr ssl, psk_delegate psk_cb)
        {
            try
            {
                GCHandle gch = GCHandle.FromIntPtr(ssl);
                ssl_handle handles = (ssl_handle)gch.Target;

                handles.set_psk(GCHandle.Alloc(psk_cb));
                wolfSSL_set_psk_server_callback(handles.get_ssl(), psk_cb);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl psk server callback error " + e.ToString());
            }
        }


        /// <summary>
        /// Set Socket for TLS connection
        /// </summary>
        /// <param name="ssl">structure to set Socket in</param>
        /// <param name="fd">Socket to use</param>
        /// <returns>1 on success</returns>
        public static int set_fd(IntPtr ssl, Socket fd)
        {
            /* sanity check on inputs */
            if (ssl == IntPtr.Zero)
            {
                return FAILURE;
            }

            try
            {
                if (!fd.Equals(null))
                {
                    GCHandle gch = GCHandle.FromIntPtr(ssl);
                    ssl_handle handles = (ssl_handle)gch.Target;
                    IntPtr sslCtx = handles.get_ssl();
                    IntPtr ptr;
                    GCHandle fd_pin = GCHandle.Alloc(fd);

                    if (sslCtx == IntPtr.Zero)
                    {
                        log(ERROR_LOG, "wolfssl error setting up fd!!");
                        return FAILURE;
                    }

                    handles.set_fd(fd_pin);
                    ptr = GCHandle.ToIntPtr(fd_pin);
                    wolfSSL_SetIOWriteCtx(sslCtx, ptr); //pass along the socket for writing to
                    wolfSSL_SetIOReadCtx(sslCtx, ptr); //pass along the socket for reading from

                    return SUCCESS;
                }

                return FAILURE;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "Error setting up fd!! " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Get socket of a TLS connection
        /// </summary>
        /// <param name="ssl">structure to get socket from</param>
        /// <returns>Socket object used for connection</returns>
        public static Socket get_fd(IntPtr ssl)
        {
            try
            {
                IntPtr ptr;
                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "wolfssl get_fd error");
                    return null;
                }

                ptr = wolfSSL_GetIOReadCtx(sslCtx);
                if (ptr != IntPtr.Zero)
                {
                    GCHandle gch = GCHandle.FromIntPtr(ptr);
                    return (System.Net.Sockets.Socket)gch.Target;
                }
                return null;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl get_fd error " + e.ToString());
                return null;
            }
        }



        /// <summary>
        /// Set information needed to send and receive a DTLS connection
        /// </summary>
        /// <param name="ssl">structure to set information in</param>
        /// <param name="udp">UDP object to send and receive</param>
        /// <param name="ep">End point of connection</param>
        /// <returns>1 on success</returns>
        public static int set_dtls_fd(IntPtr ssl, UdpClient udp, IPEndPoint ep)
        {
            /* sanity check on inputs */
            if (ssl == IntPtr.Zero)
            {
                return FAILURE;
            }

            try
            {
                if (!udp.Equals(null) && !ep.Equals(null))
                {
                    IntPtr ptr;
                    DTLS_con con;
                    GCHandle gch = GCHandle.FromIntPtr(ssl);
                    ssl_handle handles = (ssl_handle)gch.Target;
                    GCHandle fd_pin;

                    con = new DTLS_con();
                    con.udp = udp;
                    con.ep = ep;
                    fd_pin = GCHandle.Alloc(con);
                    handles.set_fd(fd_pin);
                    ptr = GCHandle.ToIntPtr(fd_pin);
                    wolfSSL_SetIOWriteCtx(handles.get_ssl(), ptr); //pass along the socket for writing to
                    wolfSSL_SetIOReadCtx(handles.get_ssl(), ptr); //pass along the socket for reading from

                    return SUCCESS;
                }
                return FAILURE;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "Error setting up fd!! " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Get the pointer to DTLS_con class used for connection
        /// </summary>
        /// <param name="ssl">structure to get connection from</param>
        /// <returns>DTLS_con object</returns>
        public static DTLS_con get_dtls_fd(IntPtr ssl)
        {
            try
            {
                IntPtr ptr;
                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "wolfssl get_dtls_fd error");
                    return null;
                }

                ptr = wolfSSL_GetIOReadCtx(sslCtx);
                if (ptr != IntPtr.Zero)
                {
                    GCHandle gch = GCHandle.FromIntPtr(ptr);
                    return (DTLS_con)gch.Target;
                }
                return null;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl get_dtls_fd error " + e.ToString());
                return null;
            }
        }


        /// <summary>
        /// Get available cipher suites
        /// </summary>
        /// <param name="list">list to fill with cipher suite names</param>
        /// <param name="sz">size of list available to fill</param>
        /// <returns>1 on success</returns>
        public static int get_ciphers(StringBuilder list, int sz)
        {
            try
            {
                return wolfSSL_get_ciphers(list, sz);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl get_ciphers error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Initialize wolfSSL library
        /// </summary>
        /// <returns>1 on success</returns>
        public static int Init()
        {
            try
            {
                return wolfSSL_Init();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl init error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Clean up wolfSSL library memory
        /// </summary>
        /// <returns>1 on success</returns>
        public static int Cleanup()
        {
            try
            {
                return wolfSSL_Cleanup();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl cleanup error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Set up TLS version 1.2 method
        /// </summary>
        /// <returns>pointer to TLSv1.2 method</returns>
        public static IntPtr useTLSv1_2_server()
        {
            try
            {
                return wolfTLSv1_2_server_method();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl error " + e.ToString());
                return IntPtr.Zero;
            }
        }

        /// <summary>
        /// Set up TLS version 1.3 method
        /// </summary>
        /// <returns>pointer to TLSv1.3 method</returns>
        public static IntPtr useTLSv1_3_server()
        {
            try
            {
                return wolfTLSv1_3_server_method();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl error " + e.ToString());
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Use any TLS version
        /// </summary>
        /// <returns>pointer to method</returns>
        public static IntPtr usev23_server()
        {
            try
            {
                return wolfSSLv23_server_method();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl error " + e.ToString());
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Set up TLS version 1.2 method
        /// </summary>
        /// <returns>pointer to TLSv1.2 method</returns>
        public static IntPtr useTLSv1_2_client()
        {
            try
            {
                return wolfTLSv1_2_client_method();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl error " + e.ToString());
                return IntPtr.Zero;
            }
        }

        /// <summary>
        /// Set up TLS version 1.3 method
        /// </summary>
        /// <returns>pointer to TLSv1.3 method</returns>
        public static IntPtr useTLSv1_3_client()
        {
            try
            {
                return wolfTLSv1_3_client_method();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl error " + e.ToString());
                return IntPtr.Zero;
            }
        }

        /// <summary>
        /// Use any TLS version
        /// </summary>
        /// <returns>pointer to method</returns>
        public static IntPtr usev23_client()
        {
            try
            {
                return wolfSSLv23_client_method();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl error " + e.ToString());
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Set up DTLS version 1.2
        /// </summary>
        /// <returns>pointer to DTLSv1.2 method</returns>
        public static IntPtr useDTLSv1_2_server()
        {
            try
            {
                return wolfDTLSv1_2_server_method();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl error " + e.ToString());
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Set up DTLS version 1.2
        /// </summary>
        /// <returns>pointer to DTLSv1.2 method</returns>
        public static IntPtr useDTLSv1_2_client()
        {
            try
            {
                return wolfDTLSv1_2_client_method();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl error " + e.ToString());
                return IntPtr.Zero;
            }
        }


        /// <summary>
        /// Gets the current cipher suite being used in connection
        /// </summary>
        /// <param name="ssl">SSL struct to get cipher suite from</param>
        /// <returns>string containing current cipher suite</returns>
        public static string get_current_cipher(IntPtr ssl)
        {
            if (ssl == IntPtr.Zero)
                return null;
            try
            {
                IntPtr ssl_cipher;
                IntPtr ssl_cipher_ptr;
                string ssl_cipher_str;

                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "wolfssl get_current_cipher error");
                    return null;
                }

                ssl_cipher = wolfSSL_get_current_cipher(sslCtx);
                ssl_cipher_ptr = wolfSSL_CIPHER_get_name(ssl_cipher);
                ssl_cipher_str = Marshal.PtrToStringAnsi(ssl_cipher_ptr);

                return ssl_cipher_str;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl get current cipher error " + e.ToString());
                return null;
            }
        }


        /// <summary>
        /// Set available cipher suites for all ssl structs created from ctx
        /// </summary>
        /// <param name="ctx">CTX structure to set</param>
        /// <param name="list">List full of ciphers suites</param>
        /// <returns>1 on success</returns>
        public static int CTX_set_cipher_list(IntPtr ctx, StringBuilder list)
        {
            try
            {
                IntPtr local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "CTX set cipher list error");
                    return FAILURE;
                }

                return wolfSSL_CTX_set_cipher_list(local_ctx, list);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl ctx set cipher list error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Set available cipher suite in local connection
        /// </summary>
        /// <param name="ssl">Structure to set cipher suite in</param>
        /// <param name="list">List of cipher suites</param>
        /// <returns>1 on success</returns>
        public static int set_cipher_list(IntPtr ssl, StringBuilder list)
        {
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "wolfssl set_cipher_list error");
                    return FAILURE;
                }

                return wolfSSL_set_cipher_list(sslCtx, list);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl set cipher error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Gets the version of the connection made ie TLSv1.2
        /// </summary>
        /// <param name="ssl">SSL struct to get version of</param>
        /// <returns>string containing version</returns>
        public static string get_version(IntPtr ssl)
        {
            if (ssl == IntPtr.Zero)
                return null;

            try
            {
                IntPtr version_ptr;
                string version;

                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "wolfssl get_version error");
                    return null;
                }

                version_ptr = wolfSSL_get_version(sslCtx);
                version = Marshal.PtrToStringAnsi(version_ptr);

                return version;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl get version error " + e.ToString());
                return null;
            }
        }


        /// <summary>
        /// Get a string containing error value and reason
        /// </summary>
        /// <param name="ssl">SSL struct that had error</param>
        /// <returns>String containing error value and reason</returns>
        public static string get_error(IntPtr ssl)
        {
            if (ssl == IntPtr.Zero)
                return null;

            try
            {
                int err;
                StringBuilder err_name;
                StringBuilder ret;

                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "wolfssl get_error error");
                    return null;
                }

                /* wolfSSL max error length is 80 */
                ret = new StringBuilder(' ', 100);
                err = wolfSSL_get_error(sslCtx, 0);
                err_name = new StringBuilder(new String(' ', 80));
                wolfSSL_ERR_error_string((uint)err, err_name);
                ret.Append("Error " + err + " " + err_name.ToString());

                return ret.ToString();
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl get error, error " + e.ToString());
                return null;
            }
        }


        /// <summary>
        /// Used to load in the certificate file
        /// </summary>
        /// <param name="ctx">CTX structure for TLS/SSL connections</param>
        /// <param name="fileCert">Name of the file to load including absolute path</param>
        /// <param name="type">Type of file ie PEM or DER</param>
        /// <returns>1 on success</returns>
        public static int CTX_use_certificate_file(IntPtr ctx, string fileCert, int type)
        {
            try
            {
                IntPtr local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "CTX use certificate file error");
                    return FAILURE;
                }

                return wolfSSL_CTX_use_certificate_file(local_ctx, fileCert, type);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl ctx use cert file error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Used to load in the peer trusted root file
        /// </summary>
        /// <param name="ctx">CTX structure for TLS/SSL connections</param>
        /// <param name="fileCert">Name of the file to load including absolute path</param>
        /// <param name="type">path to multiple certificates (try to load all in path) </param>
        /// <returns>1 on success</returns>
        public static int CTX_load_verify_locations(IntPtr ctx, string fileCert, string path)
        {
            try
            {
                IntPtr local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "CTX load verify locations certificate file error");
                    return FAILURE;
                }

                return wolfSSL_CTX_load_verify_locations(local_ctx, fileCert, path);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl ctx load verify locations file error " + e.ToString());
                return FAILURE;
            }
        }

        /// <summary>
        /// Used to load in the private key from a file
        /// </summary>
        /// <param name="ctx">CTX structure for TLS/SSL connections </param>
        /// <param name="fileKey">Name of the file, including absolute directory</param>
        /// <param name="type">Type of file ie PEM or DER</param>
        /// <returns>1 on success</returns>
        public static int CTX_use_PrivateKey_file(IntPtr ctx, string fileKey, int type)
        {
            try
            {
                IntPtr local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "CTX use PrivateKey file error");
                    return FAILURE;
                }

                return wolfSSL_CTX_use_PrivateKey_file(local_ctx, fileKey, type);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl ctx use key file error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Set temporary DH parameters
        /// </summary>
        /// <param name="ssl">Structure to set in</param>
        /// <param name="dhparam">file name</param>
        /// <param name="file_type">type of file ie PEM</param>
        /// <returns>1 on success</returns>
        public static int SetTmpDH_file(IntPtr ssl, StringBuilder dhparam, int file_type)
        {
            try
            {
                IntPtr sslCtx = unwrap_ssl(ssl);
                if (sslCtx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "SetTmpDH_file ssl unwrap error");
                    return FAILURE;
                }

                return wolfSSL_SetTmpDH_file(sslCtx, dhparam, file_type);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "SetTmpDH_file error " + e.ToString());
                return FAILURE;
            }
        }

        /// <summary>
        /// Set temporary DH parameters
        /// </summary>
        /// <param name="ctx">Structure to set in</param>
        /// <param name="dhparam">file name</param>
        /// <param name="file_type">type of file ie PEM</param>
        /// <returns>1 on success</returns>
        public static int CTX_SetTmpDH_file(IntPtr ctx, StringBuilder dhparam, int file_type)
        {
            try
            {
                IntPtr local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "CTX_SetTmpDH_file ctx unwrap error");
                    return FAILURE;
                }

                return wolfSSL_CTX_SetTmpDH_file(local_ctx, dhparam, file_type);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "CTX_SetTmpDH_file error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Used to set the minimum size of DH key
        /// </summary>
        /// <param name="ctx">Structure to store key size</param>
        /// <param name="minDhKey">Min key size </param>
        /// <returns>1 on success</returns>
        public static int CTX_SetMinDhKey_Sz(IntPtr ctx, short minDhKey)
        {
            try
            {
                IntPtr local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "CTX SetMinDhKey_Sz error");
                    return FAILURE;
                }

                return wolfSSL_CTX_SetMinDhKey_Sz(local_ctx, minDhKey);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl ctx set min dh key error " + e.ToString());
                return FAILURE;
            }
        }

        /// <summary>
        /// Set the certificate verification mode and optional callback function
        /// </summary>
        /// <param name="ctx">pointer to CTX that the function is set in</param>
        /// <param name="mode">See SSL_VERIFY options</param>
        /// <param name="vc">Optional verify callback function to use</param>
        public static int CTX_set_verify(IntPtr ctx, int mode, CallbackVerify_delegate vc)
        {
            try
            {
                GCHandle   gch;
                ctx_handle handles;
                IntPtr     local_ctx = unwrap_ctx(ctx);
                if (local_ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "CTX set_verify error");
                    return FAILURE;
                }

                /* pin the verify callback to protect from garbage collection */
                if (!vc.Equals(null)) {
                    gch = GCHandle.FromIntPtr(ctx);
                    handles = (ctx_handle)gch.Target;
                    handles.set_vrf(GCHandle.Alloc(vc));
                }

                wolfSSL_CTX_set_verify(local_ctx, mode, vc);
                return SUCCESS;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl ctx set verify error " + e.ToString());
                return FAILURE;
            }
        }

        /// <summary>
        /// Set the certificate verification mode and optional callback function
        /// </summary>
        /// <param name="ctx">pointer to SSL object that the function is set in</param>
        /// <param name="mode">See SSL_VERIFY options</param>
        /// <param name="vc">Optional verify callback function to use</param>
        public static int set_verify(IntPtr ssl, int mode, CallbackVerify_delegate vc)
        {
            try
            {
                GCHandle   gch;
                ssl_handle handles;
                IntPtr     local_ssl = unwrap_ssl(ssl);
                if (local_ssl == IntPtr.Zero)
                {
                    log(ERROR_LOG, "set_verify error");
                    return FAILURE;
                }

                /* pin the verify callback to protect from garbage collection */
                if (!vc.Equals(null)) {
                    gch = GCHandle.FromIntPtr(ssl);
                    handles = (ssl_handle)gch.Target;
                    handles.set_vrf(GCHandle.Alloc(vc));
                }

                wolfSSL_set_verify(local_ssl, mode, vc);
                return SUCCESS;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl set verify error " + e.ToString());
                return FAILURE;
            }
        }


        /// <summary>
        /// Set the certificate verification mode and optional callback function
        /// </summary>
        /// <param name="ctx">pointer to SSL object that the function is set in</param>
        /// <param name="mode">See SSL_VERIFY options</param>
        /// <param name="vc">Optional verify callback function to use</param>
        public static X509 X509_STORE_CTX_get_current_cert(IntPtr x509Ctx)
        {
            X509 ret = null;
            try
            {
                if (x509Ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "pointer passed in was not set");
                    return ret;
                }
                IntPtr x509 = wolfSSL_X509_STORE_CTX_get_current_cert(x509Ctx);
                if (x509 != IntPtr.Zero) {
                    return new X509(x509, false);
                }
                return ret;
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl WOLFSSL_X509_STORE_CTX error " + e.ToString());
                return ret;
            }
        }


        /// <summary>
        /// Gets all of the certificates from store
        /// </summary>
        /// <param name="x509Ctx">pointer to store to get certificates from</param>
        public static X509[] X509_STORE_CTX_get_certs(IntPtr x509Ctx)
        {
            X509[] ret = null;
            try
            {
                if (x509Ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "pointer passed in was not set");
                    return ret;
                }
                IntPtr sk = wolfSSL_X509_STORE_GetCerts(x509Ctx);
                if (sk != IntPtr.Zero) {
                    int i;
                    int numCerts = wolfSSL_sk_X509_num(sk);
                    ret = new X509[numCerts];

                    for (i = 0; i < numCerts; i++) {
                        IntPtr current = wolfSSL_sk_X509_pop(sk);
                        if (current != IntPtr.Zero)
                        {
                            ret[i] = new X509(current, true);
                        }
                    }
                    wolfSSL_sk_X509_free(sk);
                }
                return ret;

            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl WOLFSSL_X509_STORE_CTX error " + e.ToString());
                return ret;
            }
        }


        /// <summary>
        /// Get the current WOLFSSL_X509_STORE_CTX error value
        /// </summary>
        /// <param name="x509Ctx">pointer to store to get error from</param>
        public static int X509_STORE_CTX_get_error(IntPtr x509Ctx)
        {
            try
            {
                if (x509Ctx == IntPtr.Zero)
                {
                    log(ERROR_LOG, "pointer passed in was not set");
                    return -1;
                }
                return wolfSSL_X509_STORE_CTX_get_error(x509Ctx);
            }
            catch (Exception e)
            {
                log(ERROR_LOG, "wolfssl WOLFSSL_X509_STORE_CTX error " + e.ToString());
                return -1;
            }
        }

        /// <summary>
        /// Print low level C library debug messages to stdout when compiled with macro DEBUG_WOLFSSL
        /// </summary>
        public static void Debugging_ON()
        {
            wolfSSL_Debugging_ON();
        }

        /// <summary>
        /// Turn off low level C debug messages
        /// </summary>
        public static void Debugging_OFF()
        {
            wolfSSL_Debugging_OFF();
        }

        /// <summary>
        /// Set the function to use for logging
        /// </summary>
        /// <param name="input">Function that conforms as to loggingCb</param>
        /// <returns>1 on success</returns>
        public static int SetLogging(loggingCb input)
        {
            internal_log = input;
            return SUCCESS;
        }


        /// <summary>
        /// Log a message to set logging function
        /// </summary>
        /// <param name="lvl">Level of log message</param>
        /// <param name="msg">Message to log</param>
        public static void log(int lvl, string msg)
        {
            /* if log is not set then print nothing */
            if (internal_log == null)
                return;
            StringBuilder ptr = new StringBuilder(msg);
            internal_log(lvl, ptr);
        }
    }
}

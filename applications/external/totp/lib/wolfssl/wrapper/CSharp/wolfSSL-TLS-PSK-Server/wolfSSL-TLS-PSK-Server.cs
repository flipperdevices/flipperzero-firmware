/* wolfSSL-TLS-PSK-Server.cs
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
using wolfSSL.CSharp;



public class wolfSSL_TLS_PSK_Server
{


    /// <summary>
    /// Example of a PSK function call back
    /// </summary>
    /// <param name="ssl">pointer to ssl structure</param>
    /// <param name="identity">identity of client connecting</param>
    /// <param name="key">buffer to hold key</param>
    /// <param name="max_key">max key size</param>
    /// <returns>size of key set</returns>
    public static uint my_psk_server_cb(IntPtr ssl, string identity, IntPtr key, uint max_key)
    {
        /* perform a check on the identity sent across
         * log function must be set for print out of logging information
         */
        wolfssl.log(wolfssl.INFO_LOG, "PSK Client Identity = " + identity);

        /* Use desired key, note must be a key smaller than max key size parameter
            Replace this with desired key. Is trivial one for testing */
        if (max_key < 4)
            return 0;
        byte[] tmp = { 26, 43, 60, 77 };
        Marshal.Copy(tmp, 0, key, 4);

        return (uint)4;
    }


    private static void clean(IntPtr ssl, IntPtr ctx)
    {
        wolfssl.free(ssl);
        wolfssl.CTX_free(ctx);
        wolfssl.Cleanup();
    }


    public static void Main(string[] args)
    {
        IntPtr ctx;
        IntPtr ssl;
        Socket fd;

        wolfssl.psk_delegate psk_cb = new wolfssl.psk_delegate(my_psk_server_cb);

        /* These paths should be changed according to use */
        string fileCert = @"server-cert.pem";
        string fileKey = @"server-key.pem";
        StringBuilder dhparam = new StringBuilder("dh2048.pem");

        StringBuilder buff = new StringBuilder(1024);
        StringBuilder reply = new StringBuilder("Hello, this is the wolfSSL C# wrapper");

        wolfssl.Init();

        Console.WriteLine("Calling ctx Init from wolfSSL");
        ctx = wolfssl.CTX_new(wolfssl.useTLSv1_2_server());
        if (ctx == IntPtr.Zero)
        {
            Console.WriteLine("Error creating ctx structure");
            return;
        }
        Console.WriteLine("Finished init of ctx .... now load in cert and key");

        if (!File.Exists(fileCert) || !File.Exists(fileKey))
        {
            Console.WriteLine("Could not find cert or key file");
            wolfssl.CTX_free(ctx);
            return;
        }

        if (wolfssl.CTX_use_certificate_file(ctx, fileCert, wolfssl.SSL_FILETYPE_PEM) != wolfssl.SUCCESS)
        {
            Console.WriteLine("Error in setting cert file");
            wolfssl.CTX_free(ctx);
            return;
        }

        if (wolfssl.CTX_use_PrivateKey_file(ctx, fileKey, wolfssl.SSL_FILETYPE_PEM) != wolfssl.SUCCESS)
        {
            Console.WriteLine("Error in setting key file");
            wolfssl.CTX_free(ctx);
            return;
        }


        StringBuilder ciphers = new StringBuilder(new String(' ', 4096));
        wolfssl.get_ciphers(ciphers, 4096);
        Console.WriteLine("Ciphers : " + ciphers.ToString());

        short minDhKey = 128;
        wolfssl.CTX_SetMinDhKey_Sz(ctx, minDhKey);
        Console.Write("Setting cipher suite to ");

        /* In order to use static PSK build wolfSSL with the preprocessor flag WOLFSSL_STATIC_PSK */
        StringBuilder set_cipher = new StringBuilder("DHE-PSK-AES128-CBC-SHA256");
        Console.WriteLine(set_cipher);
        if (wolfssl.CTX_set_cipher_list(ctx, set_cipher) != wolfssl.SUCCESS)
        {
            Console.WriteLine("Failed to set cipher suite");
            return;
        }

        /* Test psk use with DHE */
        StringBuilder hint = new StringBuilder("cyassl server");
        if (wolfssl.CTX_use_psk_identity_hint(ctx, hint) != wolfssl.SUCCESS)
        {
            Console.WriteLine("Error setting hint");
            wolfssl.CTX_free(ctx);
            return;
        }
        wolfssl.CTX_set_psk_server_callback(ctx, psk_cb);

        /* set up TCP socket */
        IPAddress ip = IPAddress.Parse("0.0.0.0"); //bind to any
        TcpListener tcp = new TcpListener(ip, 11111);
        tcp.Start();

        Console.WriteLine("Started TCP and waiting for a connection");
        fd = tcp.AcceptSocket();
        ssl = wolfssl.new_ssl(ctx);
        if (ssl == IntPtr.Zero)
        {
            Console.WriteLine("Error creating ssl object");
            tcp.Stop();
            wolfssl.CTX_free(ctx);
            return;
        }

        Console.WriteLine("Connection made wolfSSL_accept ");
        if (wolfssl.set_fd(ssl, fd) != wolfssl.SUCCESS)
        {
            /* get and print out the error */
            Console.WriteLine(wolfssl.get_error(ssl));
            tcp.Stop();
            clean(ssl, ctx);
            return;
        }

        wolfssl.SetTmpDH_file(ssl, dhparam, wolfssl.SSL_FILETYPE_PEM);

        if (wolfssl.accept(ssl) != wolfssl.SUCCESS)
        {
            /* get and print out the error */
            Console.WriteLine(wolfssl.get_error(ssl));
            tcp.Stop();
            clean(ssl, ctx);
            return;
        }

        /* print out results of TLS/SSL accept */
        Console.WriteLine("SSL version is " + wolfssl.get_version(ssl));
        Console.WriteLine("SSL cipher suite is " + wolfssl.get_current_cipher(ssl));

        /* read and print out the message then reply */
        if (wolfssl.read(ssl, buff, 1023) < 0)
        {
            Console.WriteLine("Error in read");
            tcp.Stop();
            clean(ssl, ctx);
            return;
        }
        Console.WriteLine(buff);

        if (wolfssl.write(ssl, reply, reply.Length) != reply.Length)
        {
            Console.WriteLine("Error in write");
            tcp.Stop();
            clean(ssl, ctx);
            return;
        }

        wolfssl.shutdown(ssl);
        fd.Close();
        tcp.Stop();
        clean(ssl, ctx);
    }
}

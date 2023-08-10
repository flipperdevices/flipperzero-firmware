/* wolfSSL-TLS-Server.cs
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
using System.IO;
using System.Net;
using System.Net.Sockets;
using wolfSSL.CSharp;

public class wolfSSL_TLS_CSHarp
{
    /// <summary>
    /// Example of a logging function
    /// </summary>
    /// <param name="lvl">level of log</param>
    /// <param name="msg">message to log</param>
    public static void standard_log(int lvl, StringBuilder msg)
    {
        Console.WriteLine(msg);
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

        /* These paths should be changed for use */
        string fileCert = @"server-cert.pem";
        string fileKey = @"server-key.pem";
        StringBuilder dhparam = new StringBuilder("dh2048.pem");

        StringBuilder buff = new StringBuilder(1024);
        StringBuilder reply = new StringBuilder("Hello, this is the wolfSSL C# wrapper");

        //example of function used for setting logging
        wolfssl.SetLogging(standard_log);

        wolfssl.Init();


        Console.WriteLine("Calling ctx Init from wolfSSL");
        ctx = wolfssl.CTX_new(wolfssl.usev23_server());
        if (ctx == IntPtr.Zero)
        {
            Console.WriteLine("Error in creating ctx structure");
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

        /* set up TCP socket */
        IPAddress ip = IPAddress.Parse("0.0.0.0"); /* bind to any */
        TcpListener tcp = new TcpListener(ip, 11111);
        tcp.Start();

        Console.WriteLine("Started TCP and waiting for a connection");
        fd = tcp.AcceptSocket();
        ssl = wolfssl.new_ssl(ctx);
        if (ssl == IntPtr.Zero)
        {
            Console.WriteLine("Error in creating ssl object");
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

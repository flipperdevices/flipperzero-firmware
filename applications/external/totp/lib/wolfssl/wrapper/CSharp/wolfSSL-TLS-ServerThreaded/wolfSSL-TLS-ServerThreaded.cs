/* wolfSSL-TLS-ServerThreaded.cs
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
using System.Threading;

using wolfSSL.CSharp;

public class wolfSSL_TLS_ServerThread
{
    private IntPtr _ctx;
    private Socket _fd;

    public wolfSSL_TLS_ServerThread(IntPtr ctx, Socket fd)
    {
        _ctx = ctx;
        _fd = fd;
    }

    private const int kEchoBufSz = 1024;
    public void start_client()
    {
        StringBuilder buff = new StringBuilder(kEchoBufSz);
        IntPtr ssl = wolfssl.new_ssl(_ctx);
        if (ssl == IntPtr.Zero)
        {
            Console.WriteLine("Error in creating ssl object");
            return;
        }

        if (wolfssl.set_fd(ssl, _fd) != wolfssl.SUCCESS)
        {
            /* get and print out the error */
            Console.WriteLine(wolfssl.get_error(ssl));
            _fd.Close();
            wolfssl.free(ssl);
            return;
        }

        Console.WriteLine("Starting TLS handshake");
        if (wolfssl.accept(ssl) != wolfssl.SUCCESS)
        {
            /* get and print out the error */
            Console.WriteLine("Failed " + wolfssl.get_error(ssl));
            _fd.Close();
            wolfssl.free(ssl);
            return;
        }

        /* print out results of TLS/SSL accept */
        Console.WriteLine("SSL version is " + wolfssl.get_version(ssl));
        Console.WriteLine("SSL cipher suite is " + wolfssl.get_current_cipher(ssl));

        /* echo data until error */
        while (true)
        {
            /* read and print out the message then reply */
            if (wolfssl.read(ssl, buff, kEchoBufSz-1) < 0)
            {
                Console.WriteLine("Error in read");
                break;
            }
            Console.WriteLine(buff);

            if (wolfssl.write(ssl, buff, buff.Length) != buff.Length)
            {
                Console.WriteLine("Error in write");
                break;
            }
        }

        Console.WriteLine("Closing " + wolfssl.get_error(ssl));
        _fd.Close();
        wolfssl.free(ssl);
    }
}

public class wolfSSL_TLS_ServerThreaded
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

    public static void Main(string[] args)
    {
        IntPtr ctx;

        /* These paths should be changed for use */
        string fileCert = @"server-cert.pem";
        string fileKey = @"server-key.pem";
        StringBuilder dhparam = new StringBuilder("dh2048.pem");

        /* example of function used for setting logging */
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
        wolfssl.CTX_SetTmpDH_file(ctx, dhparam, wolfssl.SSL_FILETYPE_PEM);

        /* set up TCP socket */
        IPAddress ip = IPAddress.Parse("0.0.0.0"); /* bind to any */
        TcpListener tcp = new TcpListener(ip, 11111);
        tcp.Start();

        Console.WriteLine("Started TCP and waiting for a connection");

        while (true) {
            try
            {
                Socket fd = tcp.AcceptSocket();
                Console.WriteLine("Got client connection");

                /* Spin up thread for client */
                wolfSSL_TLS_ServerThread thread = new wolfSSL_TLS_ServerThread(ctx, fd);
                Thread thr = new Thread(new ThreadStart(thread.start_client));
                thr.Start();
            }
            catch(Exception ex)
            {
                Console.WriteLine("Server Exception " + ex.ToString());
                break;
            }
        }

        tcp.Stop();
        wolfssl.CTX_free(ctx);
        wolfssl.Cleanup();
    }
}
